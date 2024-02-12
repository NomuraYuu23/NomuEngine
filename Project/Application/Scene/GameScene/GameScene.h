#pragma once

#include "../../../Engine/Scene/IScene/IScene.h"
#include "../../../Engine/Collider/ColliderDebugDraw/ColliderDebugDraw.h"// コライダーデバッグ描画
#include"../../../Engine/Collision/CollisionManager.h"
#include "../../../Engine/base/ITextureHandleManager.h"
#include "../../../Engine/Collision/CollisionManager.h"
#include "../../UI/UIManager.h"
#include "../../AudioManager/GameAudioManager.h"
#include "../../Skydome/Skydome.h"
#include "../../../Engine/3D/OutLineData.h"
#include "../../ShadowManager/ShadowManager.h"


class GameScene : public IScene
{

public:

	~GameScene();

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 更新処理
	/// </summary>
	void Update();

	/// <summary>
	/// 描画処理
	/// </summary>
	void Draw();

	/// <summary>
	/// imgui描画処理
	/// </summary>
	void ImguiDraw();

public: //メンバ関数

	/// <summary>
	/// デバッグカメラ更新
	/// </summary>
	void DebugCameraUpdate();

private: // メンバ関数

	/// <summary>
	/// モデルクリエイト
	/// </summary>
	void ModelCreate() override;

	/// <summary>
	/// テクスチャロード
	/// </summary>
	void TextureLoad() override;

private:
	  
	/// <summary>
	/// 音量
	/// </summary>
	void LowerVolumeBGM();

	/// <summary>
	/// 影更新
	/// </summary>
	void ShadowUpdate();

private:

	// パーティクルマネージャー
	ParticleManager* particleManager_ = nullptr;
	std::unique_ptr<Model> particleUvcheckerModel_ = nullptr;
	std::unique_ptr<Model> particleCircleModel_ = nullptr;

	// ライト
	Vector3 direction = { 1.0f, -1.0f, 0.0f};
	float intencity = 1.0f;

	std::unique_ptr<CollisionManager> collisionManager_;

	// UIマネージャー
	//std::unique_ptr<UIManager> uiManager_;
	//::array<uint32_t, UITextureHandleIndex::kUITextureHandleIndexOfCount> uiTextureHandles_;

	// オーディオマネージャー
	std::unique_ptr<GameAudioManager> audioManager_;
	bool isDecreasingVolume = true;

	// スカイドーム
	std::unique_ptr<Skydome> skydome_;
	std::unique_ptr<Model> skydomeModel_;

	//アウトライン仮
	OutLineData outline_;;

	//影
	std::unique_ptr<ShadowManager> shadowManager_;
	std::unique_ptr<Model> shadowModel_;

};
