#include "Mesh.h"
#include "../base/BufferResource.h"

/// <summary>
/// メッシュデータ生成
/// </summary>
void Mesh::CreateMesh(
	ID3D12Device* sDevice, 
	const std::vector<VertexData>& vertices,
	const std::vector<VertexInfluence>& vertexInfluences) {

	// バッファ
	vertBuff_ = BufferResource::CreateBufferResource(sDevice, ((sizeof(VertexData) + 0xff) & ~0xff) * vertices.size());

	//リソースの先頭のアドレスから使う
	vbView_.BufferLocation = vertBuff_->GetGPUVirtualAddress();
	//使用するリソースのサイズは頂点3つ分のサイズ
	vbView_.SizeInBytes = UINT(sizeof(VertexData) * vertices.size());
	//1頂点あたりのサイズ
	vbView_.StrideInBytes = sizeof(VertexData);

	//書き込むためのアドレスを取得
	vertBuff_->Map(0, nullptr, reinterpret_cast<void**>(&vertMap));
	//頂点データをリソースにコピー
	std::memcpy(vertMap, vertices.data(), sizeof(VertexData) * vertices.size());



	// インフルエンスバッファ
	influenceBuff_ = BufferResource::CreateBufferResource(sDevice, ((sizeof(VertexInfluence) + 0xff) & ~0xff) * vertexInfluences.size());

	//リソースの先頭のアドレスから使う
	influenceView_.BufferLocation = influenceBuff_->GetGPUVirtualAddress();
	//使用するリソースのサイズは頂点3つ分のサイズ
	influenceView_.SizeInBytes = UINT(sizeof(VertexInfluence) * vertexInfluences.size());
	//1頂点あたりのサイズ
	influenceView_.StrideInBytes = sizeof(VertexInfluence);

	//書き込むためのアドレスを取得
	influenceBuff_->Map(0, nullptr, reinterpret_cast<void**>(&influenceMap));
	//頂点データをリソースにコピー
	std::memcpy(influenceMap, vertexInfluences.data(), sizeof(VertexInfluence) * vertexInfluences.size());

}
