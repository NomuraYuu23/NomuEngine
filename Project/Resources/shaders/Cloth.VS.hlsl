#include "Cloth.hlsli"

struct ViewProjectionMatrix {
	float32_t4x4 Matrix;
};

ConstantBuffer<ViewProjectionMatrix> gViewProjectionMatrix : register(b0);

struct VertexShaderInput {
	float32_t4 position : POSITION0;
	float32_t2 texcoord : TEXCOORD0;
	float32_t3 normal : NORMAL0;
};

VertexShaderOutput main(VertexShaderInput input)
{

	VertexShaderOutput output;

	output.position = mul(input.position, gViewProjectionMatrix.Matrix);
	output.texcoord = input.texcoord;
	output.normal = input.normal;
	output.worldPosition = input.position.xyz;

	return output;

}