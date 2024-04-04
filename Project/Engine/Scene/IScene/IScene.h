#pragma once

// 基盤
#include "../../base/DirectXCommon.h" // DirectX関連
#include "../../Audio/Audio.h"// サウンド再生
#include "../../Input/Input.h"//入力デバイス
#include "../../base/TextureManager.h" // テクスチャマネージャー
#include "../../base/ITextureHandleManager.h" // テクスチャハンドルマネージャー
#include "../../base/D3DResourceLeakChecker.h" // リソース解放確認
#include "../../2D/ImguiManager.h" // imGuiマネージャー
#include "../../GlobalVariables/GlobalVariables.h" // グローバル変数

// カメラ
#include "../../Camera/BaseCamera.h" // ベースカメラ
#include "../../Camera/DebugCamera.h"//デバッグカメラ
#include "../../Camera/DebugCamera.h"//デバッグカメラ

// 描画系
#include "../../2D/Sprite.h" // スプライト
#include "../../3D/Model.h" // モデル
#include "../../3D/LargeNumberOfObjects.h" // 大量のオブジェクト用モデル
#include "../../2D/DrawLine.h" // 線描画
#include "../../3D/Material.h"// マテリアル
#include "../../Particle/ParticleManager.h" // パーティクルマネージャー
#include "../../PostEffect/PostEffect.h" // ポストエフェクト

// 数学系
#include "../../Math/DeltaTime.h" // デルタタイム
#include "../../Math/Ease.h" // 補間

// 衝突系
#include "../../Collision/CollisionManager.h" // 3D衝突マネージャー
#include "../../Collider/ColliderDebugDraw/ColliderDebugDraw.h" // 3Dコライダーデバッグ描画
#include "../../../Engine/Collision2D/Collision2DManager.h" // 2D衝突マネージャー
#include "../../../Engine/Collision2D/Collision2DDebugDraw.h" // 2Dコライダーデバッグ描画

// 光源系
#include "../../../Engine/Light/DirectionalLight/DirectionalLight.h" // 平行光源
#include "../../../Engine/Light/PointLight/PointLightManager.h" // 点光源
#include "../../../Engine/Light/SpotLight/SpotLightManager.h" // スポット

// アプリケーション側
#include "../../../Application/Scene/SceneName.h" // シーンの名前

/// <summary>
/// シーンの元になるクラス
/// </summary>
class IScene
{

protected: // 静的メンバ変数

	// シーン番号
	static int sceneNo;
	static int requestSceneNo;

	// 入力マネージャー
	static DirectXCommon* dxCommon_;
	static Input* input_;
	static Audio* audio_;
	static RenderTargetTexture* renderTargetTexture_;

	//ビュープロジェクション
	static BaseCamera camera_;

	//デバッグカメラ
	static std::unique_ptr<DebugCamera> debugCamera_;
	static bool isDebugCameraActive_;

public: // メンバ関数

	/// <summary>
	/// 静的初期化
	/// </summary>
	void StaticInitialize();

	/// <summary>
	/// 初期化
	/// </summary>
	virtual void Initialize();

	/// <summary>
	/// 更新
	/// </summary>
	virtual void Update() = 0;

	/// <summary>
	/// 描画
	/// </summary>
	virtual void Draw() = 0;

	//デストラクタ
	virtual ~IScene();

	// シーン番号のゲッター
	int GetSceneNo();

	// リクエストシーン番号のゲッター
	int GetRequestSceneNo();

	// シーンをリセットするか
	bool GetResetScene() { return resetScene_; }

	// オーディオを止める
	void SetStopAudio(bool stopAudio) { stopAudio_ = stopAudio; }

protected:  // メンバ関数

	/// <summary>
	/// モデルクリエイト
	/// </summary>
	virtual void ModelCreate();

	/// <summary>
	/// テクスチャロード
	/// </summary>
	virtual void TextureLoad();

protected: // メンバ変数

	// テクスチャハンドル管理
	std::unique_ptr<ITextureHandleManager> textureHandleManager_ = nullptr;

	// シーンをリセット
	bool resetScene_ = false;

	// リセット中
	bool isBeingReset_ = false;

	// 削除された時オーディオ止める
	bool stopAudio_ = false;

};

