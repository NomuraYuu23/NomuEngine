#include "SelectScene.h"

void SelectScene::Initialize()
{

	IScene::Initialize();

	ModelCreate();
	TextureLoad();

	IScene::InitilaizeCheck();

}

void SelectScene::Update()
{

	if (input_->TriggerKey(DIK_SPACE)) {
		// 行きたいシーンへ
		requestSceneNo_ = kGame;
	}

}

void SelectScene::Draw()
{
}

void SelectScene::ModelCreate()
{
}

void SelectScene::TextureLoad()
{
}
