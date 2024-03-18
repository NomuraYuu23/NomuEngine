#pragma once
#include <d3d12.h>
#include <wrl.h>
#include <vector>

#include "../DirectXCommon.h"
#include "BaseConpute.h"

class TestCompute : public BaseConpute
{

public:

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="device">シェーダーファイルのパス</param>
	void Initialize(const std::wstring& filePath) override;
	
	/// <summary>
	/// 実行
	/// </summary>
	/// <param name="dxCommon">DirectX/param>
	void Execution(DirectXCommon* dxCommon) override;

private: // 関数

	/// <summary>
	/// ルートシグネチャを作る
	/// </summary>
	void CreateRootSignature() override;

	/// <summary>
	/// リソース作成
	/// </summary>
	void CreateResource();

	/// <summary>
	/// UAV作成
	/// </summary>
	void CreateUAV();

	/// <summary>
	/// マップ
	/// </summary>
	void Map();

private:

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

