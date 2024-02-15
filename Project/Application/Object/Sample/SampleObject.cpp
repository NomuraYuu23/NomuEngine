#include "SampleObject.h"
#include "../../../Engine/GlobalVariables/GlobalVariables.h"
#include "../../../Engine/2D/ImguiManager.h"

void SampleObject::Initialize(Model* model)
{

	model_ = model;

	material_.reset(Material::Create());

	worldtransform_.Initialize();

	enableLighting_ = 0;

	shininess_ = 100.0f;

	RegisteringGlobalVariables();

	ApplyGlobalVariables();

}

void SampleObject::Update()
{

	ApplyGlobalVariables();

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
