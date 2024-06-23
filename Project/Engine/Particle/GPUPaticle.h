#pragma once

#include <DirectXMath.h>
#include <Windows.h>
#include <d3d12.h>
#include <string>
#include <wrl.h>
#include <dxcapi.h>
#include <array>
#include "../3D/Model.h"
#include "GPUPerticleView.h"

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

	/// <summary>
	/// インスタンス取得
	/// </summary>
	/// <returns></returns>
	static GPUPaticle* GetInstance();

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="device">デバイス</param>
	/// <param name="commandList">コマンドリスト</param>
	/// <param name="rootSignature">ルートシグネチャ</param>
	/// <param name="pipelineState">パイプラインステート</param>
	void Initialize(
		ID3D12Device* device,
		ID3D12GraphicsCommandList* commandList,
		ID3D12RootSignature* rootSignature,
		ID3D12PipelineState* pipelineState);

	/// <summary>
	/// 描画
	/// </summary>
	/// <param name="commandList">コマンドリスト</param>
	/// <param name="camera">カメラ</param>
	void Draw(
		ID3D12GraphicsCommandList* commandList,
		BaseCamera& camera);

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

	/// <summary>
	/// モデルの初期化
	/// </summary>
	void ModelInitialize();

	/// <summary>
	/// GPUParticleViewマッピング
	/// </summary>
	/// <param name="camera">カメラ</param>
	void GPUParticleViewMapping(BaseCamera& camera);

private:

	// パーティクルの最大数
	const uint32_t kParticleMax = 1024;
	// モデルのディレクトリパス
	const std::string kDirectoryPath = "Resources/Particle/";
	// モデルのファイルの名前
	const std::string kFilename = "plane.obj";

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

	// ルートシグネチャ
	ID3D12RootSignature* rootSignature_;
	// パイプラインステートオブジェクト
	ID3D12PipelineState* pipelineState_;

	// モデル
	std::unique_ptr<Model> model_;

	// マテリアル
	std::unique_ptr<Material> material_;

	// GPUParticleViewバッファ
	Microsoft::WRL::ComPtr<ID3D12Resource> gpuParticleViewBuff_;

	// GPUParticleViewマップ
	GPUParticleView* gpuParticleViewMap = nullptr;

private: // シングルトン

	GPUPaticle() = default;
	~GPUPaticle() = default;
	GPUPaticle(const GPUPaticle&) = delete;
	GPUPaticle& operator=(const GPUPaticle&) = delete;

};

