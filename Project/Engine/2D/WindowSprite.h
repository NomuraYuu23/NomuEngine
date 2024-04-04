#pragma once
#include <memory>
#include "../../externals/DirectXTex/d3dx12.h"
#include "SpriteVertex.h"
#include "../Math/Matrix4x4.h"
#include "SpriteForGPU.h"
#include "../3D/Material.h"

class WindowSprite
{

private:

	// 頂点数
	const int kVertNum = 6;
	// デバイス
	ID3D12Device* device_;
	// コマンドリスト
	ID3D12GraphicsCommandList* commandList_;

public:

	/// <summary>
	/// インスタンス取得
	/// </summary>
	static WindowSprite* GetInstance();

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="device">デバイス</param>
	/// <param name="commandList">コマンドリスト</param>
	void Initialize(
		ID3D12Device* device,
		ID3D12GraphicsCommandList* commandList);

	/// <summary>
	/// 描画
	/// </summary>
	/// <param name="srvGPUHandle">GPUハンドル</param>
	void Draw(const CD3DX12_GPU_DESCRIPTOR_HANDLE& srvGPUHandle);

private:
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

};

