#pragma once

#include <DirectXMath.h>
#include <Windows.h>
#include <d3d12.h>
#include <string>
#include <wrl.h>
#include <dxcapi.h>
#include <array>

#pragma comment(lib, "dxcompiler.lib")

class GPUPaticle
{

public:


	/// <summary>
	/// CSのパイプラインステート
	/// </summary>
	enum PipelineStateCSIndex {
		kPiprlineStateCSIndexInitialize, // 初期化
		//kPiprlineStateCSIndexUpdate,
		kPipelineStateCSIndexOfCount // 数える用
	};


public:

	void Initialize(
		ID3D12Device* device,
		ID3D12GraphicsCommandList* commandList);

	void Execution();

private:

	/// <summary>
	/// パイプラインステートの初期化CS
	/// </summary>
	void PipelineStateCSInitialize(ID3D12Device* device);

	/// <summary>
	/// バッファの初期化
	/// </summary>
	/// <param name="device"></param>
	void BuffInitialize(ID3D12Device* device,
		ID3D12GraphicsCommandList* commandList);

private:

	// パーティクルの最大数
	const uint32_t kParticleMax = 1024;

	// UAVバッファ
	Microsoft::WRL::ComPtr<ID3D12Resource> buff_;

	// CPUハンドル
	D3D12_CPU_DESCRIPTOR_HANDLE uavHandleCPU_{};
	// GPUハンドル
	D3D12_GPU_DESCRIPTOR_HANDLE uavHandleGPU_{};
	// ディスクリプタヒープの位置
	uint32_t uavIndexDescriptorHeap_ = 0;

	// CPUハンドル
	D3D12_CPU_DESCRIPTOR_HANDLE srvHandleCPU_{};
	// GPUハンドル
	D3D12_GPU_DESCRIPTOR_HANDLE srvHandleGPU_{};
	// ディスクリプタヒープの位置
	uint32_t srvIndexDescriptorHeap_ = 0;

	// ルートシグネチャCS
	std::array<Microsoft::WRL::ComPtr<ID3D12RootSignature>, kPipelineStateCSIndexOfCount> rootSignaturesCS_;
	// パイプラインステートオブジェクトCS
	std::array<Microsoft::WRL::ComPtr<ID3D12PipelineState>, kPipelineStateCSIndexOfCount> pipelineStatesCS_;

};

