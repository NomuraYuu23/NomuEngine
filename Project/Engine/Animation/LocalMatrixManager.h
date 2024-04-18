#pragma once
#include <wrl.h>
#include <d3d12.h>
#include "LocalMatrix.h"
#include "../3D/ModelNodeData.h"

class LocalMatrixManager
{

public:

	// コマンドリスト
	static ID3D12GraphicsCommandList* sCommandList;

public:

	~LocalMatrixManager();

	void Initialize(const std::vector<NodeData>& nodeDatas);

	void Map(const std::vector<NodeData>& nodeDatas);
	
	/// <summary>
	/// GPUに送る
	/// </summary>
	/// <param name="cmdList">コマンドリスト</param>
	/// <param name="rootParameterIndex">ルートパラメータインデックス</param>
	void SetGraphicsRootDescriptorTable(ID3D12GraphicsCommandList* cmdList, uint32_t rootParameterIndex);


public:

	// local
	Microsoft::WRL::ComPtr<ID3D12Resource> localMatrixesBuff_;
	//書き込むためのアドレスを取得
	LocalMatrix* localMatrixesMap_{};
	// CPUハンドル
	D3D12_CPU_DESCRIPTOR_HANDLE localMatrixesHandleCPU_;
	// GPUハンドル
	D3D12_GPU_DESCRIPTOR_HANDLE localMatrixesHandleGPU_;
	// ディスクリプタヒープの位置
	uint32_t indexDescriptorHeap_ = 0;

};

