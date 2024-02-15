#pragma once
#include <d3d12.h>
#include <string>
#include <dxcapi.h>
#include <cassert>
#include <format>
#include <wrl.h>
#include <array>
#include <vector>

/// <summary>
/// サンプラーの名前
/// </summary>
enum SamplerIndex {
	kSamplerIndexNormal,
	kSamplerIndexOfCount,
};

class SamplerManager
{

public: // ルートパラメータの関数

	/// <summary>
	/// シングルトンインスタンスの取得
	/// </summary>
	/// <returns></returns>
	static SamplerManager* GetInstance();

	/// <summary>
	/// 初期化
	/// </summary>
	static void Initialize();

public: // サンプラーの変数

	static std::array<std::vector<D3D12_STATIC_SAMPLER_DESC>, kSamplerIndexOfCount> samplers_;

private: // シングルトン

	SamplerManager() = default;
	~SamplerManager() = default;
	SamplerManager(const SamplerManager&) = delete;
	SamplerManager& operator=(const SamplerManager&) = delete;

};

