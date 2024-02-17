#pragma once
#include <DirectXMath.h>
#include <Windows.h>
#include <d3d12.h>
#include <string>
#include <wrl.h>
#include <dxcapi.h>

#pragma comment(lib, "dxcompiler.lib")

#include "SpotLightData.h"

class SpotLight
{

public:

	/// <summary>
	/// 静的初期化
	/// </summary>
	/// <param name="device">デバイス</param>
	static void StaticInitialize(ID3D12Device* device);

	/// <summary>
	/// 光源生成
	/// </summary>
	/// <returns></returns>
	static SpotLight* Create();

private:

	// デバイス
	static ID3D12Device* sDevice;
	// コマンドリスト
	static ID3D12GraphicsCommandList* sCommandList;

public:

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 更新
	/// </summary>
	void Update(const SpotLightData& spotLightData);

	/// <summary>
	/// 描画
	/// </summary>
	void Draw(ID3D12GraphicsCommandList* cmdList, uint32_t rootParameterIndex);

private:

	Microsoft::WRL::ComPtr<ID3D12Resource> spotLightBuff_;

	SpotLightData* spotLightMap = nullptr;

};



