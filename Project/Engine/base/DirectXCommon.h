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

// クラス
#include "DXGIDevice.h"

/// <summary>
/// DirectX汎用
/// </summary>
class DirectXCommon
{
public:
	/// <summary>
	/// シングルトンインスタンスの取得
	/// </summary>
	/// <returns></returns>
	static DirectXCommon* GetInstance();

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="winApp">WinApp</param>
	/// <param name="backBufferWidth"></param>
	/// <param name="backBufferHeight"></param>
	void Initialize(
		WinApp* winApp, int32_t backBufferWidth = WinApp::kWindowWidth,
		int32_t backBufferHeight = WinApp::kWindowHeight);

	/// <summary>
	/// 描画前処理
	/// </summary>
	void PreDraw();

	/// <summary>
	/// 描画後処理
	/// </summary>
	void PostDraw();

	/// <summary>
	/// レンダーターゲットのクリア
	/// </summary>
	void ClearRenderTarget();

	/// <summary>
	/// 深度バッファのクリア
	/// </summary>
	void ClearDepthBuffer();

	ID3D12Device* GetDevice() const { return dxgiDevice_->GetDevice(); }

	ID3D12GraphicsCommandList* GetCommadList() const { return commandList_.Get(); }

	ID3D12GraphicsCommandList* GetCommadListLoad() const { return commandListLoad_.Get(); }

	ID3D12CommandAllocator* GetCommandAllocator() const { return commandAllocator_.Get(); }

	ID3D12CommandAllocator* GetCommandAllocatorLoad() const { return commandAllocatorLoad_.Get(); }

	ID3D12CommandQueue* GetCommandQueue() const { return commandQueue_.Get(); }

	ID3D12Fence* GetFence() const { return fence_.Get(); }

	UINT64 GetFenceVal() const { return fenceVal_; }

	void SetFenceVal(UINT64 fenceVal) { fenceVal_ = fenceVal; }

	int32_t GetBackBufferWidth() const { return backBufferWidth_; }

	int32_t GetBackBufferHeight() const { return backBufferHeight_; }

private:

	WinApp* winApp_;

	//Direct3D関連
	//Microsoft::WRL::ComPtr<IDXGIFactory7> dxgiFactory_;
	//Microsoft::WRL::ComPtr<ID3D12Device> device_;

	DXGIDevice* dxgiDevice_;

	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList_;
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandListLoad_;
	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> commandAllocator_;
	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> commandAllocatorLoad_;
	Microsoft::WRL::ComPtr<ID3D12CommandQueue> commandQueue_;
	Microsoft::WRL::ComPtr<IDXGISwapChain4> swapChain_;
	Microsoft::WRL::ComPtr<ID3D12Resource> swapChainResources[2];
	Microsoft::WRL::ComPtr<ID3D12Resource> depthStencilResource;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> rtvHeap_;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> dsvHeap_;
	Microsoft::WRL::ComPtr<ID3D12Fence> fence_;
	UINT64 fenceVal_ = 0;
	int32_t backBufferWidth_ = 0;
	int32_t backBufferHeight_ = 0;

	// 記録時間(FPS固定用)
	std::chrono::steady_clock::time_point reference_;

public: // マルチパスレンダリング

	Microsoft::WRL::ComPtr<ID3D12Resource> rtvTmp_;
	// シェーダーリソースビューのハンドル(CPU)
	CD3DX12_CPU_DESCRIPTOR_HANDLE cpuHandleSRV;
	// シェーダーリソースビューのハンドル(GPU)
	CD3DX12_GPU_DESCRIPTOR_HANDLE gpuHandleSRV;
	//  DSVのインデックス
	uint32_t indexDescriptorHeapSRV;

	// ルートシグネチャ
	ID3D12RootSignature* postRootSignature_;
	// パイプラインステートオブジェクト
	ID3D12PipelineState* postPipelineState_;

	// 頂点バッファ
	Microsoft::WRL::ComPtr<ID3D12Resource> vertBuff_;
	// 頂点バッファマップ
	SpriteVertex* vertMap = nullptr;
	// 頂点バッファビュー
	D3D12_VERTEX_BUFFER_VIEW vbView_{};

	Microsoft::WRL::ComPtr<ID3D12Resource> indexBuff_;

	//インデックスバッファビュー
	D3D12_INDEX_BUFFER_VIEW ibView_{};

	//インデックスリソースにデータを書き込む
	uint32_t* indexMap = nullptr;

private:
	DirectXCommon() = default;
	~DirectXCommon() = default;
	DirectXCommon(const DirectXCommon&) = delete;
	const DirectXCommon& operator=(const DirectXCommon&) = delete;

	D3D12_CPU_DESCRIPTOR_HANDLE GetCPUDescriptorHandle(ID3D12DescriptorHeap* descriptorHeap, uint32_t descriptorSize, uint32_t index);

	D3D12_GPU_DESCRIPTOR_HANDLE GetGPUDescriptorHandle(ID3D12DescriptorHeap* descriptorHeap, uint32_t descriptorSize, uint32_t index);

	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> CreateDescriptorHeap(
		Microsoft::WRL::ComPtr<ID3D12Device> device, const D3D12_DESCRIPTOR_HEAP_TYPE& heapType, UINT numDescriptors, bool shaderVisible);

	Microsoft::WRL::ComPtr<ID3D12Resource> CreateDepthStencilTextureResource(Microsoft::WRL::ComPtr<ID3D12Device> device, int32_t width, int32_t height);

	/// <summary>
	/// DXGIデバイス初期化
	/// </summary>
	//void InitializeDXGIDevice();

	/// <summary>
	/// スワップチェーンの生成
	/// </summary>
	void CreateSwapChain();

	/// <summary>
	/// コマンド関連初期化
	/// </summary>
	void Initializecommand();

	/// <summary>
	/// 深度バッファ生成
	/// </summary>
	void CreateDepthBuffer();

	/// <summary>
	/// フェンス生成
	/// </summary>
	void CreateFence();

	/// <summary>
	/// FPS固定初期化
	/// </summary>
	void InitializeFixFPS();

	/// <summary>
	/// FPS固定更新
	/// </summary>
	void UpdateFixFPS();

public:

	/// <summary>
	/// レンダーターゲット生成
	/// </summary>
	void CreateFinalRenderTarget();

	void PostEffectInitialize(ID3D12RootSignature* postRootSignature,
		ID3D12PipelineState* postPipelineState);

	void PostEffect();

};

