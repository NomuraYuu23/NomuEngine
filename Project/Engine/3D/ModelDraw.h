#pragma once
#include "WorldTransform.h"
#include "../Camera/BaseCamera.h"
#include "Material.h"
#include "../Animation/LocalMatrixManager.h"
#include "Model.h"
#include "FogManager.h"
#include "Outline.h"

class ModelDraw
{

public: // サブクラス

	// パイプライン番号
	enum PipelineStateIndex {
		kPipelineStateIndexAnimModel, // アニメーションモデル
		kPipelineStateIndexNormalModel, // アニメーション無しモデル
		kPipelineStateIndexAnimInverseModel, // アニメーション反転モデル(右手座標系)
		kPipelineStateIndexManyAnimObjects, // 複数のアニメーションオブジェクト(アニメーションは同じ)
		kPipelineStateIndexManyNormalObjects, // 複数のアニメーション無しオブジェクト


		kPipelineStateIndexNormalOutline, // アニメーション無しアウトライン

		kPipelineStateIndexOfCount
	};
	
	// 描画前処理引数
	struct PreDrawDesc
	{
		ID3D12GraphicsCommandList* commandList; // コマンドリスト
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

	// アニメーション無しオブジェクト引数
	struct NormalObjectDesc
	{
		Model* model; //モデル
		WorldTransform* worldTransform; // ワールドトランスフォーム
		BaseCamera* camera; // カメラ
		Material* material; // マテリアル(なくてもいい)
		std::vector<UINT> textureHandles; // テクスチャハンドル(なくてもいい)
	};

	// 複数のアニメーションオブジェクト
	struct ManyAnimObjectsDesc
	{
		Model* model; //モデル
		D3D12_GPU_DESCRIPTOR_HANDLE* localMatrixesHandle;
		D3D12_GPU_DESCRIPTOR_HANDLE* transformationMatrixesHandle;
		BaseCamera* camera;
		uint32_t numInstance;
		D3D12_GPU_DESCRIPTOR_HANDLE* materialsHandle;
		std::vector<UINT> textureHandles;
	};

	// 複数のアニメーション無しオブジェクト
	struct ManyNormalObjectsDesc
	{
		Model* model; //モデル
		D3D12_GPU_DESCRIPTOR_HANDLE* transformationMatrixesHandle;
		BaseCamera* camera;
		uint32_t numInstance;
		D3D12_GPU_DESCRIPTOR_HANDLE* materialsHandle;
		std::vector<UINT> textureHandles;
	};

	struct NormalOutlineDesc 
	{
		Model* model; //モデル
		WorldTransform* worldTransform; // ワールドトランスフォーム
		Outline* outline; // アウトライン
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

	// 現在のパイプライン番号
	static PipelineStateIndex currentPipelineStateIndex_;

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
	/// アニメーション無しオブジェクト
	/// </summary>
	/// <param name="desc">アニメーション無しオブジェクト引数</param>
	static void NormalObjectDraw(NormalObjectDesc& desc);

	/// <summary>
	/// アニメーション反転オブジェクト
	/// </summary>
	/// <param name="desc">アニメーションオブジェクト引数</param>
	static void AnimInverseObjectDraw(AnimObjectDesc& desc);

	/// <summary>
	/// 複数のアニメーションオブジェクト
	/// </summary>
	/// <param name="desc">複数のアニメーションオブジェクト引数</param>
	static void ManyAnimObjectsDraw(ManyAnimObjectsDesc& desc);

	/// <summary>
	/// 複数のアニメーション無しオブジェクト
	/// </summary>
	/// <param name="desc">複数のアニメーション無しオブジェクト引数</param>
	static void ManyNormalObjectsDraw(ManyNormalObjectsDesc& desc);

	/// <summary>
	/// 
	/// </summary>
	static void NormalOutlineDraw(NormalOutlineDesc& desc);

};

