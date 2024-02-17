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
	float32_t4 color;
	float32_t3 position;
	float intencity;
};

ConstantBuffer<Material> gMaterial : register(b0);

ConstantBuffer<DirectionalLight> gDirectionalLight : register(b1);

ConstantBuffer<Camera> gCamera : register(b2);

ConstantBuffer<PointLight> gPointLight : register(b3);

struct PixelShaderOutput {
	float32_t4 color : SV_TARGET0;
};

PixelShaderOutput main(VertexShaderOutput input) {
	PixelShaderOutput output;
	float4 transformedUV = mul(float32_t4(input.texcoord, 0.0f, 1.0f), gMaterial.uvTransform);
	float32_t4 textureColor = gTexture.Sample(gSampler, transformedUV.xy);
	float32_t3 toEye = normalize(gCamera.worldPosition - input.worldPosition);

	// ポイントライト
	float32_t3 pointLightDirection = normalize(input.worldPosition - gPointLight.position);

	// ライティング無し
	if (gMaterial.enableLighting == 0) {
		output.color = gMaterial.color * textureColor;
	}
	// ランバート
	else if (gMaterial.enableLighting == 1) {
		// 平行光源
		float directionalLightCos = saturate(dot(normalize(input.normal), -gDirectionalLight.direction));
		float32_t3 directionalLightColor = gDirectionalLight.color.rgb * directionalLightCos * gDirectionalLight.intencity;

		// ポイントライト
		float pointLightCos = saturate(dot(normalize(input.normal), -pointLightDirection));
		float32_t3 pointLightColor = gPointLight.color.rgb * pointLightCos * gPointLight.intencity;

		// 全てのライトデータを入れる
		output.color.rgb = gMaterial.color.rgb * textureColor.rgb * directionalLightColor * pointLightColor;
		output.color.a = gMaterial.color.a * textureColor.a;
	}
	// ハーフランバート
	else if (gMaterial.enableLighting == 2) {
		// 平行光源
		float directionalLightNdotL = dot(normalize(input.normal), -gDirectionalLight.direction);
		float directionalLightCos = pow(directionalLightNdotL * 0.5f + 0.5f, 2.0f);
		float32_t3 directionalLightColor = gDirectionalLight.color.rgb * directionalLightCos * gDirectionalLight.intencity;

		// ポイントライト
		float pointLightNdotL = dot(normalize(input.normal), -pointLightDirection);
		float pointLightCos = pow(pointLightNdotL * 0.5f + 0.5f, 2.0f);
		float32_t3 pointLightColor = gPointLight.color.rgb * pointLightCos * gPointLight.intencity;

		// 全てのライトデータを入れる
		output.color.rgb = gMaterial.color.rgb * textureColor.rgb * directionalLightColor * pointLightColor;
		output.color.a = gMaterial.color.a * textureColor.a;
	}
	// 鏡面反射
	else if (gMaterial.enableLighting == 3) {
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
			gMaterial.color.rgb * textureColor.rgb * gPointLight.color.rgb * pointLightCos * gPointLight.intencity;
		// 鏡面反射
		float32_t3 pointLightSpecular =
			gPointLight.color.rgb * gPointLight.intencity * pointLightSpecularPow * float32_t3(1.0f, 1.0f, 1.0f);

		// 全てのライトデータを入れる
		// 拡散反射+鏡面反射
		output.color.rgb = directionalLightDiffuse + directionalLightSpecular + pointLightDiffuse + pointLightSpecular;
		// α
		output.color.a = gMaterial.color.a * textureColor.a;
	}
	// ブリン鏡面反射
	else if (gMaterial.enableLighting == 4) {

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
		float pointLightNdotL = dot(normalize(input.normal), -pointLightDirection);
		float pointLightCos = pow(pointLightNdotL * 0.5f + 0.5f, 2.0f);
		float32_t3 pointLightReflectLight = reflect(pointLightDirection, normalize(input.normal));
		float32_t3 pointLightHalfVector = normalize(-pointLightDirection + toEye);
		float pointLightNDotH = dot(normalize(input.normal), pointLightHalfVector);
		float pointLightSpecularPow = pow(saturate(pointLightNDotH), gMaterial.shininess);
		// 拡散反射
		float32_t3 pointLightDiffuse =
			gMaterial.color.rgb * textureColor.rgb * gPointLight.color.rgb * pointLightCos * gPointLight.intencity;
		// 鏡面反射
		float32_t3 pointLightSpecular =
			gPointLight.color.rgb * gPointLight.intencity * pointLightSpecularPow * float32_t3(1.0f, 1.0f, 1.0f);

		// 全てのライトデータを入れる
		// 拡散反射+鏡面反射
		output.color.rgb = directionalLightDiffuse + directionalLightSpecular + pointLightDiffuse + pointLightSpecular;
		// α
		output.color.a = gMaterial.color.a * textureColor.a;

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
