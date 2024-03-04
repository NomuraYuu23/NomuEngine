#include "GameScene.h"
#include "../../../Engine/base/WinApp.h"
#include "../../../Engine/base/TextureManager.h"
#include "../../../Engine/2D/ImguiManager.h"
#include "../../../Engine/base/D3DResourceLeakChecker.h"
#include "../../../Engine/GlobalVariables/GlobalVariables.h"
#include "../../Particle/EmitterName.h"
#include "../../../Engine/Math/DeltaTime.h"

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
	TransformStructure baseCameraTransform = {
		1.0f, 1.0f, 1.0f,
		0.58f,0.0f,0.0f,
		0.0f, 23.0f, -35.0f };
	camera_.SetTransform(baseCameraTransform);

	//パーティクル
	particleManager_ = ParticleManager::GetInstance();
	std::array<Model*, ParticleModelIndex::kCountofParticleModelIndex> particleModel;
	particleModel[ParticleModelIndex::kUvChecker] = particleUvcheckerModel_.get();
	particleModel[ParticleModelIndex::kCircle] = particleCircleModel_.get();
	particleManager_->ModelCreate(particleModel);
	TransformStructure emitter = { {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{-3.0f,0.0f,0.0f} };
	particleManager_->MakeEmitter(emitter, 1, 0.5f, 300.0f, ParticleModelIndex::kUvChecker, 0, 0);
	emitter.translate.x = 3.0f;
	particleManager_->MakeEmitter(emitter, 1, 0.5f, 300.0f, ParticleModelIndex::kCircle, 0, 0);


	isDebugCameraActive_ = false;

	collisionManager_.reset(new CollisionManager);
	collisionManager_->Initialize();

	//UIマネージャー
	//uiManager_ = std::make_unique<UIManager>();
	//uiManager_->Initialize(uiTextureHandles_);

	// オーディオマネージャー
	audioManager_ = std::make_unique<GameAudioManager>();
	audioManager_->Initialize();
	//uiManager_->SetAudioManager(audioManager_.get());

	// スカイドーム
	skydome_ = std::make_unique<Skydome>();
	skydome_->Initialize(skydomeModel_.get());


	outline_.Initialize();
	outline_.color_ = { 0.8f,0.4f,0.1f,1.0f };

	//影
	//shadowManager_ = std::make_unique<ShadowManager>();
	//shadowManager_->Initialize(shadowModel_.get());

	// 平行光源
	directionalLight_ = std::make_unique<DirectionalLight>();
	directionalLight_->Initialize();

	// サンプルobj
	sampleObj_ = std::make_unique<SampleObject>();
	sampleObj_->Initialize(sampleObjModel_.get());

	// 点光源
	pointLightManager_ = std::make_unique<PointLightManager>();
	pointLightManager_->Initialize();
	for (size_t i = 0; i < pointLightDatas_.size(); ++i) {
		pointLightDatas_[i].color = { 1.0f,1.0f,1.0f,1.0f };
		pointLightDatas_[i].position = { 0.0f, -1.0f, 0.0f };
		pointLightDatas_[i].intencity = 1.0f;
		pointLightDatas_[i].radius = 10.0f;
		pointLightDatas_[i].decay = 10.0f;
		pointLightDatas_[i].used = false;
	}

	spotLightManager_ = std::make_unique<SpotLightManager>();
	spotLightManager_->Initialize();
	for (size_t i = 0; i < spotLightDatas_.size(); ++i) {
		spotLightDatas_[i].color = { 1.0f,1.0f,1.0f,1.0f };
		spotLightDatas_[i].position = { 0.0f, -1.0f, 0.0f };
		spotLightDatas_[i].intencity = 1.0f;
		spotLightDatas_[i].direction = { 0.0f, -1.0f, 0.0f }; // ライトの方向
		spotLightDatas_[i].distance = 10.0f; // ライトの届く距離
		spotLightDatas_[i].decay = 2.0f; // 減衰率
		spotLightDatas_[i].cosAngle = 2.0f; // スポットライトの余弦
		spotLightDatas_[i].cosFalloffStart = 1.0f; // フォールオフ開始位置
		spotLightDatas_[i].used = false; // 使用している
	}

	collision2DManager_ = std::make_unique<Collision2DManager>();
	collision2DManager_->Initialize();

	collision2DDebugDraw_ = std::make_unique<Collision2DDebugDraw>();
	collision2DDebugDraw_->Initialize(dxCommon_->GetDevice(), collision2DDebugDrawTextures_,
		GraphicsPipelineState::sRootSignature[GraphicsPipelineState::kPipelineStateNameCollision2DDebugDraw].Get(),
		GraphicsPipelineState::sPipelineState[GraphicsPipelineState::kPipelineStateNameCollision2DDebugDraw].Get());

	boxCenter_ = { 640.0f,360.0f };
	
	box1Center_ = { 0.0f,0.0f };

	circleCenter_ = { 0.0f,360.0f };

	circle1Center_ = { 640.0f,0.0f };

	box_ = std::make_unique<Box>();
	box_->Initialize(boxCenter_, 160.0f, 160.0f, nullptr);

	box1_ = std::make_unique<Box>();
	box1_->Initialize(box1Center_, 160.0f, 160.0f, nullptr);

	circle_ = std::make_unique<Circle>();
	circle_->Initialize(circleCenter_, 160.0f, nullptr);

	circle1_ = std::make_unique<Circle>();
	circle1_->Initialize(circle1Center_, 160.0f, nullptr);

}

/// <summary>
/// 更新処理
/// </summary>
void GameScene::Update() {

#ifdef _DEBUG
	ImguiDraw();
#endif

	if (requestSceneNo == kClear || requestSceneNo == kTitle || isBeingReset_) {
		resetScene_ = false;
		// BGM音量下げる
		if (isDecreasingVolume) {
			LowerVolumeBGM();
		}
		return;
	}

	// リスタート
	//if () {
	//	resetScene_ = true;
	//	isBeingReset_ = true;
	//	isDecreasingVolume = true;
	//}

	//光源
	DirectionalLightData directionalLightData;
	directionalLightData.color = { 1.0f,1.0f,1.0f,1.0f };
	directionalLightData.direction = Vector3::Normalize(direction);
	directionalLightData.intencity = intencity;
	directionalLight_->Update(directionalLightData);

	pointLightManager_->Update(pointLightDatas_);
	spotLightManager_->Update(spotLightDatas_);

	//Obj
	sampleObj_->Update();

	// あたり判定
	collisionManager_->ListClear();
	//collisionManager_->ListRegister();
	collisionManager_->CheakAllCollision();

	float radius = 160.0f;

	box_->Update(boxCenter_, radius, radius);
	box1_->Update(box1Center_, radius, radius);
	circle_->Update(circleCenter_, radius);
	circle1_->Update(circle1Center_, radius);

	collision2DManager_->ListClear();
	collision2DManager_->ListRegister(box_.get());
	collision2DManager_->ListRegister(box1_.get());
	collision2DManager_->ListRegister(circle_.get());
	collision2DManager_->ListRegister(circle1_.get());
	collision2DManager_->CheakAllCollision();

	collision2DDebugDraw_->Clear();
	collision2DDebugDraw_->Register(box_.get());
	collision2DDebugDraw_->Register(box1_.get());
	collision2DDebugDraw_->Register(circle_.get());
	collision2DDebugDraw_->Register(circle1_.get());
	
	// 影
	ShadowUpdate();

	// スカイドーム
	skydome_->Update();

	//uiManager_->Update();

	// デバッグカメラ
	DebugCameraUpdate();

	//パーティクル
	particleManager_->Update(camera_);

	//アウトライン
	outline_.Map();

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
	dxCommon_->ClearDepthBuffer();


#pragma endregion

	Model::PreDraw(dxCommon_->GetCommadList(), pointLightManager_.get(), spotLightManager_.get());

	//光源
	directionalLight_->Draw(dxCommon_->GetCommadList(), 6);
	//3Dオブジェクトはここ
	
	//Obj
	sampleObj_->Draw(camera_);

	// スカイドーム
	skydome_->Draw(camera_);

#ifdef _DEBUG

	// デバッグ描画
	//colliderDebugDraw_->Draw(camera_);

#endif // _DEBUG

	Model::PostDraw();
	
#pragma region アウトライン描画
	Model::PreDrawOutLine(dxCommon_->GetCommadList());
	
	Model::PostDraw();

#pragma endregion

#pragma region パーティクル描画
	Model::PreParticleDraw(dxCommon_->GetCommadList(), camera_.GetViewProjectionMatrix());

	//光源
	directionalLight_->Draw(dxCommon_->GetCommadList(), 6);

	// パーティクルはここ
	particleManager_->Draw();

	Model::PostDraw();

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

}

void GameScene::ImguiDraw(){
#ifdef _DEBUG

	ImGui::Begin("Light");
	ImGui::DragFloat3("direction", &direction.x, 0.1f);
	ImGui::DragFloat("i", &intencity, 0.01f);

	//ImGui::DragFloat3("PointPosition0", &pointLightDatas_[0].position.x, 0.1f);
	//ImGui::DragFloat("PointIntencity0", &pointLightDatas_[0].intencity, 0.01f);
	//ImGui::DragFloat("PointRadius0", &pointLightDatas_[0].radius, 0.01f);
	//ImGui::DragFloat("PointDecay0", &pointLightDatas_[0].decay, 0.01f);

	//ImGui::DragFloat3("PointPosition1", &pointLightDatas_[1].position.x, 0.1f);
	//ImGui::DragFloat("PointIntencity1", &pointLightDatas_[1].intencity, 0.01f);
	//ImGui::DragFloat("PointRadius1", &pointLightDatas_[1].radius, 0.01f);
	//ImGui::DragFloat("PointDecay1", &pointLightDatas_[1].decay, 0.01f);

	//ImGui::DragFloat3("PointPosition2", &pointLightDatas_[2].position.x, 0.1f);
	//ImGui::DragFloat("PointIntencity2", &pointLightDatas_[2].intencity, 0.01f);
	//ImGui::DragFloat("PointRadius2", &pointLightDatas_[2].radius, 0.01f);
	//ImGui::DragFloat("PointDecay2", &pointLightDatas_[2].decay, 0.01f);

	//ImGui::DragFloat3("SpotPosition0", &spotLightDatas_[0].position.x, 0.1f);
	//ImGui::DragFloat("SpotIntencity0", &spotLightDatas_[0].intencity, 0.01f);
	//ImGui::DragFloat3("SpotDirection0", &spotLightDatas_[0].direction.x, 0.1f);
	//ImGui::DragFloat("SpotDistance0", &spotLightDatas_[0].distance, 0.01f);
	//ImGui::DragFloat("SpotDecay0", &spotLightDatas_[0].decay, 0.01f);
	//ImGui::DragFloat("SpotCosAngle0", &spotLightDatas_[0].cosAngle, 0.01f);
	//ImGui::DragFloat("SpotCosFalloffStart0", &spotLightDatas_[0].cosFalloffStart, 0.01f);

	//ImGui::DragFloat3("SpotPosition1", &spotLightDatas_[1].position.x, 0.1f);
	//ImGui::DragFloat("SpotIntencity1", &spotLightDatas_[1].intencity, 0.01f);
	//ImGui::DragFloat3("SpotDirection1", &spotLightDatas_[1].direction.x, 0.1f);
	//ImGui::DragFloat("SpotDistance1", &spotLightDatas_[1].distance, 0.01f);
	//ImGui::DragFloat("SpotDecay1", &spotLightDatas_[1].decay, 0.01f);
	//ImGui::DragFloat("SpotCosAngle1", &spotLightDatas_[1].cosAngle, 0.01f);
	//ImGui::DragFloat("SpotCosFalloffStart1", &spotLightDatas_[1].cosFalloffStart, 0.01f);

	//ImGui::DragFloat3("SpotPosition2", &spotLightDatas_[2].position.x, 0.1f);
	//ImGui::DragFloat("SpotIntencity2", &spotLightDatas_[2].intencity, 0.01f);
	//ImGui::DragFloat3("SpotDirection2", &spotLightDatas_[2].direction.x, 0.1f);
	//ImGui::DragFloat("SpotDistance2", &spotLightDatas_[2].distance, 0.01f);
	//ImGui::DragFloat("SpotDecay2", &spotLightDatas_[2].decay, 0.01f);
	//ImGui::DragFloat("SpotCosAngle2", &spotLightDatas_[2].cosAngle, 0.01f);
	//ImGui::DragFloat("SpotCosFalloffStart2", &spotLightDatas_[2].cosFalloffStart, 0.01f);

	//ImGui::DragFloat3("SpotPosition3", &spotLightDatas_[3].position.x, 0.1f);
	//ImGui::DragFloat("SpotIntencity3", &spotLightDatas_[3].intencity, 0.01f);
	//ImGui::DragFloat3("SpotDirection3", &spotLightDatas_[3].direction.x, 0.1f);
	//ImGui::DragFloat("SpotDistance3", &spotLightDatas_[3].distance, 0.01f);
	//ImGui::DragFloat("SpotDecay3", &spotLightDatas_[3].decay, 0.01f);
	//ImGui::DragFloat("SpotCosAngle3", &spotLightDatas_[3].cosAngle, 0.01f);
	//ImGui::DragFloat("SpotCosFalloffStart3", &spotLightDatas_[3].cosFalloffStart, 0.01f);


	ImGui::DragFloat2("box_", &boxCenter_.x, 0.1f);
	ImGui::DragFloat2("box1_", &box1Center_.x, 0.1f);
	ImGui::DragFloat2("circle_", &circleCenter_.x, 0.1f);
	ImGui::DragFloat2("circle1_", &circle1Center_.x, 0.1f);

	ImGui::Text("Frame rate: %6.2f fps", ImGui::GetIO().Framerate);
	ImGui::End();

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
		// デバッグカメラの更新
		debugCamera_->Update();
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
	sampleObjModel_.reset(Model::Create("Resources/default/assimp/Animation_Node/", "Animation_Node_00.gltf", dxCommon_, textureHandleManager_.get()));

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
