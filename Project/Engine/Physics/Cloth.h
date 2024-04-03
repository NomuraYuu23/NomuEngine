#pragma once
#include "../Math/Vector3.h"
#include "../Math/Vector2.h"
#include "../Math/Vector4.h"

#include <d3d12.h>
#include <string>
#include <wrl.h>
#include <dxcapi.h>
#include "../Camera/BaseCamera.h"
#include "../3D/Material.h"

class Cloth
{

public: // サブクラス

	/// <summary>
	/// 布の質点
	/// </summary>
	struct ClothPoint
	{
		Vector3 currentPos; // 現在の位置
		Vector3 previousPos; // 前の位置
		float weight; // 重み

	};
	
	/// <summary>
	/// 制約(バネ)の種類
	/// </summary>
	enum ClothConstraintIndex {
		kClothConstraintIndexStructural, // 構成バネ
		kClothConstraintIndexShear, // せん断バネ
		kClothConstraintIndexBending, // 曲げバネ
		kClothConstraintIndexOfCount, // 数を数える用
	};


	/// <summary>
	/// 制約(バネ)
	/// </summary>
	struct ClothConstraint
	{
		ClothPoint* p1; // 質点1
		ClothPoint* p2; // 質点2
		float naturalLength; // 自然長
		ClothConstraintIndex type; // バネの種類
	};

	/// <summary>
	/// 頂点データ
	/// </summary>
	struct VertexData
	{
		Vector4 position; // 位置
		Vector2 texcoord; // テクスチャ座標
		Vector3 normal; // 法線
	};

	/// <summary>
	/// 更新処理引数
	/// </summary>
	struct ClothUpdateDesc 
	{
		float step; // 進んだ時間
		float acc; // 加速度
		uint32_t constraintSatisfactionPhaseNum; // 制約充足フェーズ回数 
		float gravity; // 重力
		float wind; // 風力
		float resistance; // 抵抗
		float basicStrength; //制約バネの基本強度
		float structuralShrink; // 構成バネの縮み抵抗
		float structuralStretch; // 構成バネの伸び抵抗
		float shearShrink; // せん断バネの縮み抵抗
		float shearStretch; // せん断バネの伸び抵抗
		float bendingShrink; // 曲げバネの縮み抵抗
		float bendingStretch; // 曲げバネの伸び抵抗
		bool isCollision; // 衝突したか
	};

	/// <summary>
	/// 制約充足フェーズ引数
	/// </summary>
	struct ConstraintSatisfactionPhaseDesc {
		float step; // 進んだ時間
		float basicStrength; //制約バネの基本強度
		float structuralShrink; // 構成バネの縮み抵抗
		float structuralStretch; // 構成バネの伸び抵抗
		float shearShrink; // せん断バネの縮み抵抗
		float shearStretch; // せん断バネの伸び抵抗
		float bendingShrink; // 曲げバネの縮み抵抗
		float bendingStretch; // 曲げバネの伸び抵抗
	};

public: // 静的変数

	// デバイス
	static ID3D12Device* sDevice;
	// コマンドリスト
	static ID3D12GraphicsCommandList* sCommandList;
	// ルートシグネチャ
	static ID3D12RootSignature* sRootSignature;
	// パイプラインステートオブジェクト
	static ID3D12PipelineState* sPipelineState;

public: // 静的関数

	static void StaticInitialize(
		ID3D12Device* device,
		ID3D12RootSignature* rootSignature,
		ID3D12PipelineState* pipelineState);

	/// <summary>
	/// 静的前処理
	/// </summary>
	/// <param name="cmdList">描画コマンドリスト</param>
	static void PreManyModelsDraw(ID3D12GraphicsCommandList* cmdList);


	/// <summary>
	/// 描画後処理
	/// </summary>
	static void PostDraw();

public: // 関数

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="scale">大きさ</param>
	/// <param name="div">質点分割数</param>
	/// <param name="textureHandle">テクスチャハンドル</param>
	void Initialize(
		float scale,
		uint32_t div,
		uint32_t textureHandle);

	/// <summary>
	/// 更新処理
	/// </summary>
	/// <param name="desc">引数構造体</param>
	void Update(const ClothUpdateDesc& desc);

	/// <summary>
	/// 描画
	/// </summary>
	/// <param name="camera">カメラ</param>
	void Draw(BaseCamera& camera);

private: // 関数

	/// <summary>
	/// 制約生成
	/// </summary>
	/// <param name="x">x</param>
	/// <param name="y">y</param>
	/// <param name="offsetX">オフセットx</param>
	/// <param name="offsetY">オフセットy</param>
	/// <param name="type">タイプ</param>
	/// <returns>制約(バネ)</returns>
	ClothConstraint ClothConstraintGenerate(
		uint32_t x,
		uint32_t y,
		int32_t offsetX,
		int32_t offsetY,
		ClothConstraintIndex type);

	/// <summary>
	/// 頂点データ初期化
	/// </summary>
	void VertexesInitialize();

	/// <summary>
	/// 頂点データ更新
	/// </summary>
	void VertexesUpdate();

	/// <summary>
	/// 積分フェーズ
	/// </summary>
	/// <param name="gravity">重力</param>
	/// <param name="wind">風力</param>
	/// <param name="acc">加速</param>
	/// <param name="step">ステップ(デルタタイム)</param>
	/// <param name="resistance">抵抗</param>
	void IntegralPhase(
		float gravity,
		float wind,
		float acc,
		float step,
		float resistance);

	/// <summary>
	/// 制約充足フェーズ
	/// </summary>
	/// <param name="desc">引数構造体</param>
	void ConstraintSatisfactionPhase(
		const ConstraintSatisfactionPhaseDesc& desc);

private: // 変数

	// スケール
	float scale_;

	// 質点分割数
	uint32_t div_;

	// 質点
	std::vector<ClothPoint> points_;

	// 制約(バネ)
	std::vector<ClothConstraint> clothConstraints_;

	// 質点の質量
	float pointMass_;

	// 頂点バッファ
	Microsoft::WRL::ComPtr<ID3D12Resource> vertBuff_;
	// 頂点バッファマップ
	VertexData* vertMap_ = nullptr;
	// 頂点バッファビュー
	D3D12_VERTEX_BUFFER_VIEW vbView_{};

	// マテリアル
	std::unique_ptr<Material> material_;

	// テクスチャハンドル
	uint32_t textureHandle_;

};

