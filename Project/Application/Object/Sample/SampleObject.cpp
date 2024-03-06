#include "SampleObject.h"
#include "../../../Engine/GlobalVariables/GlobalVariables.h"
#include "../../../Engine/2D/ImguiManager.h"

SampleObject::~SampleObject()
{

}

void SampleObject::Initialize(Model* model)
{

	model_ = model;

	material_.reset(Material::Create());

	worldtransform_.Initialize(model_->GetRootNode());

	// 初期ローカル座標
	std::vector<Vector3> initPositions;
	initPositions.resize(worldtransform_.GetNodeDatas().size());
	initPositions[0] = { 0.0f, 0.0f, 0.0f };
	initPositions[1] = { 0.0f, 0.0f, 0.0f };

	std::vector<Quaternion> initRotations;
	initRotations.resize(worldtransform_.GetNodeDatas().size());
	initRotations[0] = { 0.707107f, 0.0f, 0.0f, 0.707107f };
	initRotations[1] = { 0.707107f, 0.0f, 0.0f, 0.707107f };

	std::vector<Vector3> initScalings;
	initScalings.resize(worldtransform_.GetNodeDatas().size());
	initScalings[0] = { 1.0f, 1.0f, 1.0f };
	initScalings[1] = { 1.0f, 1.0f, 1.0f };

	nodeAnimation_.Initialize(
		model_->GetNodeAnimationData(),
		initPositions,
		initRotations,
		initScalings,
		worldtransform_.GetNodeNames());

	nodeAnimation_.startAnimation(0, true);
	nodeAnimation_.startAnimation(1, true);

	enableLighting_ = 0;

	shininess_ = 100.0f;

	RegisteringGlobalVariables();

	ApplyGlobalVariables();

}

void SampleObject::Update()
{

	ApplyGlobalVariables();

	worldtransform_.SetNodeLocalMatrix(nodeAnimation_.Animation());

	worldtransform_.UpdateMatrix();

	material_->SetEnableLighting(enableLighting_);
	material_->SetShininess(shininess_);

}

void SampleObject::Draw(BaseCamera camera)
{

	model_->Draw(worldtransform_, camera, material_.get());

}

void SampleObject::ImGuiDraw()
{

	ImGui::Begin("SampleObject");

	ImGui::Text("enableLighting");
	ImGui::RadioButton("None", &enableLighting_, EnableLighting::None);
	ImGui::SameLine();
	ImGui::RadioButton("Lambert", &enableLighting_, EnableLighting::Lambert);
	ImGui::SameLine();
	ImGui::RadioButton("HalfLambert", &enableLighting_, EnableLighting::HalfLambert);
	ImGui::SameLine();
	ImGui::RadioButton("PhongReflection", &enableLighting_, EnableLighting::PhongReflection);
	ImGui::SameLine();
	ImGui::RadioButton("BlinnPhongReflection", &enableLighting_, EnableLighting::BlinnPhongReflection);

	ImGui::DragFloat("shininess", &shininess_);

	ImGui::End();

}

void SampleObject::RegisteringGlobalVariables()
{

	GlobalVariables* globalVariables = GlobalVariables::GetInstance();

	// グループ名
	const std::string groupName = "SampleObject";

	// スケール
	globalVariables->AddItem(groupName, "Scale", worldtransform_.transform_.scale);
	// 回転
	globalVariables->AddItem(groupName, "Rotate", worldtransform_.transform_.rotate);
	// 位置
	globalVariables->AddItem(groupName, "Transform", worldtransform_.transform_.translate);

}

void SampleObject::ApplyGlobalVariables()
{

	GlobalVariables* globalVariables = GlobalVariables::GetInstance();

	// グループ名
	const std::string groupName = "SampleObject";

	// スケール
	worldtransform_.transform_.scale = globalVariables->GetVector3Value(groupName, "Scale");
	// 回転
	worldtransform_.transform_.rotate = globalVariables->GetVector3Value(groupName, "Rotate");
	// 位置
	worldtransform_.transform_.translate = globalVariables->GetVector3Value(groupName, "Transform");

}
