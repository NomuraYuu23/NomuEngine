#include "Outline.hlsli"

struct TransformationMatrix {
	float32_t4x4 WVP;
	float32_t4x4 World;
	float32_t4x4 WorldInverseTranspose;
};

ConstantBuffer<TransformationMatrix> gTransformationMatrix : register(b0);

struct OutlineParameter {

	float32_t4 color;
	float32_t width;

};

ConstantBuffer<OutlineParameter> gOutlineParameter : register(b1);

struct VertexShaderInput {
	float32_t4 position : POSITION0;
	float32_t2 texcoord : TEXCOORD0;
	float32_t3 normal : NORMAL0;
};

VertexShaderOutput main(VertexShaderInput input)
{

	VertexShaderOutput output;

	output.position.xyz = input.position.xyz + (input.normal * gOutlineParameter.width);

	output.position.w = 1.0f;

	output.position = mul(output.position, gTransformationMatrix.WVP);

	output.color = gOutlineParameter.color;

	return output;

}