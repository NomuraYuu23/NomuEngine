#include "Line.hlsli"

struct VertexShaderInput {
	float32_t4 position : POSITION0;
	float32_t4 color : COLOR0;
};

struct VP {
	float32_t4x4 m;
};

ConstantBuffer<VP> gVP : register(b0);


VertexShaderOutput main(VertexShaderInput input)
{

	VertexShaderOutput output;

	output.position = mul(input.position, gVP.m);
	output.color = input.color;

	return output;

}