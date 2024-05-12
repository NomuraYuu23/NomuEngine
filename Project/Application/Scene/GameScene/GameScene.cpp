#include "GameScene.h"
#include "../../Particle/EmitterName.h"
#include "../../../Engine/3D/ModelDraw.h"
#include "../../../Engine/base/WindowSpriteStorage.h"

GameScene::~GameScene()
{

	if (stopAudio_) {
		for (uint32_t i = 0; i < audioManager_->kMaxPlayingSoundData; ++i) {
			audioManager_->StopWave(i);
		}
	}

	particleManager_->Finalize();

}

/// <summary>
/// 初期化
/// </summary>
void GameScene::Initialize() {

	IScene::Initialize();

	ModelCreate();
	TextureLoad();

	// ビュープロジェクション
	EulerTransform baseCameraTransform = {
		1.0f, 1.0f, 1.0f,
		0.58f,0.0f,0.0f,
		0.0f, 23.0f, -35.0f };
	camera_.SetTransform(baseCameraTransform);

	//パーティクル
	particleManager_ = ParticleManager::GetInstance();
	std::array<Model*, ParticleModelIndex::kCountofParticleModelIndex> particleModel{};
	particleModel[ParticleModelIndex::kUvChecker] = particleUvcheckerModel_.get();
	particleModel[ParticleModelIndex::kCircle] = particleCircleModel_.get();
	particleManager_->ModelCreate(particleModel);


	EulerTransform emitter = { {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{-3.0f,0.0f,0.0f} };

	EmitterDesc emitterDesc{};
	emitterDesc.transform = &emitter;
	emitterDesc.instanceCount = 1;
	emitterDesc.frequency = 0.5f;
	emitterDesc.lifeTime = 300.0f;
	emitterDesc.paeticleName = 0;
	emitterDesc.particleModelNum = ParticleModelIndex::kUvChecker;

	particleManager_->MakeEmitter(emitterDesc, 0);
	emitter.translate.x = 3.0f;
	emitterDesc.particleModelNum = ParticleModelIndex::kCircle;
	particleManager_->MakeEmitter(emitterDesc, 0);


	isDebugCameraActive_ = false;

	collisionManager_.reset(new CollisionManager);
	collisionManager_->Initialize();

	// オーディオマネージャー
	audioManager_ = std::make_unique<GameAudioManager>();
	audioManager_->Initialize();
	//uiManager_->SetAudioManager(audioManager_.get());

	// スカイドーム
	skydome_ = std::make_unique<Skydome>();
	skydome_->Initialize(skydomeModel_.get());

	// サンプルobj
	sampleObj_ = std::make_unique<SampleObject>();
	sampleObj_->Initialize(sampleObjModel_.get());

	collision2DManager_ = std::make_unique<Collision2DManager>();
	collision2DManager_->Initialize();

	collision2DDebugDraw_ = std::make_unique<Collision2DDebugDraw>();
	collision2DDebugDraw_->Initialize(dxCommon_->GetDevice(), collision2DDebugDrawTextures_,
		GraphicsPipelineState::sRootSignature[GraphicsPipelineState::kPipelineStateIndexCollision2DDebugDraw].Get(),
		GraphicsPipelineState::sPipelineState[GraphicsPipelineState::kPipelineStateIndexCollision2DDebugDraw].Get());

	boxCenter_ = { 640.0f,360.0f };
	
	box1Center_ = { 0.0f,0.0f };

	circleCenter_ = { 0.0f,360.0f };

	circle1Center_ = { 640.0f,0.0f };

	box_ = std::make_unique<Box>();
	box_->Initialize(boxCenter_, 160.0f, 160.0f, 0.0f, nullptr);

	box1_ = std::make_unique<Box>();
	box1_->Initialize(box1Center_, 160.0f, 160.0f, 0.0f, nullptr);

	circle_ = std::make_unique<Circle>();
	circle_->Initialize(circleCenter_, 160.0f, nullptr);

	circle1_ = std::make_unique<Circle>();
	circle1_->Initialize(circle1Center_, 160.0f, nullptr);

	testManyObject_ = std::make_unique<LargeNumberOfObjects>();
	testManyObject_->Initialize(testModel_.get());

	for (uint32_t i = 0; i < 5; ++i) {
		OneOfManyObjects* obj = new OneOfManyObjects();
		obj->Initialize();
		obj->transform_.translate.x = i * 1.0f;
		obj->materialData_.uvTransform = Matrix4x4::MakeAffineMatrix(Vector3{ 1.0f * (i + 1),1.0f * (i + 1),1.0f }, Vector3{ 0.0f,0.0f,0.0f }, Vector3{ 0.0f,0.0f,0.0f });
		testManyObject_->AddObject(obj);
	}

	testManyObject_->Update();

	segment1_ = std::make_unique<Segment2D>();
	segment1_->Initialize({ 0.0f, 360.0f }, { 1280.0f, 0.0f }, nullptr);
	segment2_ = std::make_unique<Segment2D>();
	segment2_->Initialize({ 1280.0f, 1280.0f }, { 0.0f, 0.0f }, nullptr);

	shockWaveManager_ = std::make_unique<ShockWaveManager>();
	shockWaveManager_->Initialize();

	testString_ = std::make_unique<String>();
	MassPoint testSpringMassPoint{};
	testSpringMassPoint.position = { 1.2f,0.0f,0.0f };
	testSpringMassPoint.mass = 0.5f;
	testSpringMassPoint.acceleration = { 0.0f,0.0f,0.0f };
	testSpringMassPoint.velocity = { 0.0f,0.0f,0.0f };
	testSpringMassPoint.force = { 0.0f,0.0f,0.0f };
	testString_->Initialize(
		testStringModel_.get(),
		{ 0.0f,0.0f,0.0f },
		0.001f, 500.0f, 2.0f, 0.5f
	);

	testString_->SetAnchor(0, true);

	testStringAnchor_ = { 0.0f,0.0f,0.0f };

	stringWind = { 0.0f,0.0f,0.0f };

	IScene::InitilaizeCheck();

}

/// <summary>
/// 更新処理
/// </summary>
void GameScene::Update() {

#ifdef _DEBUG
	ImguiDraw();
#endif

	if (requestSceneNo_ == kClear || requestSceneNo_ == kTitle || isBeingReset_) {
		resetScene_ = false;
		// BGM音量下げる
		if (isDecreasingVolume) {
			LowerVolumeBGM();
		}
		return;
	}

	//光源

	pointLightManager_->Update(pointLightDatas_);
	spotLightManager_->Update(spotLightDatas_);

	//Obj
	sampleObj_->Update();
	sampleObj_->DebugDrawMap(drawLine_);

	// あたり判定
	collisionManager_->ListClear();
	//collisionManager_->ListRegister();
	collisionManager_->CheakAllCollision();

	float radius = 160.0f;

	box_->Update(boxCenter_, radius, radius, 50.0f);
	box1_->Update(box1Center_, radius, radius, 5.0f);
	circle_->Update(circleCenter_, radius);
	circle1_->Update(circle1Center_, radius);
	
	// 影
	ShadowUpdate();

	// スカイドーム
	skydome_->Update();

	//uiManager_->Update();

	// デバッグカメラ
	DebugCameraUpdate();

	//パーティクル
	particleManager_->Update(camera_);

	time = std::fmodf(time + kDeltaTime_  * 10.0f, 50.0f);
	PostEffect::GetInstance()->SetTime(time + 40.0f);
	shockWaveManager_->Update();

	testString_->SetPosition(0, testStringAnchor_);
	testString_->Update(stringWind);
	testString_->DebugDrawMap(drawLine_);

	WindowSpriteStorage::GetInstance()->Reset();

}

/// <summary>
/// 描画処理
/// </summary>
void GameScene::Draw() {

	//ゲームの処理 

#pragma region 背景スプライト描画
	// 背景スプライト描画前処理
	Sprite::PreDraw(dxCommon_->GetCommadList());

	// スプライト描画後処理
	Sprite::PostDraw();
	// 深度バッファクリア
	renderTargetTexture_->ClearDepthBuffer();

#pragma endregion

#pragma region モデル描画

	ModelDraw::PreDrawDesc preDrawDesc{};
	preDrawDesc.commandList = dxCommon_->GetCommadList();
	preDrawDesc.directionalLight = directionalLight_.get();
	preDrawDesc.fogManager = FogManager::GetInstance();
	preDrawDesc.pointLightManager = pointLightManager_.get();
	preDrawDesc.spotLightManager = spotLightManager_.get();

	ModelDraw::PreDraw(preDrawDesc);

	// スカイドーム
	skydome_->Draw(camera_);

	//Obj
	sampleObj_->Draw(camera_);

	// 紐
	testString_->Draw(camera_);

	//testManyObject_->Draw(camera_);

	ModelDraw::PostDraw();

#pragma endregion
	
#pragma region 線描画

	drawLine_->Draw(dxCommon_->GetCommadList(),camera_);

#pragma endregion
	
#pragma region パーティクル描画

	// パーティクルはここ
	particleManager_->Draw(camera_.GetViewProjectionMatrix(), dxCommon_->GetCommadList());

#pragma endregion

#ifdef _DEBUG
#pragma region コライダー2d描画

	collision2DDebugDraw_->Draw(dxCommon_->GetCommadList());

#pragma endregion
#endif // DEBUG_


#pragma region 前景スプライト描画
	// 前景スプライト描画前処理
	Sprite::PreDraw(dxCommon_->GetCommadList());
	

	//背景
	//前景スプライト描画

	// UIマネージャー
	//uiManager_->Draw();

	// 前景スプライト描画後処理
	Sprite::PostDraw();

#pragma endregion

	renderTargetTexture_->ClearDepthBuffer();

	PostEffect::GetInstance()->Execution(
		dxCommon_->GetCommadList(),
		renderTargetTexture_,
		PostEffect::kCommandIndexShockWave);

	renderTargetTexture_->TextureDraw(PostEffect::GetInstance()->GetEditTextures(0)->GetUavHandleGPU());

}

void GameScene::ImguiDraw(){
#ifdef _DEBUG

	//ImGui::DragFloat2("box_", &boxCenter_.x, 0.1f);
	//ImGui::DragFloat2("box1_", &box1Center_.x, 0.1f);
	//ImGui::DragFloat2("circle_", &circleCenter_.x, 0.1f);
	//ImGui::DragFloat2("circle1_", &circle1Center_.x, 0.1f);

	ImGui::Begin("Frame rate");
	ImGui::Text("Frame rate: %6.2f fps", ImGui::GetIO().Framerate);
	ImGui::End();
	

	ImGui::Begin("testString");
	ImGui::DragFloat3("Anchor", &testStringAnchor_.x, 0.01f);
	ImGui::DragFloat3("stringWind", &stringWind.x, 0.01f);
	ImGui::End();

	PostEffect::GetInstance()->ImGuiDraw();

	shockWaveManager_->ImGuiDraw();
	FogManager::GetInstance()->ImGuiDraw();

	//Obj
	sampleObj_->ImGuiDraw();

	// スカイドーム
	skydome_->ImGuiDraw();

	debugCamera_->ImGuiDraw();

	collision2DDebugDraw_->ImGuiDraw();

#endif // _DEBUG

}

void GameScene::DebugCameraUpdate()
{

#ifdef _DEBUG
	if (input_->TriggerKey(DIK_RETURN)) {
		if (isDebugCameraActive_) {
			isDebugCameraActive_ = false;
		}
		else {
			isDebugCameraActive_ = true;
		}
	}

	// カメラの処理
	if (isDebugCameraActive_) {
		if (input_->TriggerKey(DIK_Z)) {
			debugCamera_->ShakeStart(1.0f,3.0f);
		}
		if (input_->TriggerKey(DIK_X)) {
			debugCamera_->ShakeStop();
		}

		if (input_->TriggerKey(DIK_C)) {
			debugCamera_->SetTargetFovY(0.7f);
		}
		if (input_->TriggerKey(DIK_X)) {
			debugCamera_->SetTargetFovY(0.45f);
			debugCamera_->SetFovY(0.45f);
		}

		// デバッグカメラの更新
		debugCamera_->Update(kDeltaTime_ * 5.0f);
		// デバッグカメラのビュー行列をコピー
		camera_ = static_cast<BaseCamera>(*debugCamera_.get());
		// ビュー行列の転送
		camera_.Update();
	}
#endif

}

void GameScene::ModelCreate()
{

	// パーティクル
	particleUvcheckerModel_.reset(Model::Create("Resources/default/", "plane.gltf", dxCommon_, textureHandleManager_.get()));
	particleCircleModel_.reset(Model::Create("Resources/Particle/", "plane.obj", dxCommon_, textureHandleManager_.get()));

	// スカイドーム
	skydomeModel_.reset(Model::Create("Resources/Model/Skydome/", "skydome.obj", dxCommon_, textureHandleManager_.get()));

	// サンプルobj
	//sampleObjModel_.reset(Model::Create("Resources/Model/Player2/", "player.gltf", dxCommon_, textureHandleManager_.get()));
	sampleObjModel_.reset(Model::Create("Resources/default/", "Ball.gltf", dxCommon_, textureHandleManager_.get()));

	// テスト
	testModel_.reset(Model::Create("Resources/default/", "Ball.obj", dxCommon_, textureHandleManager_.get()));

	// テスト 紐
	testStringModel_.reset(Model::Create("Resources/Model/SpearRibbon_R/", "SpearRibbon_R.gltf", dxCommon_, textureHandleManager_.get()));

}

void GameScene::TextureLoad()
{

	collision2DDebugDrawTextures_ = {
		TextureManager::Load("Resources/Debug/Box.png", DirectXCommon::GetInstance(), textureHandleManager_.get()),
		TextureManager::Load("Resources/Debug/Circle.png", DirectXCommon::GetInstance(), textureHandleManager_.get())
	};

	//uiTextureHandles_ = {

	//};
}

void GameScene::LowerVolumeBGM()
{


	const uint32_t startHandleIndex = 3;

	//for (uint32_t i = 0; i < audioManager_->kMaxPlayingSoundData; ++i) {
	//	if (audioManager_->GetPlayingSoundDatas()[i].handle_ == kGameAudioNameIndexBGM + startHandleIndex) {
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

void GameScene::ShadowUpdate()
{

	// リストクリア
	//shadowManager_->ListClear();

	// リスト登録（影を発生させる物）
	//shadowManager_->CastsShadowObjListRegister();

	// リスト登録（影が現れる物）
	//shadowManager_->ShadowAppearsObjListRegister();

	// 影が出るか
	//shadowManager_->SeeShadow();

}
