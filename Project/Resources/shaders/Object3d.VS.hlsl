#include "Object3d.hlsli"

struct TransformationMatrix {
	float32_t4x4 WVP;
	float32_t4x4 World;
	float32_t4x4 WorldInverseTranspose;
	float32_t4x4 ScaleInverse;
};
ConstantBuffer<TransformationMatrix> gTransformationMatrix : register(b0);

StructuredBuffer<TransformationMatrix> gTransformationMatrixes : register(t0);

struct VertexShaderInput {
	float32_t4 position : POSITION0;
	float32_t2 texcoord : TEXCOORD0;
	float32_t3 normal : NORMAL0;
};

VertexShaderOutput main(VertexShaderInput input) {
	VertexShaderOutput output;
	output.position = mul(input.position, gTransformationMatrixes[0].WVP);
	output.texcoord = input.texcoord;
	output.normal = normalize(mul(input.normal, (float32_t3x3)gTransformationMatrixes[0].WorldInverseTranspose));
	output.worldPosition = mul(input.position, gTransformationMatrixes[0].World).xyz;
	return output;
}