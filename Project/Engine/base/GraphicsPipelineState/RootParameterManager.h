#pragma once
#include <d3d12.h>
#include <string>
#include <dxcapi.h>
#include <cassert>
#include <format>
#include <wrl.h>
#include <array>
#include <vector>
#include <variant>

/// <summary>
/// ディスクリプタレンジの名前 PS
/// </summary>
enum DescriptorRangePSIndex {
	kDescriptorRangePSIndexTexture0 = 0, // テクスチャ0
	kDescriptorRangePSIndexTexture1 = 1, // テクスチャ1
	kDescriptorRangePSIndexTexture2 = 2, // テクスチャ2
	kDescriptorRangePSIndexTexture3 = 3, // テクスチャ3
	kDescriptorRangePSIndexTexture4 = 4, // テクスチャ4
	kDescriptorRangePSIndexTexture5 = 5, // テクスチャ5
	kDescriptorRangePSIndexTexture6 = 6, // テクスチャ6
	kDescriptorRangePSIndexTexture7 = 7, // テクスチャ7
	kDescriptorRangePSIndexPointLight = 8, // ポイントライト
	kDescriptorRangePSIndexSpotLight = 9, // スポットライト
	kDescriptorRangePSIndexMaterials = 10, // マテリアル
	kDescriptorRangePSIndexOfCount,
};

/// <summary>
/// ディスクリプタレンジの名前 VS
/// </summary>
enum DescriptorRangeVSIndex {
	kDescriptorRangeVSIndexParticleForGPU = 0, //  パーティクル用
	kDescriptorRangeVSIndexLocalMatrix = 1, // ローカル行列
	kDescriptorRangeVSIndexCollider2DDebugDrawForGPU = 2, // コライダーデバッグ2d
	kDescriptorRangeVSIndexTransformationMatrix = 3, // トランスフォーム行列
	kDescriptorRangeVSIndexLineForGPU = 4, // 線描画
	kDescriptorRangeVSIndexOfCount,
};

/// <summary>
/// CBV PS
/// </summary>
enum CBV_PSIndex {

	kCBV_PSIndexMaterial = 0,// マテリアル
	kCBV_PSIndexDirectionalLight = 1,// 平行光源
	kCBV_PSIndexCamera = 2,// カメラ
	kCBV_PSIndexFog = 3,// 霧
	kCBV_PSIndexOfCount

};

/// <summary>
/// CBV VS
/// </summary>
enum CBV_VSIndex {

	kCBV_VSIndexWorldTransform = 0,// ワールドトランスフォーム
	kCBV_VSIndexOutlineData = 1,// アウトラインデータ
	kCBV_VSIndexParticleStart = 2,// パーティクルの開始位置
	kCBV_VSIndexViewProjection = 3,// viewProjection
	kCBV_VSIndexSpriteForGPU = 4, // スプライト用
	kCBV_VSIndexOfCount

};

/// <summary>
/// ルートパラメータの名前
/// </summary>
enum RootParameterIndex {
	kRootParameterIndexAnimModel, // アニメーションモデル
	kRootParameterIndexNormalModel, // アニメーション無しモデル

	kRootParameterIndexNormalOutline, // アニメーション無しアウトライン

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

private:

	using Item = std::variant<DescriptorRangePSIndex, DescriptorRangeVSIndex, CBV_PSIndex, CBV_VSIndex>;

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

	///// <summary>
	///// アニメーションモデル
	///// </summary>
	//static void CreateForAnimModel();

	///// <summary>
	///// アニメーション無しモデル
	///// </summary>
	//static void CreateForNormalModel();

	///// <summary>
	///// アニメーション無しアウトライン
	///// </summary>
	//static void CreateForNormalOutline();

	///// <summary>
	///// スプライト
	///// </summary>
	//static void RootParameterInitializeForSprite();

	///// <summary>
	///// パーティクル
	///// </summary>	
	//static void RootParameterInitializeForParticle();

	///// <summary>
	///// コライダーデバッグ2D
	///// </summary>
	//static void RootParameterInitializeForCollision2DDebugDraw();

	///// <summary>
	///// 線
	///// </summary>
	//static void RootParameterInitializeForLine();

	///// <summary>
	///// ポストエフェクト
	///// </summary>
	//static void RootParameterInitializeForSwapChain();

	///// <summary>
	///// たくさんのアニメーションモデル
	///// </summary>
	//static void RootParameterInitializeForManyAnimModels();

	///// <summary>
	///// たくさんのアニメーション無しモデル
	///// </summary>
	//static void RootParameterInitializeForManyNormalModels();

	/// <summary>
	/// ディスクリプタレンジの初期化
	/// </summary>
	static void DescriptorRangeInitialize();

	/// <summary>
	/// データ解析
	/// </summary>
	/// <param name="rootParameterIndex">ルートパラメータ番号</param>
	/// <param name="data">データ</param>
	static void Analyze(RootParameterIndex rootParameterIndex, const std::vector<Item>& data);

public: // 変数

	// ルートパラメータ
	static std::array<std::vector<D3D12_ROOT_PARAMETER>, kRootParameterIndexOfCount> rootParameters_;
	// ディスクリプタレンジPS
	static std::array<std::vector<D3D12_DESCRIPTOR_RANGE>, kDescriptorRangePSIndexOfCount> descriptorRangesPS_;
	// ディスクリプタレンジVS
	static std::array<std::vector<D3D12_DESCRIPTOR_RANGE>, kDescriptorRangeVSIndexOfCount> descriptorRangesVS_;

private: // シングルトン

	RootParameterManager() = default;
	~RootParameterManager() = default;
	RootParameterManager(const RootParameterManager&) = delete;
	RootParameterManager& operator=(const RootParameterManager&) = delete;

};

