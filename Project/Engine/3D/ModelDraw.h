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
		kPipelineStateIndexParticle, // パーティクル
		kPipelineStateIndexManyAnimObjects, // 複数のアニメーションオブジェクト
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

	// アニメーションオブジェクト引数
	struct AnimObjectDesc
	{
		Model* model; //モデル
		WorldTransform* worldTransform; // ワールドトランスフォーム
		LocalMatrixManager* localMatrixManager;// ローカル行列マネージャー
		BaseCamera* camera; // カメラ
		Material* material; // マテリアル(なくてもいい)
		std::vector<UINT> textureHandles; // テクスチャハンドル(なくてもいい)
	};

	// パーティクル引数
	struct ParticleDesc 
	{
		Model* model; //モデル
		ParticleManager* particleManager; // パーティクルマネージャー
	};

	// 複数のアニメーションオブジェクト
	struct ManyAnimObjectsDesc
	{
		Model* model; //モデル
		D3D12_GPU_DESCRIPTOR_HANDLE* localMatrixesHandle;
		D3D12_GPU_DESCRIPTOR_HANDLE* transformationMatrixesHandle;
		BaseCamera* camera;
		uint32_t numInstance;
		Material* material;
		std::vector<UINT> textureHandles;
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

	/// <summary>
	/// アニメーションオブジェクト
	/// </summary>
	/// <param name="desc">アニメーションオブジェクト引数</param>
	static void AnimObjectDraw(AnimObjectDesc& desc);

	/// <summary>
	/// パーティクル
	/// </summary>
	/// <param name="desc">パーティクル引数</param>
	static void ParticleDraw(ParticleDesc& desc);

	/// <summary>
	/// 複数のアニメーションオブジェクト
	/// </summary>
	/// <param name="desc">複数のアニメーションオブジェクト引数</param>
	static void ManyAnimObjectsDraw(ManyAnimObjectsDesc& desc);

};

