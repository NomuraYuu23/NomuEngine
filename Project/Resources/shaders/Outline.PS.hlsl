#include "Outline.hlsli"

struct PixelShaderOutput {
	float32_t4 color : SV_TARGET1;
};

PixelShaderOutput main(VertexShaderOutput input)
{

	PixelShaderOutput output;

	output.color = input.color;

	return output;

}