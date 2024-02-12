#include "OutLine3D.hlsli"

struct TransformationMatrix {
	float32_t4x4 WVP;
	float32_t4x4 World;
	float32_t4x4 WorldInverseTranspose;
	float32_t4x4 ScaleInverse;
};
ConstantBuffer<TransformationMatrix> gTransformationMatrix : register(b0);

struct OutLineData {
	float32_t4x4 Scale;
};
ConstantBuffer<OutLineData> gOutLineData : register(b1);


struct VertexShaderInput {
	float32_t4 position : POSITION0;
	float32_t2 texcoord : TEXCOORD0;
	float32_t3 normal : NORMAL0;
};

VertexShaderOutput main(VertexShaderInput input) {
	VertexShaderOutput output;
	//float32_t4x4 wvp = mul(gOutLineData.Scale,gTransformationMatrix.WVP);
	float32_t4x4 wvp = gTransformationMatrix.WVP;
	float32_t4 interporationPosition;
	//interporationPosition = input.position - input.center;
	interporationPosition.w = 1.0f;
	//interporationPosition = mul(interporationPosition, gOutLineData.Scale);
	float32_t3 normal = 1;
	normal = normalize(mul(input.normal, (float32_t3x3)gTransformationMatrix.ScaleInverse));
	float32_t4 ln = 1;
	ln.xyz = normal;
	interporationPosition = mul(ln, gOutLineData.Scale);
	//interporationPosition.x *= 0.5f;
	//interporationPosition.y *= 0.5f;
	//interporationPosition.z *= 0.5f;
	//interporationPosition += input.center;
	interporationPosition += input.position;
	interporationPosition.w = 1.0f;
	output.position = mul(interporationPosition, wvp);
	
	return output;
}