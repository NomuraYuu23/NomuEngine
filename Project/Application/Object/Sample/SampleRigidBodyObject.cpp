#include "SampleRigidBodyObject.h"
#include "../../../Engine/Math/DeltaTime.h"
#include "../../../Engine/3D/ModelDraw.h"

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
	const Vector3 force = { 0.0f,0.0f,1.0f };
	rigidBody_.torque = RigidBody::TorqueCalc(rigidBody_.centerOfGravity, pointOfAction, force);

	// 慣性テンソル作成
	rigidBody_.inertiaTensor = InertiaTensor::CreateRectangular(10.0f, Vector3{ 2.0f, 2.0f, 2.0f });

	// 基本姿勢での慣性テンソル作成
	rigidBody_.basicPostureInertiaTensor = InertiaTensor::CreateRectangular(10.0f, Vector3{ 2.0f, 2.0f, 2.0f });
	
	// 姿勢行列作成
	rigidBody_.postureMatrix = Matrix4x4::MakeRotateXYZMatrix({ 0.0f,0.0f,0.0f });

	rigidBody_.angularVelocity = { 0.0f,0.0f,0.0f }; // 角速度
	rigidBody_.angularMomentum = { 0.0f,0.0f,0.0f }; // 角運動量

}

void SampleRigidBodyObject::Update()
{

	rigidBody_.postureMatrix =  RigidBody::PostureCalc(rigidBody_.postureMatrix, rigidBody_.angularVelocity, kDeltaTime_);

	rigidBody_.inertiaTensor = RigidBody::InertiaTensorCalc(rigidBody_.postureMatrix, rigidBody_.basicPostureInertiaTensor);

	rigidBody_.angularMomentum = RigidBody::AngularMomentumCalc(rigidBody_.angularMomentum, rigidBody_.torque, kDeltaTime_);

	rigidBody_.angularVelocity = RigidBody::AngularVelocityCalc(rigidBody_.inertiaTensor, rigidBody_.angularMomentum);

	rigidBody_.torque = { 0.0f,0.0f,0.0f };

	Vector3 velocity = RigidBody::PointVelocityCalc(
		rigidBody_.angularVelocity,
		rigidBody_.centerOfGravityVelocity,
		worldTransform_.GetWorldPosition(),
		rigidBody_.centerOfGravity
	);

	worldTransform_.transform_.translate += velocity;

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
}
