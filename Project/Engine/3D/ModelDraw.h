#pragma once
#include "WorldTransform.h"
#include "../Camera/BaseCamera.h"
#include "Material.h"
#include "../Animation/LocalMatrixManager.h"
#include "Model.h"
#include "FogManager.h"

class ModelDraw
{

public: // サブクラス

	// パイプライン番号
	enum PipelineStateIndex {
		kPipelineStateIndexAnimObject, // アニメーションオブジェクト
		//kPipelineStateNameParticle,
		//kPipelineStateNameManyModels,
		kPipelineStateIndexOfCount
	};
	
	// 描画前処理引数
	struct PreDrawDesc
	{
		ID3D12GraphicsCommandList* commandList; // コマンドリスト
		PipelineStateIndex pipelineStateIndex; // パイプライン番号
		DirectionalLight* directionalLight; // 平行光源
		PointLightManager* pointLightManager; // ポイントライト
		SpotLightManager* spotLightManager; // スポットライト
		FogManager* fogManager; // 霧マネージャー
	};

	struct AnimObjectDesc
	{
		Model* model; //モデル
		WorldTransform* worldTransform; // ワールドトランスフォーム
		LocalMatrixManager* localMatrixManager;// ローカル行列マネージャー
		BaseCamera* camera; // カメラ
		Material* material; // マテリアル(なくてもいい)
		std::vector<UINT> textureHandles; // テクスチャハンドル(なくてもいい)
	};

public:

	// コマンドリスト
	static ID3D12GraphicsCommandList* sCommandList;
	// ルートシグネチャ
	static ID3D12RootSignature* sRootSignature[PipelineStateIndex::kPipelineStateIndexOfCount];
	// パイプラインステートオブジェクト
	static ID3D12PipelineState* sPipelineState[PipelineStateIndex::kPipelineStateIndexOfCount];
	//	平行光源
	static DirectionalLight* sDirectionalLight_;
	// ポイントライトマネージャ
	static PointLightManager* sPointLightManager_;
	//	スポットライトマネージャ
	static SpotLightManager* sSpotLightManager_;
	// 霧マネージャー
	static FogManager* sFogManager_;

public: //関数（描画以外）

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="rootSignature">ルートシグネチャ</param>
	/// <param name="pipelineState">パイプラインステート</param>
	static void Initialize(
		const std::array<ID3D12RootSignature*, PipelineStateIndex::kPipelineStateIndexOfCount>& rootSignature,
		const std::array<ID3D12PipelineState*, PipelineStateIndex::kPipelineStateIndexOfCount>& pipelineState);

	/// <summary>
	/// 描画前処理
	/// </summary>
	/// <param name="desc">描画前処理引数</param>
	static void PreDraw(const PreDrawDesc& desc);

	/// <summary>
	/// 描画後処理
	/// </summary>
	static void PostDraw();

public: // 描画

	static void AnimObjectDraw(AnimObjectDesc& desc);

};

