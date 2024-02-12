#include "OutLine3D.hlsli"
SamplerState gSampler : register(s0);

struct PixelShaderOutput {
	float32_t4 color : SV_TARGET0;
};

struct OutLineData2 {
	float32_t4 Color;
};
ConstantBuffer<OutLineData2> gOutLineData : register(b0);

PixelShaderOutput main(VertexShaderOutput input) {
	PixelShaderOutput output;

	output.color = gOutLineData.Color;
	if (output.color.a == 0.0)
	{
		discard;
	}
	return output;
}