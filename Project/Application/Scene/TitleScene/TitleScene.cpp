#include "TitleScene.h"
#include "../../../Engine/base/TextureManager.h"
#include "../../../Engine/2D/ImguiManager.h"
#include "../../../Engine/GlobalVariables/GlobalVariables.h"
#include "../../../Engine/Math/Ease.h"

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
	TransformStructure baseCameraTransform = {
		1.0f, 1.0f, 1.0f,
		0.0f,0.0f,0.0f,
		0.0f, 0.0f, -35.0f };
	camera_.SetTransform(baseCameraTransform);

	// スカイドーム
	skydome_ = std::make_unique<Skydome>();
	skydome_->Initialize(skydomeModel_.get());

	//アウトライン
	outline_.Initialize();
	outline_.Map();

	glare_ = std::make_unique<Glare>();
	std::array<uint32_t, Glare::kImageForGlareIndexOfCount> test;
	glare_->Initialize(test);

}

void TitleScene::Update()
{

#ifdef _DEBUG
	ImguiDraw();
#endif // _DEBUG

	if ((input_->TriggerJoystick(JoystickButton::kJoystickButtonA) || input_->TriggerKey(DIK_SPACE)) &&
		requestSceneNo == kTitle) {
		// 行きたいシーンへ
		requestSceneNo = kGame;
	}

	// BGM音量下げる
	if (requestSceneNo == kTutorial && isDecreasingVolume) {
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

	Model::PreDraw(dxCommon_->GetCommadList());

	//3Dオブジェクトはここ

	// スカイドーム
	if (isDrawSkydome_) {
		skydome_->Draw(camera_);
	}

	Model::PostDraw();
	Model::PreDrawOutLine(dxCommon_->GetCommadList());

	Model::PostDraw();
#pragma region 前景スプライト描画
	// 前景スプライト描画前処理
	Sprite::PreDraw(dxCommon_->GetCommadList());

	//背景
	//前景スプライト描画

	// 前景スプライト描画後処理
	Sprite::PostDraw();

#pragma endregion

	renderTargetTexture_->ChangePixelShaderResource(0);
	glare_->Execution(
		renderTargetTexture_->GetSrvGPUHandle(0),
		0.8f,
		0.8f,
		Glare::kImageForGlareIndexHalo,
		dxCommon_->GetCommadList());
	renderTargetTexture_->ChangeRenderTarget(0);
	renderTargetTexture_->TextureDraw(glare_->GetEditTextures(0)->GetUavHandleGPU());

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
