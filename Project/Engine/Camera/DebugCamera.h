#pragma once
#include "BaseCamera.h"

class DebugCamera : public BaseCamera
{

public:

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize() override;

	/// <summary>
	/// 更新
	/// </summary>
	void Update() override;

	/// <summary>
	/// ImGui描画
	/// </summary>
	void ImGuiDraw();

private:

	// 移動速度
	float moveSpeed_;
	// 回転速度
	float rotateSpeed_;

};

