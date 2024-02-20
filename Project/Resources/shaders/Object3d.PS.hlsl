#include "Object3d.hlsli"

Texture2D<float32_t4> gTexture : register(t0);
SamplerState gSampler : register(s0);

struct Material {
	float32_t4 color;
	int32_t enableLighting;
	float32_t4x4 uvTransform;
	float32_t shininess;
};

struct DirectionalLight {
	float32_t4 color;
	float32_t3 direction;
	float intencity;
};

struct Camera {
	float32_t3 worldPosition;
};

struct PointLight {
	float32_t4 color; // ライト色
	float32_t3 position; // ライトの位置
	float intencity; // 輝度
	float radius; // ライトの届く最大距離
	float decay; // 減衰率
	bool used; // 使用している
};

struct SpotLight {
	float32_t4 color; // ライト色
	float32_t3 position; // ライトの位置
	float32_t intencity; // 輝度
	float32_t3 direction; // スポットライトの方向
	float32_t distance; // ライトの届く最大距離
	float32_t decay; // 減衰率
	float32_t cosAngle; // スポットライトの余弦
	float32_t cosFalloffStart; // フォールオフ開始位置
};

ConstantBuffer<Material> gMaterial : register(b0);

ConstantBuffer<DirectionalLight> gDirectionalLight : register(b1);

ConstantBuffer<Camera> gCamera : register(b2);

ConstantBuffer<PointLight> gPointLight : register(b3);

ConstantBuffer<SpotLight> gSpotLight : register(b4);

StructuredBuffer<PointLight> gPointLights : register(t1);

struct PointLightCalcData {
	float32_t3 pointLightDirection;
	float32_t pointFactor;
	bool used;
};

struct PixelShaderOutput {
	float32_t4 color : SV_TARGET0;
};

/// <summary>
/// ランバート
/// </summary>
float32_t4 Lambert(VertexShaderOutput input, float32_t4 textureColor, float32_t3 pointLightDirection, float32_t pointFactor,
	float32_t3 spotLightDirectionOnSuface, float32_t spotFactor) {

	float32_t4 color;

	// 平行光源
	float directionalLightCos = saturate(dot(normalize(input.normal), -gDirectionalLight.direction));
	float32_t3 directionalLightColor = gDirectionalLight.color.rgb * directionalLightCos * gDirectionalLight.intencity;

	// ポイントライト
	float pointLightCos = saturate(dot(normalize(input.normal), -pointLightDirection));
	float32_t3 pointLightColor = gPointLight.color.rgb * pointLightCos * gPointLight.intencity * pointFactor;

	// スポットライト
	float spotLightCos = saturate(dot(normalize(input.normal), -spotLightDirectionOnSuface));
	float32_t3 spotLightColor = gSpotLight.color.rgb * spotLightCos * gSpotLight.intencity * spotFactor;

	//// 全てのライトデータを入れる
	color.rgb = gMaterial.color.rgb * textureColor.rgb * (directionalLightColor + pointLightColor + spotLightColor);
	color.a = gMaterial.color.a * textureColor.a;

	return color;

}

/// <summary>
/// 半ランバート
/// </summary>
float32_t4 HalfLambert(VertexShaderOutput input, float32_t4 textureColor, float32_t3 pointLightDirection, float32_t pointFactor,
	float32_t3 spotLightDirectionOnSuface, float32_t spotFactor) {

	float32_t4 color;

	// 平行光源
	float directionalLightNdotL = dot(normalize(input.normal), -gDirectionalLight.direction);
	float directionalLightCos = pow(directionalLightNdotL * 0.5f + 0.5f, 2.0f);
	float32_t3 directionalLightColor = gDirectionalLight.color.rgb * directionalLightCos * gDirectionalLight.intencity;

	// ポイントライト
	float pointLightNdotL = dot(normalize(input.normal), -pointLightDirection);
	float pointLightCos = pow(pointLightNdotL * 0.5f + 0.5f, 2.0f);
	float32_t3 pointLightColor = gPointLight.color.rgb * pointLightCos * gPointLight.intencity * pointFactor;

	// スポットライト
	float spotLightNdotL = dot(normalize(input.normal), -spotLightDirectionOnSuface);
	float spotLightCos = pow(spotLightNdotL * 0.5f + 0.5f, 2.0f);
	float32_t3 spotLightColor = gSpotLight.color.rgb * spotLightCos * gSpotLight.intencity * spotFactor;

	// 全てのライトデータを入れる
	color.rgb = gMaterial.color.rgb * textureColor.rgb * (directionalLightColor + pointLightColor + spotLightColor);
	color.a = gMaterial.color.a * textureColor.a;

	return color;

}

/// <summary>
/// 鏡面反射
/// </summary>
float32_t4 PhongReflection(VertexShaderOutput input, float32_t4 textureColor,
	float32_t3 pointLightDirection, float32_t pointFactor, float32_t3 toEye,
	float32_t3 spotLightDirectionOnSuface, float32_t spotFactor) {

	float32_t4 color;

	// 平行光源
	float directionalLightNdotL = dot(normalize(input.normal), -gDirectionalLight.direction);
	float directionalLightCos = pow(directionalLightNdotL * 0.5f + 0.5f, 2.0f);
	float32_t3 directionalLightReflectLight = reflect(gDirectionalLight.direction, normalize(input.normal));
	float directionalLightRdotE = dot(directionalLightReflectLight, toEye);
	float directionalLightSpecularPow = pow(saturate(directionalLightRdotE), gMaterial.shininess);
	// 拡散反射
	float32_t3 directionalLightDiffuse =
		gMaterial.color.rgb * textureColor.rgb * gDirectionalLight.color.rgb * directionalLightCos * gDirectionalLight.intencity;
	// 鏡面反射
	float32_t3 directionalLightSpecular =
		gDirectionalLight.color.rgb * gDirectionalLight.intencity * directionalLightSpecularPow * float32_t3(1.0f, 1.0f, 1.0f);

	// ポイントライト
	float pointLightNdotL = dot(normalize(input.normal), -pointLightDirection);
	float pointLightCos = pow(pointLightNdotL * 0.5f + 0.5f, 2.0f);
	float32_t3 pointLightReflectLight = reflect(pointLightDirection, normalize(input.normal));
	float pointLightRdotE = dot(pointLightReflectLight, toEye);
	float pointLightSpecularPow = pow(saturate(pointLightRdotE), gMaterial.shininess);
	// 拡散反射
	float32_t3 pointLightDiffuse =
		gMaterial.color.rgb * textureColor.rgb * gPointLight.color.rgb * pointLightCos * gPointLight.intencity * pointFactor;
	// 鏡面反射
	float32_t3 pointLightSpecular =
		gPointLight.color.rgb * gPointLight.intencity * pointFactor * pointLightSpecularPow * float32_t3(1.0f, 1.0f, 1.0f);

	// スポットライト
	float spotLightNdotL = dot(normalize(input.normal), -spotLightDirectionOnSuface);
	float spotLightCos = pow(spotLightNdotL * 0.5f + 0.5f, 2.0f);
	float32_t3 spotLightReflectLight = reflect(spotLightDirectionOnSuface, normalize(input.normal));
	float spotLightRdotE = dot(spotLightReflectLight, toEye);
	float spotLightSpecularPow = pow(saturate(spotLightRdotE), gMaterial.shininess);
	// 拡散反射
	float32_t3 spotLightDiffuse =
		gMaterial.color.rgb * textureColor.rgb * gSpotLight.color.rgb * spotLightCos * gSpotLight.intencity * spotFactor;
	//// 鏡面反射
	float32_t3 spotLightSpecular =
		gSpotLight.color.rgb * gSpotLight.intencity * spotFactor * spotLightSpecularPow * float32_t3(1.0f, 1.0f, 1.0f);

	// 全てのライトデータを入れる
	// 拡散反射+鏡面反射
	color.rgb = directionalLightDiffuse + directionalLightSpecular + pointLightDiffuse + pointLightSpecular + spotLightDiffuse + spotLightSpecular;
	// α
	color.a = gMaterial.color.a * textureColor.a;

	return color;

}

/// <summary>
/// ブリン鏡面反射
/// </summary>
float32_t4 BlinnPhongReflection(VertexShaderOutput input, float32_t4 textureColor, float32_t3 toEye,
	PointLightCalcData pointLightCalcDatas[256],
	float32_t3 spotLightDirectionOnSuface, float32_t spotFactor) {

	float32_t4 color;

	// 平行光源
	float directionalLightNdotL = dot(normalize(input.normal), -gDirectionalLight.direction);
	float directionalLightCos = pow(directionalLightNdotL * 0.5f + 0.5f, 2.0f);
	float32_t3 directionalLightReflectLight = reflect(gDirectionalLight.direction, normalize(input.normal));
	float32_t3 directionalLightHalfVector = normalize(-gDirectionalLight.direction + toEye);
	float directionalLightNDotH = dot(normalize(input.normal), directionalLightHalfVector);
	float directionalLightSpecularPow = pow(saturate(directionalLightNDotH), gMaterial.shininess);
	// 拡散反射
	float32_t3 directionalLightDiffuse =
		gMaterial.color.rgb * textureColor.rgb * gDirectionalLight.color.rgb * directionalLightCos * gDirectionalLight.intencity;
	// 鏡面反射
	float32_t3 directionalLightSpecular =
		gDirectionalLight.color.rgb * gDirectionalLight.intencity * directionalLightSpecularPow * float32_t3(1.0f, 1.0f, 1.0f);

	// ポイントライト
	//float pointLightNdotL = dot(normalize(input.normal), -pointLightDirection);
	//float pointLightCos = pow(pointLightNdotL * 0.5f + 0.5f, 2.0f);
	//float32_t3 pointLightHalfVector = normalize(-pointLightDirection + toEye);
	//float pointLightNDotH = dot(normalize(input.normal), pointLightHalfVector);
	//float pointLightSpecularPow = pow(saturate(pointLightNDotH), gMaterial.shininess);
	//// 拡散反射
	//float32_t3 pointLightDiffuse =
	//	gMaterial.color.rgb * textureColor.rgb * gPointLight.color.rgb * pointLightCos * gPointLight.intencity * pointFactor;
	//// 鏡面反射
	//float32_t3 pointLightSpecular =
	//	gPointLight.color.rgb * gPointLight.intencity * pointFactor * pointLightSpecularPow * float32_t3(1.0f, 1.0f, 1.0f);

	// 拡散反射
	float32_t3 allPointLightDiffuse = { 0.0,0.0,0.0 };
	// 鏡面反射
	float32_t3 allPointLightSpecular = { 0.0,0.0,0.0 };

	for (int i = 0; i < 256; i++) {
		if (pointLightCalcDatas[i].used) {
			float pointLightNdotL = dot(normalize(input.normal), -pointLightCalcDatas[i].pointLightDirection);
			float pointLightCos = pow(pointLightNdotL * 0.5f + 0.5f, 2.0f);
			float32_t3 pointLightHalfVector = normalize(-pointLightCalcDatas[i].pointLightDirection + toEye);
			float pointLightNDotH = dot(normalize(input.normal), pointLightHalfVector);
			float pointLightSpecularPow = pow(saturate(pointLightNDotH), gMaterial.shininess);
			// 拡散反射
			float32_t3 pointLightDiffuse =
				gMaterial.color.rgb * textureColor.rgb * gPointLights[i].color.rgb * pointLightCos * gPointLights[i].intencity * pointLightCalcDatas[i].pointFactor;
			allPointLightDiffuse += pointLightDiffuse;
			// 鏡面反射
			float32_t3 pointLightSpecular =
				gPointLights[i].color.rgb * gPointLights[i].intencity * pointLightCalcDatas[i].pointFactor * pointLightSpecularPow * float32_t3(1.0f, 1.0f, 1.0f);
			allPointLightSpecular += pointLightSpecular;
		}
	}

	// スポットライト
	float spotLightNdotL = dot(normalize(input.normal), -spotLightDirectionOnSuface);
	float spotLightCos = pow(spotLightNdotL * 0.5f + 0.5f, 2.0f);
	float32_t3 spotLightHalfVector = normalize(-spotLightDirectionOnSuface + toEye);
	float spotLightNDotH = dot(normalize(input.normal), spotLightHalfVector);
	float spotLightSpecularPow = pow(saturate(spotLightNDotH), gMaterial.shininess);
	// 拡散反射
	float32_t3 spotLightDiffuse =
		gMaterial.color.rgb * textureColor.rgb * gSpotLight.color.rgb * spotLightCos * gSpotLight.intencity * spotFactor;
	//// 鏡面反射
	float32_t3 spotLightSpecular =
		gSpotLight.color.rgb * gSpotLight.intencity * spotFactor * spotLightSpecularPow * float32_t3(1.0f, 1.0f, 1.0f);


	// 全てのライトデータを入れる
	// 拡散反射+鏡面反射
	color.rgb = directionalLightDiffuse + directionalLightSpecular + allPointLightDiffuse + allPointLightSpecular + spotLightDiffuse + spotLightSpecular;
	// α
	color.a = gMaterial.color.a * textureColor.a;

	return color;

}

PointLightCalcData CreatePointLightCalcData(VertexShaderOutput input, int index) {

	PointLightCalcData pointLightCalcData;
	pointLightCalcData.pointLightDirection = float32_t3(0.0f, 0.0f, 0.0f);
	pointLightCalcData.pointFactor = 0.0f;
	pointLightCalcData.used = true;

	if (!gPointLights[index].used) {
		pointLightCalcData.used = false;
		return pointLightCalcData;
	}

	// 逆二乗の法則
	float32_t pointDistance = length(gPointLights[index].position - input.worldPosition);
	if (pointDistance > gPointLights[index].radius) {
		pointLightCalcData.used = false;
		return pointLightCalcData;
	}
	pointLightCalcData.pointFactor = pow(saturate(-pointDistance / gPointLights[index].radius + 1.0), gPointLights[index].decay);

	pointLightCalcData.pointLightDirection = normalize(input.worldPosition - gPointLights[index].position);

	return pointLightCalcData;

}

PixelShaderOutput main(VertexShaderOutput input) {
	PixelShaderOutput output;
	float4 transformedUV = mul(float32_t4(input.texcoord, 0.0f, 1.0f), gMaterial.uvTransform);
	float32_t4 textureColor = gTexture.Sample(gSampler, transformedUV.xy);
	float32_t3 toEye = normalize(gCamera.worldPosition - input.worldPosition);

	// ポイントライト
	float32_t3 pointLightDirection = normalize(input.worldPosition - gPointLight.position);
	// 逆二乗の法則
	float32_t pointDistance = length(gPointLight.position - input.worldPosition);
	float32_t pointFactor = pow(saturate(-pointDistance / gPointLight.radius + 1.0), gPointLight.decay);

	PointLightCalcData PointLightCalcDatas[256];

	for (int i = 0; i < 256; i++) {
		PointLightCalcDatas[i] = CreatePointLightCalcData(input, i);
	}

	// スポットライト
	float32_t3 spotLightDirectionOnSuface = normalize(input.worldPosition - gSpotLight.position);
	//
	float32_t spotDistance = length(gSpotLight.position - input.worldPosition);
	float32_t spotFactor = pow(saturate(-spotDistance / gSpotLight.distance + 1.0), gSpotLight.decay);
	//
	float32_t cosAngle = dot(spotLightDirectionOnSuface, gSpotLight.direction);
	float32_t fallofFactor = 0;

	if (gSpotLight.cosFalloffStart == 0.0) {
		fallofFactor = saturate((cosAngle - gSpotLight.cosAngle) / (1.0 - gSpotLight.cosAngle));
	}
	else {
		fallofFactor = saturate((cosAngle - gSpotLight.cosAngle) / (gSpotLight.cosFalloffStart - gSpotLight.cosAngle));
	}

	spotFactor = spotFactor * fallofFactor;


	// ライティング無し
	if (gMaterial.enableLighting == 0) {
		output.color = gMaterial.color * textureColor;
	}
	// ランバート
	else if (gMaterial.enableLighting == 1) {
		output.color = Lambert(input, textureColor, pointLightDirection, pointFactor, spotLightDirectionOnSuface, spotFactor);
	}
	// ハーフランバート
	else if (gMaterial.enableLighting == 2) {
		output.color = HalfLambert(input, textureColor, pointLightDirection, pointFactor, spotLightDirectionOnSuface, spotFactor);
	}
	// 鏡面反射
	else if (gMaterial.enableLighting == 3) {
		output.color = PhongReflection(input, textureColor, pointLightDirection, pointFactor, toEye, spotLightDirectionOnSuface, spotFactor);
	}
	// ブリン鏡面反射
	else if (gMaterial.enableLighting == 4) {
		output.color = BlinnPhongReflection(input, textureColor, toEye, PointLightCalcDatas, spotLightDirectionOnSuface, spotFactor);
	}
	// その他の数が入ってきた場合
	else {
		output.color = gMaterial.color * textureColor;
	}

	// textureかoutput.colorのα値が0の時にPixelを棄却
	if (textureColor.a == 0.0 || output.color.a == 0.0) {
		discard;
	}

	return output;
}