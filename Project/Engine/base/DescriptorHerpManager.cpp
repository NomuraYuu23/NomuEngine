#include "DescriptorHerpManager.h"

// ディスクリプタヒープ
Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> DescriptorHerpManager::descriptorHeap_ = nullptr;
// デバイス
ID3D12Device* DescriptorHerpManager::device_ = nullptr;
// 次のディスクリプタヒープの場所
uint32_t DescriptorHerpManager::nextIndexDescriptorHeap_ = 0u;
// ディスクリプタヒープのどこが空いているか
std::array<bool, DescriptorHerpManager::kNumDescriptors> DescriptorHerpManager::isNullDescriptorHeaps_;

DescriptorHerpManager* DescriptorHerpManager::GetInstance()
{
	static DescriptorHerpManager instance;
	return &instance;
}

void DescriptorHerpManager::Initialize(DirectXCommon* dxCommon)
{

	HRESULT result = S_FALSE;

	device_ = dxCommon->GetDevice();

	// ディスクリプタヒープを生成
	D3D12_DESCRIPTOR_HEAP_DESC descHeapDesc = {};
	descHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	descHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	descHeapDesc.NumDescriptors = kNumDescriptors;
	result = device_->CreateDescriptorHeap(&descHeapDesc, IID_PPV_ARGS(&descriptorHeap_));
	assert(SUCCEEDED(result));

	for (size_t i = 0; i < isNullDescriptorHeaps_.size(); ++i) {
		isNullDescriptorHeaps_[i] = true;
	}

}

D3D12_CPU_DESCRIPTOR_HANDLE DescriptorHerpManager::GetCPUDescriptorHandle()
{

	uint32_t descriptorSize = device_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	D3D12_CPU_DESCRIPTOR_HANDLE handleCPU = descriptorHeap_->GetCPUDescriptorHandleForHeapStart();
	handleCPU.ptr += (descriptorSize * nextIndexDescriptorHeap_);
	return handleCPU;

}

D3D12_GPU_DESCRIPTOR_HANDLE DescriptorHerpManager::GetGPUDescriptorHandle()
{

	uint32_t descriptorSize = device_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	D3D12_GPU_DESCRIPTOR_HANDLE handleGPU = descriptorHeap_->GetGPUDescriptorHandleForHeapStart();
	handleGPU.ptr += (descriptorSize * nextIndexDescriptorHeap_);
	return handleGPU;

}

void DescriptorHerpManager::NextIndexDescriptorHeapChange()
{

	isNullDescriptorHeaps_[nextIndexDescriptorHeap_] = false;
	
	for (size_t i = 0; i < isNullDescriptorHeaps_.size(); ++i) {
		if (isNullDescriptorHeaps_[i]) {
			nextIndexDescriptorHeap_ = static_cast<uint32_t>(i);
			break;
		}
	}

}

void DescriptorHerpManager::DescriptorHeapsMakeNull(uint32_t index)
{

	isNullDescriptorHeaps_[index] = true;

}
