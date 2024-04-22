#include "TutorialScene.h"
#include "../../Particle/EmitterName.h"

TutorialScene::~TutorialScene()
{

	if (stopAudio_) {
		for (uint32_t i = 0; i < audioManager_->kMaxPlayingSoundData; ++i) {
			audioManager_->StopWave(i);
		}
	}

	particleManager_->Finalize();

}

/// <summary>
/// 初期化
/// </summary>
void TutorialScene::Initialize() {

	IScene::Initialize();

	ModelCreate();
	TextureLoad();

	// ビュープロジェクション
	EulerTransform baseCameraTransform = {
		1.0f, 1.0f, 1.0f,
		0.58f,0.0f,0.0f,
		0.0f, 23.0f, -35.0f };
	camera_.SetTransform(baseCameraTransform);

	//パーティクル
	particleManager_ = ParticleManager::GetInstance();
	std::array<Model*, ParticleModelIndex::kCountofParticleModelIndex> particleModel;
	particleModel[ParticleModelIndex::kUvChecker] = particleUvcheckerModel_.get();
	particleModel[ParticleModelIndex::kCircle] = particleCircleModel_.get();
	particleManager_->ModelCreate(particleModel);

	isDebugCameraActive_ = false;

	collisionManager_.reset(new CollisionManager);
	collisionManager_->Initialize();

	//UIマネージャー
	//uiManager_ = std::make_unique<UIManager>();
	//uiManager_->Initialize(uiTextureHandles_);

	// オーディオマネージャー
	audioManager_ = std::make_unique<GameAudioManager>();
	audioManager_->Initialize();

	// スカイドーム
	skydome_ = std::make_unique<Skydome>();
	skydome_->Initialize(skydomeModel_.get());

}

/// <summary>
/// 更新処理
/// </summary>
void TutorialScene::Update() {

#ifdef _DEBUG
	ImguiDraw();
#endif

	if (requestSceneNo == kClear || requestSceneNo == kTitle || isBeingReset_) {
		resetScene_ = false;
		// BGM音量下げる
		if (isDecreasingVolume) {
			LowerVolumeBGM();
		}
		return;
	}

	// リスタート
	//if () {
	//	resetScene_ = true;
	//	isBeingReset_ = true;
	//	isDecreasingVolume = true;
	//}

	//光源
	//DirectionalLightData directionalLightData;
	//directionalLightData.color = { 1.0f,1.0f,1.0f,1.0f };
	//directionalLightData.direction = Vector3Calc::Normalize(direction);
	//directionalLightData.intencity = intencity;
	//directionalLight_->Update(directionalLightData);

	// あたり判定
	collisionManager_->ListClear();
	//collisionManager_->ListRegister();
	collisionManager_->CheakAllCollision();

	// スカイドーム
	skydome_->Update();

	//uiManager_->Update();

	// デバッグカメラ
	DebugCameraUpdate();

	//パーティクル
	particleManager_->Update(camera_);

}

/// <summary>
/// 描画処理
/// </summary>
void TutorialScene::Draw() {

}

void TutorialScene::ImguiDraw() {
#ifdef _DEBUG

	ImGui::Begin("Light");
	ImGui::DragFloat3("direction", &direction.x, 0.1f);
	ImGui::DragFloat("i", &intencity, 0.01f);
	ImGui::Text("Frame rate: %6.2f fps", ImGui::GetIO().Framerate);
	ImGui::End();

	// スカイドーム
	skydome_->ImGuiDraw();

	// 惑星
	//planet_->ImGuiDraw();

	debugCamera_->ImGuiDraw();

#endif // _DEBUG

}

void TutorialScene::DebugCameraUpdate()
{

#ifdef _DEBUG
	if (input_->TriggerKey(DIK_RETURN)) {
		if (isDebugCameraActive_) {
			isDebugCameraActive_ = false;
		}
		else {
			isDebugCameraActive_ = true;
		}
	}

	// カメラの処理
	if (isDebugCameraActive_) {
		// デバッグカメラの更新
		debugCamera_->Update();
		// デバッグカメラのビュー行列をコピー
		camera_ = static_cast<BaseCamera>(*debugCamera_.get());
		// ビュー行列の転送
		camera_.Update();
	}
#endif

}

void TutorialScene::ModelCreate()
{

	// パーティクル
	particleUvcheckerModel_.reset(Model::Create("Resources/default/", "plane.obj", dxCommon_, textureHandleManager_.get()));
	particleCircleModel_.reset(Model::Create("Resources/Particle/", "plane.obj", dxCommon_, textureHandleManager_.get()));

	// スカイドーム
	skydomeModel_.reset(Model::Create("Resources/Model/Skydome/", "skydome.obj", dxCommon_, textureHandleManager_.get()));
}

void TutorialScene::TextureLoad()
{

	/*
	uiTextureHandles_ = {

	};
	*/

}

void TutorialScene::LowerVolumeBGM()
{


	const uint32_t startHandleIndex = 3;

	//for (uint32_t i = 0; i < audioManager_->kMaxPlayingSoundData; ++i) {
	//	if (audioManager_->GetPlayingSoundDatas()[i].handle_ == kGameAudioNameIndexBGM + startHandleIndex) {
	//		float decreasingVolume = 1.0f / 60.0f;
	//		float volume = audioManager_->GetPlayingSoundDatas()[i].volume_ - decreasingVolume;
	//		if (volume < 0.0f) {
	//			volume = 0.0f;
	//			audioManager_->StopWave(i);
	//			isDecreasingVolume = false;
	//		}
	//		else {
	//			audioManager_->SetPlayingSoundDataVolume(i, volume);
	//			audioManager_->SetVolume(i, audioManager_->GetPlayingSoundDatas()[i].volume_);
	//		}
	//	}
	//}

}
