#pragma once
#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl.h>
#include <cstdint>
#include "WinApp.h"
#include "DXGIDevice.h"

class SwapChain
{

public: // 関数
	
	/// <summary>
	/// インスタンス取得
	/// </summary>
	/// <returns></returns>
	static SwapChain* GetInstance();

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="backBufferWidth">幅</param>
	/// <param name="backBufferHeight">高さ</param>
	/// <param name="dxgiDevice">dxgiDevice</param>
	/// <param name="commandQueue">コマンドキュー</param>
	/// <param name="winApp">ウィンドウアプリケーション</param>
	void Initialize(
		int32_t backBufferWidth,
		int32_t backBufferHeight,
		DXGIDevice* dxgiDevice,
		ID3D12CommandQueue * commandQueue,
		WinApp* winApp);

	/// <summary>
	/// スワップチェーン入手
	/// </summary>
	/// <returns></returns>
	IDXGISwapChain4* GetSwapChain() { return swapChain_.Get(); }

	/// <summary>
	/// リソース
	/// </summary>
	/// <param name="index">インデックス</param>
	/// <returns></returns>
	ID3D12Resource* GetResource(uint32_t index) { resources_[index].Get(); }
	
	/// <summary>
	/// GPUとOSに画面の交換を行うように通知する
	/// </summary>
	void Present() { swapChain_->Present(1, 0); }

	void PreDraw(ID3D12GraphicsCommandList* commandList,
		const D3D12_CPU_DESCRIPTOR_HANDLE& dsvHandle);

	void PostDraw();

private: // 変数

	// スワップチェーンの数
	static const uint32_t kSwapChainNum_ = 2;

	// スワップチェーン
	Microsoft::WRL::ComPtr<IDXGISwapChain4> swapChain_;
	// リソース
	Microsoft::WRL::ComPtr<ID3D12Resource> resources_[kSwapChainNum_];
	// RTVハンドル
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandles_[kSwapChainNum_];
	// RTVのIndex
	uint32_t indexDescriptorHeaps_[kSwapChainNum_];

	// コマンドリスト
	ID3D12GraphicsCommandList* commandList_;

};

