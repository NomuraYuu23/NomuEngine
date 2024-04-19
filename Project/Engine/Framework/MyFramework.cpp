#include "MyFramework.h"

void MyFramework::Initialize()
{

	//ゲームウィンドウの作成
	win = WinApp::GetInstance();
	win->CreateGameWindow(L"NomuEngine");

	//DirectX初期化
	dxCommon = DirectXCommon::GetInstance();
	dxCommon->Initialize(win);

	//入力デバイス
	input = Input::GetInstance();
	input->Initialize(win->GetHInstance(), win->GetHwnd());

	//テクスチャマネージャー
	TextureManager::GetInstance()->Initialize(dxCommon->GetDevice());

	// マテリアル静的初期化
	Material::StaticInitialize(dxCommon->GetDevice());

	// スプライト静的初期化
	Sprite::StaticInitialize(dxCommon->GetDevice(), 
		GraphicsPipelineState::sRootSignature[GraphicsPipelineState::kPipelineStateNameSprite].Get(), 
		GraphicsPipelineState::sPipelineState[GraphicsPipelineState::kPipelineStateNameSprite].Get());

	// Windowサイズのスプライト
	WindowSprite::GetInstance()->Initialize(dxCommon->GetDevice(), dxCommon->GetCommadList());

	// モデル静的初期化
	std::array<ID3D12RootSignature*, ModelDraw::PipelineStateIndex::kPipelineStateIndexOfCount> rootSignature = {
		GraphicsPipelineState::sRootSignature[GraphicsPipelineState::kPipelineStateNameModel].Get()/*,
		GraphicsPipelineState::sRootSignature[GraphicsPipelineState::kPipelineStateNameParticle].Get(),
		GraphicsPipelineState::sRootSignature[GraphicsPipelineState::kPipelineStateNameOutLine].Get(),
		GraphicsPipelineState::sRootSignature[GraphicsPipelineState::kPipelineStateNameManyModels].Get()*/ };
	std::array<ID3D12PipelineState*, ModelDraw::PipelineStateIndex::kPipelineStateIndexOfCount> pipelineState = {
	GraphicsPipelineState::sPipelineState[GraphicsPipelineState::kPipelineStateNameModel].Get()/*,
	GraphicsPipelineState::sPipelineState[GraphicsPipelineState::kPipelineStateNameParticle].Get(),
	GraphicsPipelineState::sPipelineState[GraphicsPipelineState::kPipelineStateNameOutLine].Get(),
	GraphicsPipelineState::sPipelineState[GraphicsPipelineState::kPipelineStateNameManyModels].Get()*/ };
	Model::StaticInitialize(dxCommon->GetDevice());

	// モデル描画
	ModelDraw::Initialize(rootSignature, pipelineState);

	// 線描画静的初期化
	DrawLine::StaticInitialize(dxCommon->GetDevice(),
		GraphicsPipelineState::sRootSignature[GraphicsPipelineState::kPipelineStateNameLine].Get(),
		GraphicsPipelineState::sPipelineState[GraphicsPipelineState::kPipelineStateNameLine].Get());

	// 光源静的初期化
	DirectionalLight::StaticInitialize(dxCommon->GetDevice());
	PointLightManager::StaticInitialize(dxCommon->GetDevice());
	SpotLightManager::StaticInitialize(dxCommon->GetDevice());

	// パーティクル
	ParticleManager::GetInstance()->Initialize();

	// ポストエフェクト
	PostEffect::GetInstance()->Initialize();

	// 霧
	FogManager::GetInstance()->Initialize();

	//サウンド
	audio = Audio::GetInstance();
	audio->Initialize();

	// ImGuiマネージャー
	imGuiManager = ImGuiManager::GetInstance();
	imGuiManager->Initialize(win, dxCommon);

	//グローバル変数ファイル読み込み
	GlobalVariables::GetInstance()->LoadFiles();

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
