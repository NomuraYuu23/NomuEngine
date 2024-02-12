#pragma once

#include <DirectXMath.h>
#include <Windows.h>
#include <d3d12.h>
#include <string>
#include <wrl.h>
#include <dxcapi.h>
#include <list>
#include <memory>

#pragma comment(lib, "dxcompiler.lib")

#include "../Math/Vector2.h"
#include "../Math/Vector3.h"
#include "../Math/Vector4.h"
#include "../Math/Matrix4x4.h"

#include "../3D/VertexData.h"
#include "../3D/TransformationMatrix.h"
#include "../3D/TransformStructure.h"

#include "../3D/Material.h"
#include "../3D/WorldTransform.h"

/// <summary>
/// スプライト
/// </summary>
class Sprite
{

public:

	/// <summary>
	/// 静的初期化
	/// </summary>
	/// <param name="device">デバイス</param>
	static void StaticInitialize(
		ID3D12Device* device,
		Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature,
		Microsoft::WRL::ComPtr<ID3D12PipelineState> pipelineState);

	/// <summary>
	/// 静的前処理
	/// </summary>
	/// <param name="cmdList">描画コマンドリスト</param>
	static void PreDraw(ID3D12GraphicsCommandList* cmdList);

	/// <summary>
	/// 描画後処理
	/// </summary>
	static void PostDraw();

	/// <summary>
	/// スプライト生成
	/// </summary>
	/// <param name="textureHandle">テクスチャハンドル</param>

	/// <returns>生成されたスプライト</returns>
	static Sprite* Create(
		uint32_t textureHandle, const Vector2& position, const Vector4& color);

private:

	// 頂点数
	static const int kVertNum = 6;
	// デバイス
	static ID3D12Device* sDevice;
	// ディスクリプタサイズ
	static UINT sDescriptorHandleIncrementSize;
	// コマンドリスト
	static ID3D12GraphicsCommandList* sCommandList;
	// ルートシグネチャ
	static Microsoft::WRL::ComPtr<ID3D12RootSignature> sRootSignature;
	// パイプラインステートオブジェクト
	static Microsoft::WRL::ComPtr<ID3D12PipelineState> sPipelineState;
	//計算
	static Matrix4x4Calc* matrix4x4Calc;

public:
	/// <summary>
	/// コンストラクタ
	/// </summary>
	Sprite();
	/// <summary>
	/// コンストラクタs
	/// </summary>
	Sprite(uint32_t textureHandle, const Vector2& position, const Vector2& size, const Vector4& color);

	/// <summary>
	/// 初期化
	/// </summary>
	/// <returns>成否</returns>
	bool Initialize();

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// テクスチャハンドルの設定
	/// </summary>
	/// <param name="textureHandle"></param>
	void SetTextureHandle(uint32_t textureHandle);

	uint32_t GetTevtureHandle() { return textureHandle_; }

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

	/// <summary>
	/// テクスチャの範囲
	/// </summary>
	void SetTextureRange();

private:
	// 頂点バッファ
	Microsoft::WRL::ComPtr<ID3D12Resource> vertBuff_;
	// 頂点バッファマップ
	VertexData* vertMap = nullptr;
	// 頂点バッファビュー
	D3D12_VERTEX_BUFFER_VIEW vbView_{};

	Microsoft::WRL::ComPtr<ID3D12Resource> indexBuff_;

	//インデックスバッファビュー
	D3D12_INDEX_BUFFER_VIEW ibView_{};

	//インデックスリソースにデータを書き込む
	uint32_t* indexMap = nullptr;

	//テクスチャ番号
	UINT textureHandle_ = 0;
	// リソース設定
	D3D12_RESOURCE_DESC resourceDesc_;

public: // アクセッサ(ワールドトランスフォーム)

	void SetPosition(const Vector2& position);

	void SetRotate(float rotate);

	void SetSize(const Vector2& size);

	void SetAnchorPoint(const Vector2& anchorPoint);

	void SetIsFlipX(bool isFlipX);

	void SetIsFlipY(bool isFlipY);
	
	void SetIsInvisible(bool isInvisible) { isInvisible_ = isInvisible; }
	
	void SetTextureSize(const Vector2& textureSize);

	void SetTextureLeftTop(const Vector2& textureLeftTop);

	// 位置
	Vector2 GetPosition() { return position_; }
	// 回転
	float GetRotate() { return rotate_; }
	// サイズ
	Vector2 GetSize() { return size_; }

	// アンカーポイント
	Vector2 GetAnchorPoint() { return anchorPoint_; }

	// フリップ
	bool GetIsFlipX() { return isFlipX_; }
	bool GetIsFlipY() { return isFlipY_; }

	// テクスチャのサイズ
	Vector2 GetTextureSize() { return textureSize_; }
	// 描画する
	Vector2 GetTextureLeftTop() { return textureLeftTop_; }
	// テクスチャ初期サイズ
	Vector2 GetTextureInitSize() { return textureInitSize_; }

	bool GetIsInvisible() { return isInvisible_; }

private: // メンバ変数(ワールドトランスフォーム)

	// 位置
	Vector2 position_;
	// 回転
	float rotate_;
	// サイズ
	Vector2 size_;
	// ワールドトランスフォーム
	WorldTransform worldTransform_;

	// アンカーポイント
	Vector2 anchorPoint_;

	// フリップ
	bool isFlipX_ = false;
	bool isFlipY_ = false;

	// 非表示フラグ
	bool isInvisible_ = false;

	// テクスチャのサイズ
	Vector2 textureSize_;
	// 描画する
	Vector2 textureLeftTop_;
	// テクスチャ初期サイズ
	Vector2 textureInitSize_;

public: // アクセッサ(マテリアル)

	void SetUvTransform(const TransformStructure& uvTransform);
	void SetUvTransform(const Vector3& scale, const Vector3& rotate, const Vector3& translate);

	void SetColor(const Vector4& color);

	void SetEnableLighting(int enableLighting);

private: // メンバ変数(マテリアル)

	// マテリアル
	std::unique_ptr<Material> material_;

	TransformStructure uvTransform_;
	Vector4 color_;
	int enableLighting_;

};

