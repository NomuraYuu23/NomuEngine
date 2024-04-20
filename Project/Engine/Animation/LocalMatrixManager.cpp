#include "LocalMatrixManager.h"

#include "../base/BufferResource.h"
#include "../base/WinApp.h"
#include "../base/DirectXCommon.h"
#include "../base/SRVDescriptorHerpManager.h"

// コマンドリスト
ID3D12GraphicsCommandList* LocalMatrixManager::sCommandList = nullptr;

LocalMatrixManager::~LocalMatrixManager()
{

	SRVDescriptorHerpManager::DescriptorHeapsMakeNull(indexDescriptorHeap_);

}

void LocalMatrixManager::Initialize(const std::vector<NodeData>& nodeDatas)
{
	
	num_ = static_cast<uint32_t>(nodeDatas.size());

	localMatrixesBuff_ = BufferResource::CreateBufferResource(DirectXCommon::GetInstance()->GetDevice(), ((sizeof(LocalMatrix) + 0xff) & ~0xff) * num_);
	localMatrixesBuff_->Map(0, nullptr, reinterpret_cast<void**>(&localMatrixesMap_));

	for (uint32_t i = 0; i < num_; ++i) {
		localMatrixesMap_[i].matrix = Matrix4x4::Multiply(nodeDatas[i].offsetMatrix, nodeDatas[i].matrix);
		localMatrixesMap_[i].matrixInverseTranspose = Matrix4x4::Transpose(Matrix4x4::Inverse(localMatrixesMap_[i].matrix));
	}

	D3D12_SHADER_RESOURCE_VIEW_DESC localMatrixesSrvDesc{};
	localMatrixesSrvDesc.Format = DXGI_FORMAT_UNKNOWN;
	localMatrixesSrvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	localMatrixesSrvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
	localMatrixesSrvDesc.Buffer.FirstElement = 0;
	localMatrixesSrvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
	localMatrixesSrvDesc.Buffer.NumElements = static_cast<UINT>(num_);
	localMatrixesSrvDesc.Buffer.StructureByteStride = sizeof(LocalMatrix);
	localMatrixesHandleCPU_ = SRVDescriptorHerpManager::GetCPUDescriptorHandle();
	localMatrixesHandleGPU_ = SRVDescriptorHerpManager::GetGPUDescriptorHandle();
	indexDescriptorHeap_ = SRVDescriptorHerpManager::GetNextIndexDescriptorHeap();
	SRVDescriptorHerpManager::NextIndexDescriptorHeapChange();
	DirectXCommon::GetInstance()->GetDevice()->CreateShaderResourceView(localMatrixesBuff_.Get(), &localMatrixesSrvDesc, localMatrixesHandleCPU_);

}

void LocalMatrixManager::Map(const std::vector<NodeData>& nodeDatas)
{

	for (uint32_t i = 0; i < nodeDatas.size(); ++i) {
		localMatrixesMap_[i].matrix = Matrix4x4::Multiply(nodeDatas[i].offsetMatrix,nodeDatas[i].matrix);
		localMatrixesMap_[i].matrixInverseTranspose = Matrix4x4::Transpose(Matrix4x4::Inverse(localMatrixesMap_[i].matrix));
	}

}

void LocalMatrixManager::SetGraphicsRootDescriptorTable(ID3D12GraphicsCommandList* cmdList, uint32_t rootParameterIndex)
{

	assert(sCommandList == nullptr);

	sCommandList = cmdList;

	sCommandList->SetGraphicsRootDescriptorTable(rootParameterIndex, localMatrixesHandleGPU_);

	// コマンドリストを解除
	sCommandList = nullptr;

}
