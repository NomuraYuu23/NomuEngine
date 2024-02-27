#include "Mesh.h"
#include "../base/BufferResource.h"

/// <summary>
/// メッシュデータ生成
/// </summary>
void Mesh::CreateMesh(ID3D12Device* sDevice, const std::vector<VertexData>& vertices) {


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

}
