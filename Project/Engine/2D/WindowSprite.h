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
	const int kVertNum = 3;
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

};

