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
	float32_t blend0 : BLENDWEIGHT0;
	float32_t blend1 : BLENDWEIGHT1;
	float32_t blend2 : BLENDWEIGHT2;
	uint32_t idx0 : BLENDINDICES0;
	uint32_t idx1 : BLENDINDICES1;
	uint32_t idx2 : BLENDINDICES2;
	uint32_t idx3 : BLENDINDICES3;
};

VertexShaderOutput main(VertexShaderInput input, uint32_t vertexId : SV_VertexID) {

	VertexShaderOutput output;
	// texcoord
	output.texcoord = input.texcoord;

	// comb
	float32_t w[3] = { input.blend0, input.blend1, input.blend2 };
	uint32_t id[4] = { input.idx0, input.idx1, input.idx2, input.idx3 };
	float32_t4x4 comb = (float32_t4x4)0;

	for (int i = 0; i < 3; ++i) {
		comb += gLocalMatrixes[id[i]].Matrix * w[i];
	}
	comb += gLocalMatrixes[id[3]].Matrix * (1.0f - w[0] - w[1] - w[2]);

	input.position.w = 1.0f;

	output.position = mul(input.position, comb);
	output.position = mul(output.position, gTransformationMatrix.World);
	output.position = mul(output.position, gViewProjectionMatrix.Matrix);

	float32_t4x4 worldInverseTranspose = comb * gTransformationMatrix.WorldInverseTranspose;
	output.normal = normalize(mul(input.normal, (float32_t3x3)worldInverseTranspose));
	
	float32_t4 worldPosition = mul(input.position, comb);
	worldPosition.w = 1.0f;
	output.worldPosition = mul(worldPosition, gTransformationMatrix.World).xyz;
	
	return output;
}