#pragma once
#include <cstdint>
#include "PointLight.h"

class Model;
class BaseCamera;

class PointLightManager
{

public: // 静的メンバ変数

	// パーティクル最大数
	static const uint32_t kNumInstanceMax_ = 4;

private:

	// デバイス
	static ID3D12Device* sDevice;
	// コマンドリスト
	static ID3D12GraphicsCommandList* sCommandList;

public: // メンバ関数

	/// <summary>
	/// インスタンス取得
	/// </summary>
	/// <returns></returns>
	static PointLightManager* GetInstance();

	/// <summary>
	/// 静的初期化
	/// </summary>
	/// <param name="device">デバイス</param>
	static void StaticInitialize(ID3D12Device* device);

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// SRVを作る
	/// </summary>
	void SRVCreate();

	/// <summary>
	/// 更新
	/// </summary>
	void Update(const std::array<PointLightData, PointLightManager::kNumInstanceMax_>& pointLightDatas);

	/// <summary>
	/// 描画
	/// </summary>
	/// <param name="viewProjection"></param>
	void Draw(ID3D12GraphicsCommandList* cmdList, uint32_t rootParameterIndex);

private:

	//WVP用のリソースを作る。
	Microsoft::WRL::ComPtr<ID3D12Resource> pointLightDataBuff_;
	//書き込むためのアドレスを取得
	PointLightData* pointLightDataMap_{};

	D3D12_CPU_DESCRIPTOR_HANDLE instancingSrvHandleCPU_;

	D3D12_GPU_DESCRIPTOR_HANDLE instancingSrvHandleGPU_;

};

