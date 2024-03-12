#include "Line.hlsli"

struct VertexShaderInput {
	float32_t4 position : POSITION0;
	float32_t4 color : COLOR0;
};

struct WVP {
	float32_t4x4 m;
};

ConstantBuffer<WVP> gWVP0 : register(b0);
ConstantBuffer<WVP> gWVP1 : register(b1);

VertexShaderOutput main(VertexShaderInput input, uint32_t vertexId : SV_VertexID)
{

	VertexShaderOutput output;

	WVP wvp[2];
	wvp[0] = gWVP0;
	wvp[1] = gWVP1;

	output.position = mul(input.position, wvp[vertexId].m);
	output.color = input.color;

	return output;

}