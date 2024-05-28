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
		kPipelineStateIndexModel, // モデル
		kPipelineStateIndexAnimModel, // アニメーションモデル
		kPipelineStateIndexAnimInverseModel, // 反転モデル(右手座標系)
		kPipelineStateIndexManyObjects, // 複数のアニメーション無しオブジェクト
		kPipelineStateIndexAnimManyObjects, // 複数のアニメーションありオブジェクト

		kPipelineStateIndexAnimModelRT2, // アニメーションモデル
		kPipelineStateIndexAnimInverseModelRT2, // 反転モデル(右手座標系)

		kPipelineStateIndexNormalOutline, // アニメーション無しアウトライン

		kPipelineStateIndexOfCount
	};
	
	// 描画前処理引数
	struct PreDrawDesc
	{
		ID3D12GraphicsCommandList* commandList = nullptr; // コマンドリスト
		DirectionalLight* directionalLight = nullptr; // 平行光源
		PointLightManager* pointLightManager = nullptr; // ポイントライト
		SpotLightManager* spotLightManager = nullptr; // スポットライト
		FogManager* fogManager = nullptr; // 霧マネージャー
		uint32_t environmentTextureHandle = 1024; // 環境マップ(映り込み用テクスチャ)ハンドル
	};

	// アニメーションオブジェクト引数
	struct AnimObjectDesc
	{
		Model* model = nullptr; //モデル
		WorldTransform* worldTransform = nullptr; // ワールドトランスフォーム
		LocalMatrixManager* localMatrixManager = nullptr;// ローカル行列マネージャー
		BaseCamera* camera = nullptr; // カメラ
		Material* material = nullptr; // マテリアル(なくてもいい)
		std::vector<UINT> textureHandles; // テクスチャハンドル(なくてもいい)
	};

	// アニメーション無しオブジェクト引数
	struct NormalObjectDesc
	{
		Model* model = nullptr; //モデル
		WorldTransform* worldTransform = nullptr; // ワールドトランスフォーム
		BaseCamera* camera = nullptr; // カメラ
		Material* material = nullptr; // マテリアル(なくてもいい)
		std::vector<UINT> textureHandles; // テクスチャハンドル(なくてもいい)
	};

	// 複数のアニメーションオブジェクト
	struct ManyAnimObjectsDesc
	{
		Model* model = nullptr; //モデル
		LocalMatrixManager* localMatrixManager = nullptr;// ローカル行列マネージャー
		D3D12_GPU_DESCRIPTOR_HANDLE* transformationMatrixesHandle = nullptr;
		BaseCamera* camera = nullptr;
		uint32_t numInstance = 0;
		D3D12_GPU_DESCRIPTOR_HANDLE* materialsHandle = nullptr;
		std::vector<UINT> textureHandles;
	};

	// 複数のアニメーション無しオブジェクト
	struct ManyNormalObjectsDesc
	{
		Model* model = nullptr; //モデル
		D3D12_GPU_DESCRIPTOR_HANDLE* transformationMatrixesHandle = nullptr;
		BaseCamera* camera = nullptr;
		uint32_t numInstance = 0;
		D3D12_GPU_DESCRIPTOR_HANDLE* materialsHandle = nullptr;
		std::vector<UINT> textureHandles;
	};

	struct NormalOutlineDesc 
	{
		Model* model = nullptr; //モデル
		WorldTransform* worldTransform = nullptr; // ワールドトランスフォーム
		Outline* outline = nullptr; // アウトライン
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
	// 環境マップ(映り込み用テクスチャ)ハンドル
	static uint32_t sEnvironmentTextureHandle_;

	// 現在のパイプライン番号
	static PipelineStateIndex currentPipelineStateIndex_;

	// ルートシグネチャCS
	static Microsoft::WRL::ComPtr<ID3D12RootSignature> sRootSignatureCS_;
	// パイプラインステートオブジェクトCS
	static Microsoft::WRL::ComPtr<ID3D12PipelineState> sPipelineStateCS_;

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
	static void AnimObjectDraw(AnimObjectDesc& desc, uint32_t renderTargetIndex = 0);

	/// <summary>
	/// アニメーション無しオブジェクト
	/// </summary>
	/// <param name="desc">アニメーション無しオブジェクト引数</param>
	static void NormalObjectDraw(NormalObjectDesc& desc);

	/// <summary>
	/// アニメーション反転オブジェクト
	/// </summary>
	/// <param name="desc">アニメーションオブジェクト引数</param>
	static void AnimInverseObjectDraw(AnimObjectDesc& desc, uint32_t renderTargetIndex = 0);

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

private: // 関数

	static void UpdateVertexUAV(
		Model* model,
		LocalMatrixManager* localMatrixManager);

};

