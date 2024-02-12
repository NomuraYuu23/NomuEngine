#pragma once
#include "../../base/DirectXCommon.h"
#include "../../Audio/Audio.h"// サウンド再生
#include "../../Input/Input.h"//入力デバイス
#include "../../Camera/DebugCamera.h"//デバッグカメラ

#include "../../3D/WorldTransform.h"
#include "../../Camera/BaseCamera.h"
#include "../../2D/Sprite.h"
#include "../../3D/Model.h"
#include "../../3D/Material.h"
#include "../../3D/DirectionalLight.h"
#include "../../Particle/ParticleManager.h"

#include "../../../Application/Scene/SceneName.h"
#include "../../base/ITextureHandleManager.h"

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

	//ビュープロジェクション
	static BaseCamera camera_;

	//デバッグカメラ
	static std::unique_ptr<DebugCamera> debugCamera_;
	static bool isDebugCameraActive_;

	//光源
	static std::unique_ptr<DirectionalLight> directionalLight_;

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
	/// マテリアルクリエイト
	/// </summary>
	virtual void MaterialCreate();

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

