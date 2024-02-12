#include "DebugCamera.h"
#include "../Input/Input.h"
#include "../2D/ImguiManager.h"

/// <summary>
/// 初期化
/// </summary>
void DebugCamera::Initialize() {

	BaseCamera::Initialize();

	moveSpeed_ = 0.1f;
	rotateSpeed_ = 0.01f;

}

/// <summary>
/// 更新
/// </summary>
void DebugCamera::Update() {

	Input* input = Input::GetInstance();
	
	if (input->PushKey(DIK_D)) {
		transform_.translate.x += moveSpeed_;
	}
	if (input->PushKey(DIK_A)) {
		transform_.translate.x -= moveSpeed_;
	}
	if (input->PushKey(DIK_W)) {
		transform_.translate.y += moveSpeed_;
	}
	if (input->PushKey(DIK_S)) {
		transform_.translate.y -= moveSpeed_;
	}
	if (input->PushKey(DIK_E)) {
		transform_.translate.z += moveSpeed_;
	}
	if (input->PushKey(DIK_Q)) {
		transform_.translate.z -= moveSpeed_;
	}

	if (input->PushKey(DIK_RIGHT)) {
		transform_.rotate.y += rotateSpeed_;
	}
	if (input->PushKey(DIK_LEFT)) {
		transform_.rotate.y -= rotateSpeed_;
	}
	if (input->PushKey(DIK_DOWN)) {
		transform_.rotate.x += rotateSpeed_;
	}
	if (input->PushKey(DIK_UP)) {
		transform_.rotate.x -= rotateSpeed_;
	}

	BaseCamera::Update();

}

void DebugCamera::ImGuiDraw()
{

	ImGui::Begin("DebugCamera");
	ImGui::DragFloat3("translate",&transform_.translate.x,0.01f);
	ImGui::DragFloat3("rotate", &transform_.rotate.x, 0.01f);
	ImGui::End();

}
