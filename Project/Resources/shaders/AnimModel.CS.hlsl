struct Vertex {
	float32_t4 position;
	float32_t2 texcoord;
	float32_t3 normal;
};

struct VertexInfluence {
	float32_t4 weight;
	int32_t4 index;
};

struct LocalMatrix{
	float32_t4x4 Matrix;
	float32_t4x4 MatrixInverseTranspose;
};

struct VertexNumber {
	uint32_t num;
};

// 頂点数
ConstantBuffer<VertexNumber> gVertexNumber : register(b0);

// 頂点情報
StructuredBuffer<Vertex> gVertices : register(t0);

// 重み情報
StructuredBuffer<VertexInfluence> gVertexInfluences : register(t1);

// ローカル行列
StructuredBuffer<LocalMatrix> gLocalMatrixes : register(t2);

// 書き込まれるデータ
RWStructuredBuffer<Vertex> gOutputVertices : register(u0);

[numthreads(1024, 1, 1)]
void main(uint32_t3 dispatchId : SV_DispatchThreadID)
{

	uint32_t VertexIndex = dispatchId.x;

	if (VertexIndex < gVertexNumber.num) {
		Vertex output;

		Vertex input = gVertices[VertexIndex];
		VertexInfluence inputVI = gVertexInfluences[VertexIndex];
		// texcoord
		output.texcoord = input.texcoord;

		// comb
		float32_t w[4] = { inputVI.weight.x, inputVI.weight.y, inputVI.weight.z, inputVI.weight.w };
		uint32_t id[4] = { inputVI.index.x, inputVI.index.y, inputVI.index.z, inputVI.index.w };
		float32_t4x4 comb = (float32_t4x4)0;
		float32_t4x4 combInverseTranspose = (float32_t4x4)0;

		for (uint32_t i = 0; i < 4; ++i) {
			comb += gLocalMatrixes[id[i]].Matrix * w[i];
			combInverseTranspose += gLocalMatrixes[id[i]].MatrixInverseTranspose * w[i];
		}

		input.position.w = 1.0f;

		output.position = mul(input.position, comb);

		output.normal = normalize(mul(input.normal, (float32_t3x3)combInverseTranspose));

		gOutputVertices[VertexIndex] = output;
	}

}
