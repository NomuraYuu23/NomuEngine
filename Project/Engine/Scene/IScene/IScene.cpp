#include "IScene.h"

//タイトルシーンで初期化
int IScene::sceneNo = kTitle;
int IScene::requestSceneNo = kTitle;

DirectXCommon* IScene::dxCommon_ = nullptr;
Input* IScene::input_ = nullptr;
Audio* IScene::audio_ = nullptr;

//ビュープロジェクション
BaseCamera IScene::camera_;

//デバッグカメラ
std::unique_ptr<DebugCamera> IScene::debugCamera_;
bool IScene::isDebugCameraActive_;

void IScene::StaticInitialize()
{

	//機能
	dxCommon_ = DirectXCommon::GetInstance();
	input_ = Input::GetInstance();
	audio_ = Audio::GetInstance();

	//ビュープロジェクション
	camera_.Initialize();

	//デバッグカメラ
	debugCamera_ = std::make_unique<DebugCamera>();
	debugCamera_->Initialize();
	isDebugCameraActive_ = false;

}

void IScene::Initialize()
{

	textureHandleManager_ = std::make_unique<ITextureHandleManager>();
	textureHandleManager_->Initialize();

}

IScene::~IScene(){

	textureHandleManager_->ResetTextureHandles();

}

int IScene::GetSceneNo(){ return sceneNo; }

int IScene::GetRequestSceneNo(){ return requestSceneNo; }

void IScene::ModelCreate(){}

void IScene::TextureLoad(){}
