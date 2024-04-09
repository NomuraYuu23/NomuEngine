#pragma once
#include "ShockWaveData.h"
#include <wrl/client.h>
#include <d3d12.h>

class ShockWaveManager
{

public: // 関数

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();
	
	/// <summary>
	/// 更新処理
	/// </summary>
	void Update();

	/// <summary>
	/// ImGui描画
	/// </summary>
	void ImGuiDraw();
	
	/// <summary>
	/// ShockWaveバッファ取得
	/// </summary>
	/// <returns></returns>
	ID3D12Resource* GetShockWaveDataBuff() { return shockWaveDataBuff_.Get(); }

private: //変数

	// デバイス
	ID3D12Device* device_;

	// ShockWaveバッファ
	Microsoft::WRL::ComPtr<ID3D12Resource> shockWaveDataBuff_;
	// ShockWaveマップ
	ShockWaveData* shockWaveDataMap_;

};

