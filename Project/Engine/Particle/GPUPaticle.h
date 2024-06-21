#pragma once
class GPUPaticle
{

public:

	void Initialzie();

	void Execution();

private:


	//// UAVバッファ
	//Microsoft::WRL::ComPtr<ID3D12Resource> vertBuffUAV_;
	//// UAVバッファマップ
	//VertexData* vertMapUAV_ = nullptr;

	//// CPUハンドル
	//D3D12_CPU_DESCRIPTOR_HANDLE vertUAVHandleCPU_{};
	//// GPUハンドル
	//D3D12_GPU_DESCRIPTOR_HANDLE vertUAVHandleGPU_{};
	//// ディスクリプタヒープの位置
	//uint32_t vertUAVIndexDescriptorHeap_ = 0;


};

