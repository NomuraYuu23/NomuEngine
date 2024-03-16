#include "Skydome.h"
#include <cassert>
#include "../../Engine/2D/ImguiManager.h"
#include <numbers>

Skydome::~Skydome()
{

}

/// <summary>
/// 初期化
/// </summary>
/// <param name="model">モデル</param>
void Skydome::Initialize(Model* model) {

	// nullポインタチェック
	assert(model);

	model_ = model;

	material_.reset(Material::Create());

	// ワールド変換データの初期化
	worldTransform_.Initialize(model_->GetRootNode());
}

/// <summary>
/// 更新
/// </summary>
void Skydome::Update() {

	Vector3 rotate = worldTransform_.GetRoatateVector();
	rotate.y = fmodf(rotate.y + rotateSpeed_, static_cast<float>(std::numbers::pi) * 2.0f);

	worldTransform_.SetRoatate(rotate);

	worldTransform_.UpdateMatrix();

}

/// <summary>
/// 描画
/// </summary>
/// <param name="viewProjection">ビュープロジェクション</param>
void Skydome::Draw(BaseCamera& camera) {

	model_->Draw(worldTransform_, camera, material_.get());

}

void Skydome::ImGuiDraw()
{

	ImGui::Begin("Skydome");
	ImGui::DragFloat("rotateSpeed", &rotateSpeed_, 0.001f);
	ImGui::End();

}
