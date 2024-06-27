#include "SampleRigidBodyObject.h"
#include "../../../Engine/Math/DeltaTime.h"
#include "../../../Engine/3D/ModelDraw.h"
#include "../../../Engine/2D/ImguiManager.h"

SampleRigidBodyObject::~SampleRigidBodyObject()
{
}

void SampleRigidBodyObject::Initialize(Model* model)
{

	model_ = model;

	material_.reset(Material::Create());

	worldTransform_.Initialize(model_->GetRootNode());

	localMatrixManager_ = std::make_unique<LocalMatrixManager>();
	localMatrixManager_->Initialize(model_->GetRootNode());

	rigidBody_.centerOfGravityVelocity = { 0.0f,0.0f,0.0f }; // 重心位置速度
	// 重心位置 
	rigidBody_.centerOfGravity = worldTransform_.GetWorldPosition() + Vector3{ 0.0f, 1.0f, 1.0f };

	// 力を入れる
	const Vector3 pointOfAction = worldTransform_.GetWorldPosition() + Vector3{ 0.0f, -1.0f, -1.0f };
	const Vector3 force = { 0.0f,0.0f, 500.0f };
	rigidBody_.torque = RigidBody::TorqueCalc(rigidBody_.centerOfGravity, pointOfAction, force);

	// 慣性テンソル作成
	rigidBody_.inertiaTensor = InertiaTensor::CreateRectangular(0.01f, Vector3{ 2.0f, 2.0f, 2.0f });

	// 基本姿勢での慣性テンソル作成
	rigidBody_.basicPostureInertiaTensor = InertiaTensor::CreateRectangular(0.01f, Vector3{ 2.0f, 2.0f, 2.0f });
	
	// 姿勢行列作成
	rigidBody_.postureMatrix = Matrix4x4::MakeRotateXYZMatrix({ 0.0f,0.0f,0.0f });

	rigidBody_.angularVelocity = { 0.0f,0.0f,0.0f }; // 角速度
	rigidBody_.angularMomentum = { 0.0f,0.0f,0.0f }; // 角運動量

}

void SampleRigidBodyObject::Update()
{

	// 速度算出
	Vector3 velocity = RigidBody::PointVelocityCalc(
		rigidBody_.angularVelocity,
		rigidBody_.centerOfGravityVelocity,
		worldTransform_.GetWorldPosition(),
		rigidBody_.centerOfGravity
	);

	// 速度更新
	worldTransform_.transform_.translate += velocity * kDeltaTime_;

	// 姿勢行列を更新
	rigidBody_.postureMatrix =  RigidBody::PostureCalc(rigidBody_.postureMatrix, rigidBody_.angularVelocity, kDeltaTime_);

	// 慣性テンソルを更新
	rigidBody_.inertiaTensor = RigidBody::InertiaTensorCalc(rigidBody_.postureMatrix, rigidBody_.basicPostureInertiaTensor);

	// 角運動量を更新
	rigidBody_.angularMomentum = RigidBody::AngularMomentumCalc(rigidBody_.angularMomentum, rigidBody_.torque, kDeltaTime_);

	// 角速度を更新
	rigidBody_.angularVelocity = RigidBody::AngularVelocityCalc(rigidBody_.inertiaTensor, rigidBody_.angularMomentum);

	// ひねり力を0に
	rigidBody_.torque = { 0.0f,0.0f,0.0f };

	// ワールドトランスフォーム更新
	worldTransform_.UpdateMatrix(rigidBody_.postureMatrix);

}

void SampleRigidBodyObject::Draw(BaseCamera& camera)
{

	ModelDraw::AnimObjectDesc desc;
	desc.camera = &camera;
	desc.localMatrixManager = localMatrixManager_.get();
	desc.material = material_.get();
	desc.model = model_;
	desc.worldTransform = &worldTransform_;
	ModelDraw::AnimObjectDraw(desc);

}

void SampleRigidBodyObject::ImGuiDraw()
{

	ImGui::Begin("SampleRigidBodyObject");
	ImGui::Text("angularMomentum X::%f, Y::%f, Z::%f", rigidBody_.angularMomentum.x, rigidBody_.angularMomentum.y, rigidBody_.angularMomentum.z);
	ImGui::Text("angularVelocity X::%f, Y::%f, Z::%f", rigidBody_.angularVelocity.x, rigidBody_.angularVelocity.y, rigidBody_.angularVelocity.z);
	
	ImGui::Text("postureMatrix0 X::%f, Y::%f, Z::%f", rigidBody_.postureMatrix.m[0][0], rigidBody_.postureMatrix.m[0][1], rigidBody_.postureMatrix.m[0][2]);
	ImGui::Text("postureMatrix1 X::%f, Y::%f, Z::%f", rigidBody_.postureMatrix.m[1][0], rigidBody_.postureMatrix.m[1][1], rigidBody_.postureMatrix.m[1][2]);
	ImGui::Text("postureMatrix2 X::%f, Y::%f, Z::%f", rigidBody_.postureMatrix.m[2][0], rigidBody_.postureMatrix.m[2][1], rigidBody_.postureMatrix.m[2][2]);
	ImGui::End();


}
