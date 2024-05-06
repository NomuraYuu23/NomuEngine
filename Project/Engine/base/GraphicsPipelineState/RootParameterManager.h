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
/// ディスクリプタレンジの名前
/// </summary>
enum DescriptorRangeIndex {
	kDescriptorRangeIndexTexture0, // テクスチャ0
	kDescriptorRangeIndexTexture1, // テクスチャ1
	kDescriptorRangeIndexTexture2, // テクスチャ2
	kDescriptorRangeIndexTexture3, // テクスチャ3
	kDescriptorRangeIndexInstancing, // パーティクル
	kDescriptorRangeIndexPointLight, // ポイントライト
	kDescriptorRangeIndexSpotLight, // スポットライト
	kDescriptorRangeIndexLocalMatrix, // ローカル行列
	kDescriptorRangeIndexCollider2DDebugDrawForGPU, // コライダーデバッグ2d
	kDescriptorRangeIndexTransformationMatrix, // トランスフォーム行列
	kDescriptorRangeIndexLineForGPU, // 線描画
	kDescriptorRangeIndexMaterials, // マテリアル
	kDescriptorRangeIndexOfCount,
};

/// <summary>
/// ルートパラメータの名前
/// </summary>
enum RootParameterIndex {
	kRootParameterIndexAnimModel, // アニメーションモデル
	kRootParameterIndexNormalModel, // アニメーション無しモデル
	kRootParameterIndexSprite, // スプライト
	kRootParameterIndexParticle, // パーティクル
	kRootParameterIndexCollision2DDebugDraw, // コライダーデバッグ2d
	kRootParameterIndexLine, // 線
	kRootParameterIndexWindowSprite, // ウィンドウスプライト
	kRootParameterIndexManyAnimModels, // たくさんのアニメーションモデル
	kRootParameterIndexManyNormalModels, // たくさんのアニメーション無しモデル
	kRootParameterIndexOfCount,
};

class RootParameterManager
{

public: // 関数

	/// <summary>
	/// シングルトンインスタンスの取得
	/// </summary>
	/// <returns></returns>
	static RootParameterManager* GetInstance();

	/// <summary>
	/// 初期化
	/// </summary>
	static void Initialize();

private: // 関数

	/// <summary>
	/// アニメーションモデル
	/// </summary>
	static void CreateForAnimModel();

	/// <summary>
	/// アニメーション無しモデル
	/// </summary>
	static void CreateForNormalModel();

	/// <summary>
	/// スプライト
	/// </summary>
	static void RootParameterInitializeForSprite();

	/// <summary>
	/// パーティクル
	/// </summary>	
	static void RootParameterInitializeForParticle();

	/// <summary>
	/// コライダーデバッグ2D
	/// </summary>
	static void RootParameterInitializeForCollision2DDebugDraw();

	/// <summary>
	/// 線
	/// </summary>
	static void RootParameterInitializeForLine();

	/// <summary>
	/// ポストエフェクト
	/// </summary>
	static void RootParameterInitializeForSwapChain();

	/// <summary>
	/// たくさんのアニメーションモデル
	/// </summary>
	static void RootParameterInitializeForManyAnimModels();

	/// <summary>
	/// たくさんのアニメーション無しモデル
	/// </summary>
	static void RootParameterInitializeForManyNormalModels();

	/// <summary>
	/// ディスクリプタレンジの初期化
	/// </summary>
	static void DescriptorRangeInitialize();

public: // 変数

	// ルートパラメータ
	static std::array<std::vector<D3D12_ROOT_PARAMETER>, kRootParameterIndexOfCount> rootParameters_;
	// ディスクリプタレンジ
	static std::array<std::vector<D3D12_DESCRIPTOR_RANGE>, kDescriptorRangeIndexOfCount> descriptorRanges_;

private: // シングルトン

	RootParameterManager() = default;
	~RootParameterManager() = default;
	RootParameterManager(const RootParameterManager&) = delete;
	RootParameterManager& operator=(const RootParameterManager&) = delete;

};

