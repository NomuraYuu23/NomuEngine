#include "IScene.h"

//タイトルシーンで初期化
int IScene::sceneNo_ = kTitle;
int IScene::requestSceneNo_ = kTitle;

int IScene::sceneNoCheck_ = kTitle;
int IScene::requestSceneNoCheck_ = kTitle;

DirectXCommon* IScene::dxCommon_ = nullptr;
Input* IScene::input_ = nullptr;
Audio* IScene::audio_ = nullptr;

//ビュープロジェクション
BaseCamera IScene::camera_;

//デバッグカメラ
std::unique_ptr<DebugCamera> IScene::debugCamera_;
bool IScene::isDebugCameraActive_;

RenderTargetTexture* IScene::renderTargetTexture_;

DrawLine* IScene::drawLine_ = nullptr;

void IScene::StaticInitialize()
{

	sceneNoCheck_ = sceneNo_;
	requestSceneNoCheck_ = requestSceneNo_;

	//機能
	dxCommon_ = DirectXCommon::GetInstance();
	input_ = Input::GetInstance();
	audio_ = Audio::GetInstance();
	renderTargetTexture_ = dxCommon_->GetRenderTargetTexture();

	//ビュープロジェクション
	camera_.Initialize();

	//デバッグカメラ
	debugCamera_ = std::make_unique<DebugCamera>();
	debugCamera_->Initialize();
	isDebugCameraActive_ = false;
	
	// 線描画
	drawLine_ = DrawLine::GetInstance();
	drawLine_->Initialize(dxCommon_->GetDevice(), 
		GraphicsPipelineState::sRootSignature[GraphicsPipelineState::kPipelineStateIndexLine].Get(),
		GraphicsPipelineState::sPipelineState[GraphicsPipelineState::kPipelineStateIndexLine].Get());

}

void IScene::Initialize()
{

	textureHandleManager_ = std::make_unique<ITextureHandleManager>();
	textureHandleManager_->Initialize();

}

IScene::~IScene(){

	textureHandleManager_->ResetTextureHandles();

}

int IScene::GetSceneNo(){ return sceneNo_; }

int IScene::GetRequestSceneNo(){ return requestSceneNo_; }

void IScene::ModelCreate(){}

void IScene::TextureLoad(){}

void IScene::InitilaizeCheck()
{

	assert(requestSceneNo_ == requestSceneNoCheck_);
	assert(sceneNo_ == sceneNoCheck_);

}
