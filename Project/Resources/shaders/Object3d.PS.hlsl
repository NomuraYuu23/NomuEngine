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

ConstantBuffer<Material> gMaterial : register(b0);

ConstantBuffer<DirectionalLight> gDirectionalLight : register(b1);

ConstantBuffer<Camera> gCamera : register(b2);

struct PixelShaderOutput {
	float32_t4 color : SV_TARGET0;
};

PixelShaderOutput main(VertexShaderOutput input) {
	PixelShaderOutput output;
	float4 transformedUV = mul(float32_t4(input.texcoord, 0.0f, 1.0f), gMaterial.uvTransform);
	float32_t4 textureColor = gTexture.Sample(gSampler, transformedUV.xy);
	float32_t3 toEye = normalize(gCamera.worldPosition - input.worldPosition);
	
	// ライティング無し
	if (gMaterial.enableLighting == 0) {
		output.color = gMaterial.color * textureColor;
	}
	// ランバート
	else if (gMaterial.enableLighting == 1) {
		float cos = saturate(dot(normalize(input.normal), -gDirectionalLight.direction));
		output.color.rgb = gMaterial.color.rgb * textureColor.rgb * gDirectionalLight.color.rgb * cos * gDirectionalLight.intencity;
		output.color.a = gMaterial.color.a * textureColor.a;
	}
	// ハーフランバート
	else if (gMaterial.enableLighting == 2) {
		float NdotL = dot(normalize(input.normal), -gDirectionalLight.direction);
		float cos = pow(NdotL * 0.5f + 0.5f, 2.0f);
		output.color.rgb = gMaterial.color.rgb * textureColor.rgb * gDirectionalLight.color.rgb * cos * gDirectionalLight.intencity;
		output.color.a = gMaterial.color.a * textureColor.a;
	}
	// 鏡面反射
	else if (gMaterial.enableLighting == 3) {
		float NdotL = dot(normalize(input.normal), -gDirectionalLight.direction);
		float cos = pow(NdotL * 0.5f + 0.5f, 2.0f);
		float32_t3 reflectLight = reflect(gDirectionalLight.direction, normalize(input.normal));
		float RdotE = dot(reflectLight, toEye);
		float specularPow = pow(saturate(RdotE), gMaterial.shininess);
		// 拡散反射
		float32_t3 diffuse =
			gMaterial.color.rgb * textureColor.rgb * gDirectionalLight.color.rgb * cos * gDirectionalLight.intencity;
		// 鏡面反射
		float32_t3 specular =
			gDirectionalLight.color.rgb * gDirectionalLight.intencity * specularPow * float32_t3(1.0f, 1.0f, 1.0f);
		// 拡散反射+鏡面反射
		output.color.rgb = diffuse + specular;
		// α
		output.color.a = gMaterial.color.a * textureColor.a;
	}
	// ブリン鏡面反射
	else if (gMaterial.enableLighting == 4) {
		float NdotL = dot(normalize(input.normal), -gDirectionalLight.direction);
		float cos = pow(NdotL * 0.5f + 0.5f, 2.0f);
		float32_t3 reflectLight = reflect(gDirectionalLight.direction, normalize(input.normal));
		float RdotE = dot(reflectLight, toEye);
		float32_t3 halfVector = normalize(-gDirectionalLight.direction + toEye);
		float NDotH = dot(normalize(input.normal), halfVector);
		float specularPow = pow(saturate(NDotH), gMaterial.shininess);
		// 拡散反射
		float32_t3 diffuse =
			gMaterial.color.rgb * textureColor.rgb * gDirectionalLight.color.rgb * cos * gDirectionalLight.intencity;
		// 鏡面反射
		float32_t3 specular =
			gDirectionalLight.color.rgb * gDirectionalLight.intencity * specularPow * float32_t3(1.0f, 1.0f, 1.0f);
		// 拡散反射+鏡面反射
		output.color.rgb = diffuse + specular;
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
