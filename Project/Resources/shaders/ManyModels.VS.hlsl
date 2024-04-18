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

StructuredBuffer<TransformationMatrix> gTransformationMatrixes : register(t1);

ConstantBuffer<ViewProjectionMatrix> gViewProjectionMatrix : register(b0);

struct VertexShaderInput {
	float32_t4 position : POSITION0;
	float32_t2 texcoord : TEXCOORD0;
	float32_t3 normal : NORMAL0;
	float32_t4 weight : WEIGHT0;
	int32_t4 index : INDEX0;
};

VertexShaderOutput main(VertexShaderInput input, uint32_t vertexId : SV_VertexID, uint32_t instanceId : SV_InstanceID) {

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
	output.position = mul(output.position, gTransformationMatrixes[instanceId].World);
	output.position = mul(output.position, gViewProjectionMatrix.Matrix);

	float32_t4x4 worldInverseTranspose = comb * gTransformationMatrixes[instanceId].WorldInverseTranspose;
	output.normal = normalize(mul(input.normal, (float32_t3x3)worldInverseTranspose));

	float32_t4 worldPosition = mul(input.position, comb);
	worldPosition.w = 1.0f;
	output.worldPosition = mul(worldPosition, gTransformationMatrixes[instanceId].World).xyz;

	return output;
}
