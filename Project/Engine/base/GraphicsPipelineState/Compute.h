#pragma once
#include <d3d12.h>
#include <wrl.h>
#include <vector>

#include "../DirectXCommon.h"

class Compute
{

public:

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="device">デバイス</param>
	void Initialize(ID3D12Device* device);
	
	/// <summary>
	/// 実行
	/// </summary>
	/// <param name="dxCommon">DirectX/param>
	void Execution(DirectXCommon* dxCommon);

private: // 関数

	void CreateRootSignature();

	void CreatePipelineState();

	//void CreateHerp();

	void CreateResource();

	void CreateUAV();

	void Map();

private:

	// デバイス
	ID3D12Device* device_;

	// ルートシグネチャ
	Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature_;

	// シェーダー情報
	Microsoft::WRL::ComPtr<IDxcBlob> shader_;

	//パイプライン
	Microsoft::WRL::ComPtr<ID3D12PipelineState> pipelineState_;

	// リソース
	Microsoft::WRL::ComPtr<ID3D12Resource> resource_;

	// データ(お試し)
	void* data_;

	// CPUハンドル
	D3D12_CPU_DESCRIPTOR_HANDLE uavHandleCPU_;

	// GPUハンドル
	D3D12_GPU_DESCRIPTOR_HANDLE uavHandleGPU_;

	// ディスクリプタヒープの位置
	uint32_t indexDescriptorHeap_ = 0;

};

