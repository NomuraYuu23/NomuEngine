#include "ClearScene.h"

ClearScene::~ClearScene()
{

	if (stopAudio_) {
		for (uint32_t i = 0; i < audioManager_->kMaxPlayingSoundData; ++i) {
			audioManager_->StopWave(i);
		}
	}

}

void ClearScene::Initialize()
{

	IScene::Initialize();

	ModelCreate();
	TextureLoad();

	audioManager_ = std::make_unique<ClearAudioManager>();
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

void ClearScene::Update()
{

#ifdef _DEBUG

	ImguiDraw();

#endif // _DEBUG

	if ((input_->TriggerJoystick(JoystickButton::kJoystickButtonA)) &&
		requestSceneNo_ == kClear) {
		// 行きたいシーンへ
		requestSceneNo_ = kTitle;
	}

	// BGM音量下げる
	if (requestSceneNo_ == kTitle && isDecreasingVolume) {
		LowerVolumeBGM();
	}

	// カメラ
	camera_.Update();

	// スカイドーム
	skydome_->Update();
	
}

void ClearScene::Draw()
{


}

void ClearScene::ImguiDraw()
{

#ifdef _DEBUG
#endif // _DEBUG

}

void ClearScene::ModelCreate()
{

	// スカイドーム
	skydomeModel_.reset(Model::Create("Resources/Model/Skydome/", "skydome.obj", dxCommon_));

}

void ClearScene::TextureLoad()
{

}

void ClearScene::LowerVolumeBGM()
{

	const uint32_t startHandleIndex = 13;

	//for (uint32_t i = 0; i < audioManager_->kMaxPlayingSoundData; ++i) {
	//	if (audioManager_->GetPlayingSoundDatas()[i].handle_ == kClearAudioNameIndexBGM + startHandleIndex) {
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
