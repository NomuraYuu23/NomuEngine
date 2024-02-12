#include "MyGame.h"

void MyGame::Initialize()
{

	// 基底クラスの初期化
	MyFramework::Initialize();

	//ゲームシーン
	sceneManager = std::make_unique<SceneManager>();
	sceneManager->Initialize(kTitle);

	imGuiDraw = sceneManager->GetInitializing();

}

void MyGame::Finalize()
{

	// 基底クラスの終了処理
	MyFramework::Finalize();

}

void MyGame::Update()
{

	MyFramework::Update();

	// ゲームシーン更新
	sceneManager->Update();

	imGuiDraw = !sceneManager->GetInitializing();

}

void MyGame::Draw()
{

	//描画前処理
	dxCommon->PreDraw();

	//ゲームシーン描画処理
	sceneManager->Draw();

	imGuiManager->End();

	if (imGuiDraw) {
		// シェーダーリソースビューをセット
		TextureManager::GetInstance()->SetGraphicsRootDescriptorTable(dxCommon->GetCommadList(), 2, 0);
		//実際のcommandListのImGuiの描画コマンドを積む
		imGuiManager->Draw();
	}

	//描画後処理
	dxCommon->PostDraw();

}
