#include "MyFramework.h"

void MyFramework::Initialize()
{

	//ゲームウィンドウの作成
	win = WinApp::GetInstance();
	win->CreateGameWindow(L"2308_ピタバース");

	//DirectX初期化
	dxCommon = DirectXCommon::GetInstance();
	dxCommon->Initialize(win);

	//入力デバイス
	input = Input::GetInstance();
	input->Initialize(win->GetHInstance(), win->GetHwnd());

	GraphicsPipelineState::Initialize(dxCommon->GetDevice());

	//テクスチャマネージャー
	TextureManager::GetInstance()->Initialize(dxCommon->GetDevice());

	// スプライト静的初期化
	Sprite::StaticInitialize(dxCommon->GetDevice(), GraphicsPipelineState::sRootSignature[GraphicsPipelineState::kSprite], GraphicsPipelineState::sPipelineState[GraphicsPipelineState::kSprite]);

	// モデル静的初期化
	std::array<ID3D12RootSignature*, GraphicsPipelineState::PipelineStateName::kCountOfPipelineStateName> rootSignature = { 
		GraphicsPipelineState::sRootSignature[GraphicsPipelineState::kModel].Get(),
		GraphicsPipelineState::sRootSignature[GraphicsPipelineState::kSprite].Get(),
		GraphicsPipelineState::sRootSignature[GraphicsPipelineState::kParticle].Get(),
		GraphicsPipelineState::sRootSignature[GraphicsPipelineState::kOutLine].Get() };
	std::array<ID3D12PipelineState*, GraphicsPipelineState::PipelineStateName::kCountOfPipelineStateName> pipelineState = {
	GraphicsPipelineState::sPipelineState[GraphicsPipelineState::kModel].Get(),
	GraphicsPipelineState::sPipelineState[GraphicsPipelineState::kSprite].Get(),
	GraphicsPipelineState::sPipelineState[GraphicsPipelineState::kParticle].Get(),
	GraphicsPipelineState::sPipelineState[GraphicsPipelineState::kOutLine].Get() };
	Model::StaticInitialize(dxCommon->GetDevice(), rootSignature, pipelineState);

	// マテリアル静的初期化
	Material::StaticInitialize(dxCommon->GetDevice());

	// 光源静的初期化
	DirectionalLight::StaticInitialize(dxCommon->GetDevice());

	// パーティクル
	ParticleManager::GetInstance()->Initialize();

	//サウンド
	audio = Audio::GetInstance();
	audio->Initialize();

	// ImGuiマネージャー
	imGuiManager = ImGuiManager::GetInstance();
	imGuiManager->Initialize(win, dxCommon, TextureManager::GetInstance());

	//グローバル変数ファイル読み込み
	GlobalVariables::GetInstance()->LoadFiles();

	// アニメーションファイル読み込み
	AnimationFile::GetInstance()->LoadFiles();

	endRequst_ = false;

}

void MyFramework::Finalize()
{

	// サウンド後始末
	audio->Finalize();

	// パーティクル後始末
	ParticleManager::GetInstance()->Finalize();

	//色々な解放処理の前に書く
	imGuiManager->Finalize();

	//ゲームウィンドウの破棄
	win->TerminateGameWindow();

}

void MyFramework::Update()
{

	//Windowにメッセージが来てたら最優先で処理させる
	if (win->ProcessMessage() || input->TriggerKey(DIK_ESCAPE)) {
		endRequst_ = true;
	}

	//入力デバイス
	input->Update();
	if (!input->GetJoystickConnected()) {
		input->JoystickConnected(win->GetHwnd());
	}

	//ゲームの処理 
	//ImGui
	imGuiManager->Begin();

	//開発用UIの処理。実際に開発用のUIを出す場合はここをゲーム固有の処理に置き換える
	//ImGui::ShowDemoWindow();
	// グローバル変数の更新
	GlobalVariables::GetInstance()->Update();
	
	// アニメーション変数の更新
	AnimationFile::GetInstance()->Update();

}

void MyFramework::Run()
{

	Initialize();

	//ウィンドウののボタンが押されるまでループ
	while (true) {

		Update();
		if (IsEndRequst()) {
			break;
		}
		Draw();
	}

	Finalize();

}
