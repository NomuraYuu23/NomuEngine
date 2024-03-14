#pragma once
#include <Windows.h>
#include <chrono>
#include <cstdlib>
#include <d3d12.h>
#include "../../externals/DirectXTex/d3dx12.h"
#include <dxgi1_6.h>
#include <wrl.h>

#include "WinApp.h"
#include "../2D/SpriteVertex.h"

class RenderTargetTexture
{

public: 

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="device">デバイス</param>
	void Initialize(ID3D12Device* device);

	/// <summary>
	/// 描画前処理
	/// </summary>
	/// <param name="commandList">コマンドリスト</param>
	void PreDraw(ID3D12GraphicsCommandList* commandList);

	/// <summary>
	/// 描画後処理
	/// </summary>
	void PostDraw();

	/// <summary>
	/// GPUに送る
	/// </summary>
	/// <param name="cmdList">コマンドリスト</param>
	/// <param name="rootParameterIndex">ルートパラメータインデックス</param>
	void SetGraphicsRootDescriptorTable(ID3D12GraphicsCommandList* cmdList, uint32_t rootParameterIndex);

private: // RTV,DSV

	// 幅
	int32_t backBufferWidth_;
	// 高さ
	int32_t backBufferHeight_;

	// RTV, SRVリソース
	Microsoft::WRL::ComPtr<ID3D12Resource> resource_;
	// RTVハンドル
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle_;
	// RTVのIndex
	uint32_t rtvIndexDescriptorHeap_;

	// SRVのハンドル(CPU)
	CD3DX12_CPU_DESCRIPTOR_HANDLE cpuHandleSRV;
	// SRVのハンドル(GPU)
	CD3DX12_GPU_DESCRIPTOR_HANDLE gpuHandleSRV;
	//  SRVのインデックス
	uint32_t indexDescriptorHeapSRV;

	// DSVリソース
	Microsoft::WRL::ComPtr<ID3D12Resource> dsvResource_;
	// DSVハンドル
	D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle_;
	// DSVのIndex
	uint32_t dsvIndexDescriptorHeap_;

	// コマンドリスト
	ID3D12GraphicsCommandList* commandList_;

};

