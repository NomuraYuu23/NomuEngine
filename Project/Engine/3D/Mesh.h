#pragma once
#include <DirectXMath.h>
#include <Windows.h>
#include <d3d12.h>
#include <string>
#include <wrl.h>
#include <dxcapi.h>

#pragma comment(lib, "dxcompiler.lib")

#include "VertexData.h"
#include "VertexInfluence.h"

class Mesh
{

public: // 関数

	/// <summary>
	/// デストラクタ
	/// </summary>
	~Mesh();

	/// <summary>
	/// メッシュデータ生成
	/// </summary>
	/// <param name="sDevice">デバイス</param>
	/// <param name="vertices">頂点データ</param>
	/// <param name="vertexInfluences">インフルエンスデータ</param>
	void CreateMesh(
		ID3D12Device* sDevice,
		const std::vector<VertexData>& vertices,
		const std::vector<VertexInfluence>& vertexInfluences);

	/// <summary>
	/// 頂点バッファビュー
	/// </summary>
	/// <returns></returns>
	D3D12_VERTEX_BUFFER_VIEW* GetVbView() { return &vbView_; }

	/// <summary>
	/// インフルエンスバッファビュー
	/// </summary>
	/// <returns></returns>
	D3D12_VERTEX_BUFFER_VIEW* GetInfluenceView() { return &influenceView_; }

	/// <summary>
	/// UAVバッファビュー
	/// </summary>
	/// <returns></returns>
	D3D12_VERTEX_BUFFER_VIEW* GetVbViewUAV() { return &vbViewUAV_; }

	D3D12_GPU_DESCRIPTOR_HANDLE* GetVertHandleGPU() { return &vertHandleGPU_; }
	D3D12_GPU_DESCRIPTOR_HANDLE* GetInfluenceHandleGPU() { return &influenceHandleGPU_; }
	D3D12_GPU_DESCRIPTOR_HANDLE* GetVertUAVHandleGPU() { return &vertUAVHandleGPU_; }

private: // 関数

	/// <summary>
	/// 頂点バッファの初期化
	/// </summary>
	/// <param name="sDevice">デバイス</param>
	/// <param name="vertices">頂点データ</param>
	void VertBuffInitialize(
		ID3D12Device* sDevice,
		const std::vector<VertexData>& vertices);

	/// <summary>
	/// インフルエンスバッファの初期化
	/// </summary>
	/// <param name="sDevice">デバイス</param>
	/// <param name="vertices">インフルエンスデータ</param>
	void VertInfluenceBuffInitialize(
		ID3D12Device* sDevice,
		const std::vector<VertexInfluence>& vertexInfluences);

	/// <summary>
	/// UAVバッファの初期化
	/// </summary>
	/// <param name="sDevice">デバイス</param>
	/// <param name="vertices">頂点データ</param>
	void UAVBuffInitialize(
		ID3D12Device* sDevice,
		const std::vector<VertexData>& vertices);

private:

	// 頂点バッファ
	Microsoft::WRL::ComPtr<ID3D12Resource> vertBuff_;
	// 頂点バッファマップ
	VertexData* vertMap_ = nullptr;
	// 頂点バッファビュー
	D3D12_VERTEX_BUFFER_VIEW vbView_{};

	// CPUハンドル
	D3D12_CPU_DESCRIPTOR_HANDLE vertHandleCPU_{};
	// GPUハンドル
	D3D12_GPU_DESCRIPTOR_HANDLE vertHandleGPU_{};
	// ディスクリプタヒープの位置
	uint32_t vertIndexDescriptorHeap_ = 0;

	// インフルエンスバッファ
	Microsoft::WRL::ComPtr<ID3D12Resource> influenceBuff_;
	// インフルエンスバッファマップ
	VertexInfluence* influenceMap_ = nullptr;
	// インフルエンスバッファビュー
	D3D12_VERTEX_BUFFER_VIEW influenceView_{};

	// CPUハンドル
	D3D12_CPU_DESCRIPTOR_HANDLE influenceHandleCPU_{};
	// GPUハンドル
	D3D12_GPU_DESCRIPTOR_HANDLE influenceHandleGPU_{};
	// ディスクリプタヒープの位置
	uint32_t influenceIndexDescriptorHeap_ = 0;

	// UAVバッファ
	Microsoft::WRL::ComPtr<ID3D12Resource> vertBuffUAV_;
	// UAVバッファマップ
	VertexData* vertMapUAV_ = nullptr;
	// UAVバッファビュー
	D3D12_VERTEX_BUFFER_VIEW vbViewUAV_{};

	// CPUハンドル
	D3D12_CPU_DESCRIPTOR_HANDLE vertUAVHandleCPU_{};
	// GPUハンドル
	D3D12_GPU_DESCRIPTOR_HANDLE vertUAVHandleGPU_{};
	// ディスクリプタヒープの位置
	uint32_t vertUAVIndexDescriptorHeap_ = 0;
	
};

