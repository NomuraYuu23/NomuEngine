#include "SceneManager.h"

SceneManager::~SceneManager()
{

	if (sceneInitializing_) {
		sceneInitialize_.join();
	}
	if (sceneTransitionInitializing_) {
		sceneTransitionInitialize_.join();
	}

}

void SceneManager::Initialize(uint32_t earlySceneNo)
{

	// シーンファクトリー
	sceneFacyory_ = SceneFactory::GetInstance();

	// シーン(タイトル)
	scene_.reset(sceneFacyory_->CreateScene(earlySceneNo));
	// シーンの静的初期化
	scene_->StaticInitialize();
	// シーンの初期化
	sceneInitialize_ = std::thread(std::bind(&SceneManager::SceneInitializeThread, this));
	sceneInitializing_ = true;

	// 初期シーン
	currentSceneNo_ = earlySceneNo;

	requestSeneNo_ = earlySceneNo; // リクエストシーン
	prevRequestSeneNo_ = earlySceneNo; // 前のリクエストシーン

	// シーン遷移ファクトリー
	sceneTransitionFactory_ = SceneTransitionFactory::GetInstance();

	// シーン遷移を保持するメンバ変数
	sceneTransition_.reset(sceneTransitionFactory_->CreateSceneTransition(currentSceneNo_, requestSeneNo_));
	sceneTransition_->Initialize();
	sceneTransition_->SetStoppingUpdates(true);
	sceneTransition_->SetIsFadeIn(false);

}

void SceneManager::Update()
{

	// シーンのチェック
	if (!sceneInitializing_) {
		currentSceneNo_ = scene_->GetSceneNo();

		prevRequestSeneNo_ = requestSeneNo_; // 前のリクエストシーン
		requestSeneNo_ = scene_->GetRequestSceneNo(); // リクエストシーン
	}

	// リクエストシーンが変わったか
	if ( (requestSeneNo_ != prevRequestSeneNo_ || scene_->GetResetScene())
		&& !sceneTransitionInitializing_ && !sceneTransitionInitializeEnd_) {
		//シーン遷移開始（初期化）
		sceneTransition_.reset(sceneTransitionFactory_->CreateSceneTransition(currentSceneNo_, requestSeneNo_));
		sceneTransitionInitialize_ = std::thread(std::bind(&SceneManager::SceneTransitionInitializeThread, this));
		sceneTransitionInitializing_ = true;
	}

	//シーン遷移中
	if (sceneTransition_ && !sceneTransitionInitializing_) {
		// シーン遷移更新
		sceneTransition_->Update();
		if (sceneTransition_->GetSwitchScene()) {
			// シーン切り替え
			currentSceneNo_ = requestSeneNo_;
			scene_->SetStopAudio(true);
			scene_.reset(sceneFacyory_->CreateScene(currentSceneNo_));
			// シーンの初期化
			sceneInitialize_ = std::thread(std::bind(&SceneManager::SceneInitializeThread, this));
			sceneInitializing_ = true;
			sceneTransition_->SetSwitchScene(false);
			sceneTransition_->SetStoppingUpdates(true);
		}
		else if (!sceneTransition_->GetTransitioning()) {
			sceneTransition_.reset(nullptr);
		}
	}

	// 更新処理
	if (!sceneInitializing_) {
		scene_->Update();
	}

	// シーン初期化終了
	if (sceneInitializeEnd_) {
		sceneInitializeEnd_ = false;
		sceneInitialize_.detach();
		if (sceneTransition_) {
			sceneTransition_->SetStoppingUpdates(false);
		}
	}

	// シーン遷移初期化終了
	if (sceneTransitionInitializeEnd_) {
		sceneTransitionInitializeEnd_ = false;
		sceneTransitionInitialize_.detach();
	}

}

void SceneManager::Draw()
{
	// 描画処理
	if (!sceneInitializing_) {
		scene_->Draw();
	}
	if (sceneTransition_ && !sceneTransitionInitializing_) {
		sceneTransition_->Draw();
	}

}

void SceneManager::SceneInitializeThread()
{

	sceneInitializeEnd_ = false;
	sceneInitializing_ = true;
	scene_->Initialize();
	sceneInitializeEnd_ = true;
	sceneInitializing_ = false;

}

void SceneManager::SceneTransitionInitializeThread()
{

	sceneTransitionInitializeEnd_ = false;
	sceneTransitionInitializing_ = true;
	sceneTransition_->Initialize();
	sceneTransitionInitializeEnd_ = true;
	sceneTransitionInitializing_ = false;

}
