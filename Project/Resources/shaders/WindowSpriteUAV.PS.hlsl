#include "WindowSprite.hlsli"

RWTexture2D<float32_t4> gTexture : register(u0);
SamplerState gSampler : register(s0);

struct PixelShaderOutput {
	float32_t4 color : SV_TARGET0;
};

PixelShaderOutput main(VertexShaderOutput input)
{

	PixelShaderOutput output;
	float32_t2 tex = input.texcoord;
	float32_t4 textureColor = gTexture[tex];
	output.color = textureColor;

	output.color.w = 1.0f;

	return output;

}