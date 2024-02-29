#include "Object3d.hlsli"

struct TransformationMatrix {
	float32_t4x4 WVP;
	float32_t4x4 World;
	float32_t4x4 WorldInverseTranspose;
	float32_t4x4 ScaleInverse;
};
ConstantBuffer<TransformationMatrix> gTransformationMatrix : register(b0);

StructuredBuffer<TransformationMatrix> gTransformationMatrixes : register(t0);

struct MeshNumData {
	uint32_t incrementMeshNum[32];
	uint32_t incrementMeshNumMax;
};
ConstantBuffer<MeshNumData> gMeshNumData : register(b1);

struct VertexShaderInput {
	float32_t4 position : POSITION0;
	float32_t2 texcoord : TEXCOORD0;
	float32_t3 normal : NORMAL0;
};

VertexShaderOutput main(VertexShaderInput input, uint32_t vertexId : SV_VertexID) {
	
	uint32_t meshNum = 0;

	for (int i = 0; i < 32; ++i) {
		
		if (vertexId >= gMeshNumData.incrementMeshNum[i]) {
			meshNum++;
		}
		else {
			break;
		}

		if (i == gMeshNumData.incrementMeshNumMax) {
			break;
		}
	}

	VertexShaderOutput output;
	output.position = mul(input.position, gTransformationMatrixes[meshNum].WVP);
	output.texcoord = input.texcoord;
	output.normal = normalize(mul(input.normal, (float32_t3x3)gTransformationMatrixes[meshNum].WorldInverseTranspose));
	output.worldPosition = mul(input.position, gTransformationMatrixes[meshNum].World).xyz;
	return output;
}