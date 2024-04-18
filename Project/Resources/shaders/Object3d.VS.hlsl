#include "Object3d.hlsli"

struct LocalMatrix {
	float32_t4x4 Matrix;
};

struct TransformationMatrix {
	float32_t4x4 World;
	float32_t4x4 WorldInverseTranspose;
};

struct ViewProjectionMatrix {
	float32_t4x4 Matrix;
};

StructuredBuffer<LocalMatrix> gLocalMatrixes : register(t0);

ConstantBuffer<TransformationMatrix> gTransformationMatrix : register(b0);

ConstantBuffer<ViewProjectionMatrix> gViewProjectionMatrix : register(b1);

struct VertexShaderInput {
	float32_t4 position : POSITION0;
	float32_t2 texcoord : TEXCOORD0;
	float32_t3 normal : NORMAL0;
	float32_t4 weight : WEIGHT0;
	int32_t4 index : INDEX0;
};

VertexShaderOutput main(VertexShaderInput input) {

	VertexShaderOutput output;
	// texcoord
	output.texcoord = input.texcoord;

	// comb
	float32_t w[4] = { input.weight.x, input.weight.y, input.weight.z, input.weight.w };
	uint32_t id[4] = { input.index.x, input.index.y, input.index.z, input.index.w };
	float32_t4x4 comb = (float32_t4x4)0;

	for (int i = 0; i < 4; ++i) {
		comb += gLocalMatrixes[id[i]].Matrix * w[i];
	}

	input.position.w = 1.0f;

	output.position = mul(input.position, comb);
	//output.position = input.position;
	output.position = mul(output.position, gTransformationMatrix.World);
	output.position = mul(output.position, gViewProjectionMatrix.Matrix);

	float32_t4x4 worldInverseTranspose = comb * gTransformationMatrix.WorldInverseTranspose;
	output.normal = normalize(mul(input.normal, (float32_t3x3)worldInverseTranspose));
	
	float32_t4 worldPosition = mul(input.position, comb);
	worldPosition.w = 1.0f;
	output.worldPosition = mul(worldPosition, gTransformationMatrix.World).xyz;
	
	return output;
}