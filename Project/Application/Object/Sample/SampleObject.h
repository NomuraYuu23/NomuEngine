#pragma once
#include "../../../Engine/3D/Model.h"

class SampleObject
{

public:

	~SampleObject();

	void Initialize(Model* model);

	void Update();

	void Draw(BaseCamera camera);

	void ImGuiDraw();

private:

	/// <summary>
	/// 外部変数登録
	/// </summary>
	virtual void RegisteringGlobalVariables();

	/// <summary>
	/// 外部変数適用
	/// </summary>
	virtual void ApplyGlobalVariables();

private:

	// モデル
	Model* model_ = nullptr;
	// マテリアル
	std::unique_ptr<Material> material_ = nullptr;
	// ワールドトランスフォーム
	WorldTransform worldtransform_;

	// 
	int32_t enableLighting_;

	float shininess_;

};

