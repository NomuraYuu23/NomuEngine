#include "TitleScene.h"
#include "../../../Engine/3D/ModelDraw.h"
#include "../../../Engine/base/OutputLog.h"

TitleScene::~TitleScene()
{
	if (stopAudio_) {
		for (uint32_t i = 0; i < audioManager_->kMaxPlayingSoundData; ++i) {
			audioManager_->StopWave(i);
		}
	}

}

void TitleScene::Initialize()
{

	IScene::Initialize();

	ModelCreate();
	TextureLoad();

	audioManager_ = std::make_unique<TitleAudioManager>();
	audioManager_->StaticInitialize();
	audioManager_->Initialize();

	// ビュープロジェクション
	EulerTransform baseCameraTransform = {
		1.0f, 1.0f, 1.0f,
		0.0f,0.0f,0.0f,
		0.0f, 0.0f, -35.0f };
	camera_.SetTransform(baseCameraTransform);

	// スカイドーム
	skydome_ = std::make_unique<Skydome>();
	skydome_->Initialize(skydomeModel_.get());

	IScene::InitilaizeCheck();

}

void TitleScene::Update()
{

#ifdef _DEBUG
	ImguiDraw();
#endif // _DEBUG

	if ((input_->TriggerJoystick(JoystickButton::kJoystickButtonA) || input_->TriggerKey(DIK_SPACE)) &&
		requestSceneNo_ == kTitle) {
		// 行きたいシーンへ
		requestSceneNo_ = kGame;
	}

	// BGM音量下げる
	if (requestSceneNo_ == kTutorial && isDecreasingVolume) {
		LowerVolumeBGM();
	}
	
	// カメラ
	camera_.Update();
	
	// スカイドーム
	skydome_->Update();

}

void TitleScene::Draw()
{

#pragma region 背景スプライト描画
	// 背景スプライト描画前処理
	Sprite::PreDraw(dxCommon_->GetCommadList());

	// スプライト描画後処理
	Sprite::PostDraw();
	// 深度バッファクリア
	renderTargetTexture_->ClearDepthBuffer();


#pragma endregion

	ModelDraw::PreDrawDesc preDrawDesc;
	preDrawDesc.commandList = dxCommon_->GetCommadList();
	preDrawDesc.directionalLight = directionalLight_.get();
	preDrawDesc.fogManager = FogManager::GetInstance();
	preDrawDesc.pointLightManager = pointLightManager_.get();
	preDrawDesc.spotLightManager = spotLightManager_.get();

	ModelDraw::PreDraw(preDrawDesc);

	// スカイドーム
	skydome_->Draw(camera_);

	ModelDraw::PostDraw();

#pragma region 前景スプライト描画
	// 前景スプライト描画前処理
	Sprite::PreDraw(dxCommon_->GetCommadList());

	//背景
	//前景スプライト描画

	// 前景スプライト描画後処理
	Sprite::PostDraw();

#pragma endregion

}

void TitleScene::ImguiDraw()
{

#ifdef _DEBUG
	ImGui::Begin("Title");
	ImGui::Checkbox("isDrawSkydome", &isDrawSkydome_);
	ImGui::End();
#endif // _DEBUG

}

void TitleScene::ModelCreate()
{

	// スカイドーム
	skydomeModel_.reset(Model::Create("Resources/Model/Skydome/", "skydome.obj", dxCommon_, textureHandleManager_.get()));

}

void TitleScene::TextureLoad()
{

}

void TitleScene::LowerVolumeBGM()
{

	//for (uint32_t i = 0; i < audioManager_->kMaxPlayingSoundData; ++i) {
	//	if (audioManager_->GetPlayingSoundDatas()[i].handle_ == kTitleAudioNameIndexBGM) {
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
