#pragma once

#include "../../../Engine/Scene/IScene/IScene.h"
#include "../../UI/UIManager.h"
#include "../../AudioManager/GameAudioManager.h"
#include "../../Skydome/Skydome.h"
#include "../../ShadowManager/ShadowManager.h"

#include "../../Object/Sample/SampleObject.h" // サンプルオブジェクト
#include "../../../Engine/PostEffect/ShockWaveManager.h"

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
	std::unique_ptr<DirectionalLight> directionalLight_;
	Vector3 direction = { 1.0f, -1.0f, 0.0f};
	float intencity = 1.0f;

	std::unique_ptr<CollisionManager> collisionManager_;

	// UIマネージャー
	//std::unique_ptr<UIManager> uiManager_;
	//std:::array<uint32_t, UITextureHandleIndex::kUITextureHandleIndexOfCount> uiTextureHandles_;

	// オーディオマネージャー
	std::unique_ptr<GameAudioManager> audioManager_;
	bool isDecreasingVolume = true;

	// スカイドーム
	std::unique_ptr<Skydome> skydome_;
	std::unique_ptr<Model> skydomeModel_;

	//アウトライン仮
	OutLineData outline_;

	//影
	//std::unique_ptr<ShadowManager> shadowManager_;
	//std::unique_ptr<Model> shadowModel_;

	// サンプルOBJ
	std::unique_ptr<SampleObject> sampleObj_;
	std::unique_ptr<Model> sampleObjModel_;

	// 点光源
	std::unique_ptr<PointLightManager> pointLightManager_;
	std::array<PointLightData, PointLightManager::kNumInstanceMax_> pointLightDatas_;

	// スポットライト
	std::unique_ptr<SpotLightManager> spotLightManager_;
	std::array<SpotLightData, SpotLightManager::kNumInstanceMax_> spotLightDatas_;

	// Collision2DManager
	std::unique_ptr<Collision2DManager> collision2DManager_;
	// Collision2DDebugDraw
	std::unique_ptr<Collision2DDebugDraw> collision2DDebugDraw_;
	std::array<uint32_t, Collision2DDebugDraw::kTexutureNameOfCount> collision2DDebugDrawTextures_;

	std::unique_ptr<Box> box_;
	Vector2 boxCenter_;
	std::unique_ptr<Box> box1_;
	Vector2 box1Center_;
	std::unique_ptr<Circle> circle_;
	Vector2 circleCenter_;
	std::unique_ptr<Circle> circle1_;
	Vector2 circle1Center_;

	std::unique_ptr<Segment2D> segment1_;
	std::unique_ptr<Segment2D> segment2_;

	std::unique_ptr<DrawLine> line_;
	Vector3 linePos0_ = { 0.0f,0.0f,0.0f};
	Vector3 linePos1_ = { 10.0f,0.0f,0.0f };

	// テスト用
	std::unique_ptr<LargeNumberOfObjects> testManyObject_;
	std::unique_ptr<Model> testModel_;

	float time = 40.0f;

	// 衝撃波
	std::unique_ptr<ShockWaveManager> shockWaveManager_;

};
