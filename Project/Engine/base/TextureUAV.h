#pragma once
#include <array>
#include "../../externals/DirectXTex/d3dx12.h"
#include <string>
#include <unordered_map>
#include <wrl.h>
#include "../../externals/DirectXTex/DirectXTex.h"

#include "WinApp.h"
#include "DirectXCommon.h"
#include "ITextureHandleManager.h"
#include "SRVDescriptorHerpManager.h"

using namespace DirectX;

class TextureUAV
{

public:

	/// <summary>
	/// デストラクタ
	/// </summary>
	~TextureUAV();

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="device">デバイス</param>
	/// <param name="width">幅</param>
	/// <param name="height">高さ</param>
	void Initialize(
		ID3D12Device* device,
		int32_t width,
		int32_t height);

	/// <summary>
	/// DescriptorTableをセット
	/// </summary>
	/// <param name="commandList">コマンドリスト</param>
	/// <param name="rootParamIndex">ルートパラメータの番号</param>
	void SetRootDescriptorTable(ID3D12GraphicsCommandList* commandList, UINT rootParamIndex);

	/// <summary>
	/// バリア
	/// </summary>
	/// <param name="commandList">コマンドリスト</param>
	void Barrier(ID3D12GraphicsCommandList* commandList);

private: // 変数
	
	// テクスチャリソース
	Microsoft::WRL::ComPtr<ID3D12Resource> resource_;
	// シェーダーリソースビューのハンドル(CPU)
	CD3DX12_CPU_DESCRIPTOR_HANDLE uavHandleCPU_;
	// シェーダーリソースビューのハンドル(GPU)
	CD3DX12_GPU_DESCRIPTOR_HANDLE uavHandleGPU_;
	//  SRVのインデックス
	uint32_t indexDescriptorHeap_;

};

