#pragma once

#include "../../../Engine/3D/Model.h"
#include "../../../Engine/Physics/RigidBody.h"
#include "../../../Engine/Physics/InertiaTensor.h"

class SampleRigidBodyObject
{

public:

	~SampleRigidBodyObject();

	void Initialize(Model* model);

	void Update();

	void Draw(BaseCamera& camera);

	void ImGuiDraw();

private:

	// モデル
	Model* model_ = nullptr;
	// マテリアル
	std::unique_ptr<Material> material_ = nullptr;
	// ワールドトランスフォーム
	WorldTransform worldTransform_{};

	// ローカル行列
	std::unique_ptr<LocalMatrixManager> localMatrixManager_ = nullptr;

	// 剛体
	RigidBody rigidBody_{};

};

