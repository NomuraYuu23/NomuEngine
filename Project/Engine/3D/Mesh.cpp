#include "Mesh.h"
#include "../base/BufferResource.h"
#include "../base/SRVDescriptorHerpManager.h"

Mesh::~Mesh()
{

	SRVDescriptorHerpManager::DescriptorHeapsMakeNull(vertIndexDescriptorHeap_);
	SRVDescriptorHerpManager::DescriptorHeapsMakeNull(influenceIndexDescriptorHeap_);
	SRVDescriptorHerpManager::DescriptorHeapsMakeNull(vertUAVIndexDescriptorHeap_);

}

/// <summary>
/// メッシュデータ生成
/// </summary>
void Mesh::CreateMesh(
	ID3D12Device* sDevice, 
	const std::vector<VertexData>& vertices,
	const std::vector<VertexInfluence>& vertexInfluences) {

	// 頂点バッファ
	VertBuffInitialize(sDevice, vertices);

	// インフルエンスバッファ
	VertInfluenceBuffInitialize(sDevice, vertexInfluences);

	// UAVバッファ
	UAVBuffInitialize(sDevice, vertices);

	// SkinningInformationバッファ
	skinningInformationBuff_ = BufferResource::CreateBufferResource(sDevice, ((sizeof(SkinningInformation) + 0xff) & ~0xff));
	//書き込むためのアドレスを取得
	skinningInformationBuff_->Map(0, nullptr, reinterpret_cast<void**>(&skinningInformationMap_));
	// マップ
	skinningInformationMap_->num = static_cast<int32_t>(vertices.size());
	skinningInformationMap_->isInverse = false;

}

void Mesh::SetComputeRootDescriptorTableVertHandleGPU(ID3D12GraphicsCommandList* commandList, uint32_t rootParameterIndex)
{
	commandList->SetComputeRootDescriptorTable(rootParameterIndex, vertHandleGPU_);
}

void Mesh::SetComputeRootDescriptorTableInfluenceHandleGPU(ID3D12GraphicsCommandList* commandList, uint32_t rootParameterIndex)
{
	commandList->SetComputeRootDescriptorTable(rootParameterIndex, influenceHandleGPU_);
}

void Mesh::SetComputeRootDescriptorTableVertUAVHandleGPU(ID3D12GraphicsCommandList* commandList, uint32_t rootParameterIndex)
{
	commandList->SetComputeRootDescriptorTable(rootParameterIndex, vertUAVHandleGPU_);
}

void Mesh::SetGraphicsRootDescriptorTableVertUAVHandleGPU(ID3D12GraphicsCommandList* commandList, uint32_t rootParameterIndex)
{
	commandList->SetGraphicsRootDescriptorTable(rootParameterIndex, vertUAVHandleGPU_);
}

void Mesh::VertBuffInitialize(ID3D12Device* sDevice, const std::vector<VertexData>& vertices)
{

	// 頂点バッファ
	vertBuff_ = BufferResource::CreateBufferResource(sDevice, ((sizeof(VertexData) + 0xff) & ~0xff) * vertices.size());

	//リソースの先頭のアドレスから使う
	vbView_.BufferLocation = vertBuff_->GetGPUVirtualAddress();
	//使用するリソースのサイズは頂点3つ分のサイズ
	vbView_.SizeInBytes = UINT(sizeof(VertexData) * vertices.size());
	//1頂点あたりのサイズ
	vbView_.StrideInBytes = sizeof(VertexData);

	//書き込むためのアドレスを取得
	vertBuff_->Map(0, nullptr, reinterpret_cast<void**>(&vertMap_));
	//頂点データをリソースにコピー
	std::memcpy(vertMap_, vertices.data(), sizeof(VertexData) * vertices.size());

	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Format = DXGI_FORMAT_UNKNOWN;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
	srvDesc.Buffer.FirstElement = 0;
	srvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
	srvDesc.Buffer.NumElements = static_cast<UINT>(vertices.size());
	srvDesc.Buffer.StructureByteStride = sizeof(VertexData);
	vertHandleCPU_ = SRVDescriptorHerpManager::GetCPUDescriptorHandle();
	vertHandleGPU_ = SRVDescriptorHerpManager::GetGPUDescriptorHandle();
	vertIndexDescriptorHeap_ = SRVDescriptorHerpManager::GetNextIndexDescriptorHeap();
	SRVDescriptorHerpManager::NextIndexDescriptorHeapChange();
	DirectXCommon::GetInstance()->GetDevice()->CreateShaderResourceView(vertBuff_.Get(), &srvDesc, vertHandleCPU_);

}

void Mesh::VertInfluenceBuffInitialize(ID3D12Device* sDevice, const std::vector<VertexInfluence>& vertexInfluences)
{

	// インフルエンスバッファ
	influenceBuff_ = BufferResource::CreateBufferResource(sDevice, ((sizeof(VertexInfluence) + 0xff) & ~0xff) * vertexInfluences.size());

	//書き込むためのアドレスを取得
	influenceBuff_->Map(0, nullptr, reinterpret_cast<void**>(&influenceMap_));
	//頂点データをリソースにコピー
	std::memcpy(influenceMap_, vertexInfluences.data(), sizeof(VertexInfluence) * vertexInfluences.size());

	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Format = DXGI_FORMAT_UNKNOWN;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
	srvDesc.Buffer.FirstElement = 0;
	srvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
	srvDesc.Buffer.NumElements = static_cast<UINT>(vertexInfluences.size());
	srvDesc.Buffer.StructureByteStride = sizeof(VertexInfluence);
	influenceHandleCPU_ = SRVDescriptorHerpManager::GetCPUDescriptorHandle();
	influenceHandleGPU_ = SRVDescriptorHerpManager::GetGPUDescriptorHandle();
	influenceIndexDescriptorHeap_ = SRVDescriptorHerpManager::GetNextIndexDescriptorHeap();
	SRVDescriptorHerpManager::NextIndexDescriptorHeapChange();
	DirectXCommon::GetInstance()->GetDevice()->CreateShaderResourceView(influenceBuff_.Get(), &srvDesc, influenceHandleCPU_);

}

void Mesh::UAVBuffInitialize(
	ID3D12Device* sDevice,
	const std::vector<VertexData>& vertices)
{

	// UAVデータ
	vertBuffUAV_ = BufferResource::CreateBufferResourceUAV(sDevice, ((sizeof(VertexData) + 0xff) & ~0xff) * vertices.size());

	D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc{};

	uavDesc.Format = DXGI_FORMAT_UNKNOWN;
	uavDesc.ViewDimension = D3D12_UAV_DIMENSION_BUFFER;
	uavDesc.Buffer.FirstElement = 0;
	uavDesc.Buffer.NumElements = static_cast<UINT>(vertices.size());
	uavDesc.Buffer.CounterOffsetInBytes = 0;
	uavDesc.Buffer.Flags = D3D12_BUFFER_UAV_FLAG_NONE;
	uavDesc.Buffer.StructureByteStride = sizeof(VertexData);

	vertUAVHandleCPU_ = SRVDescriptorHerpManager::GetCPUDescriptorHandle();
	vertUAVHandleGPU_ = SRVDescriptorHerpManager::GetGPUDescriptorHandle();
	vertUAVIndexDescriptorHeap_ = SRVDescriptorHerpManager::GetNextIndexDescriptorHeap();
	SRVDescriptorHerpManager::NextIndexDescriptorHeapChange();

	sDevice->CreateUnorderedAccessView(vertBuffUAV_.Get(), nullptr, &uavDesc, vertUAVHandleCPU_);

}
