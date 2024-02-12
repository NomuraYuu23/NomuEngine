#include "Skydome.h"
#include <cassert>
#include "../../Engine/2D/ImguiManager.h"
#include <numbers>

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
	worldTransform_.Initialize();
}

/// <summary>
/// 更新
/// </summary>
void Skydome::Update() {

	worldTransform_.transform_.rotate.y = fmodf(worldTransform_.transform_.rotate.y + rotateSpeed_, static_cast<float>(std::numbers::pi) * 2.0f);

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
