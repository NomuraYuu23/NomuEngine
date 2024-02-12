#include "IParticle.h"
#include "../base/BufferResource.h"
#include "../base/WinApp.h"
#include "../base/DirectXCommon.h"
#include "ParticleManager.h"
#include <cassert>
#include <numbers>
#include <random>
#include "../Math/DeltaTime.h"

IParticle::~IParticle(){}

void IParticle::Initialize(const Vector3& position, const Vector3& size)
{

	Vector3Calc* vector3Calc = Vector3Calc::GetInstance();
	Matrix4x4Calc* matrix4x4Calc = Matrix4x4Calc::GetInstance();
	std::random_device seedGenerator;
	std::mt19937 randomEngine(seedGenerator());

	Vector3 distributionMax = { position.x + size.x / 2.0f,
								position.y + size.y / 2.0f,
								position.z + size.z / 2.0f };
	Vector3 distributionMin = { position.x - size.x / 2.0f,
								position.y - size.y / 2.0f,
								position.z - size.z / 2.0f };
	std::uniform_real_distribution<float> distributionX(distributionMin.x, distributionMax.x);
	std::uniform_real_distribution<float> distributionY(distributionMin.y, distributionMax.y);
	std::uniform_real_distribution<float> distributionZ(distributionMin.z, distributionMax.z);
	std::uniform_real_distribution<float> distColor(0.0f, 1.0f);
	std::uniform_real_distribution<float> distTime(1.0f, 3.0f);

	transform_.scale = {1.0f,1.0f,1.0f};
	transform_.rotate = { 0.0f,0.0f,0.0f };
	transform_.translate = { distributionX(randomEngine), distributionY(randomEngine), distributionZ(randomEngine) };

	worldMatrix_ = matrix4x4Calc->MakeIdentity4x4();

	color_ = { distColor(randomEngine),distColor(randomEngine),distColor(randomEngine), 1.0f };

	lifeTime_ = distTime(randomEngine);

	currentTime_ = 0.0f;

	useBillBoard_ = true;

	billBoardName_ = kBillBoardNameIndexAllAxis;

	UpdateMatrix(matrix4x4Calc->MakeIdentity4x4());

	isDead_ = false;

}

void IParticle::Update(const Matrix4x4& billBoardMatrix)
{
	if (lifeTime_ <= currentTime_) {
		isDead_ = true;
	}
	TimeElapsed();
	GraduallyDisappear();
	UpdateMatrix(billBoardMatrix);

}

void IParticle::UpdateMatrix(const Matrix4x4& billBoardMatrix)
{

	Matrix4x4Calc* matrix4x4Calc = Matrix4x4Calc::GetInstance();

	if (useBillBoard_) {
		Matrix4x4 scaleMatrix = matrix4x4Calc->MakeScaleMatrix(transform_.scale);
		Matrix4x4 translateMatrix = matrix4x4Calc->MakeTranslateMatrix(transform_.translate);
		worldMatrix_ = matrix4x4Calc->Multiply(scaleMatrix, matrix4x4Calc->Multiply(billBoardMatrix, translateMatrix));
	}
	else {
		worldMatrix_ = matrix4x4Calc->MakeAffineMatrix(transform_.scale, transform_.rotate, transform_.translate);
	}
}

ParticleForGPU IParticle::Map(const Matrix4x4& viewProjectionMatrix)
{

	Matrix4x4Calc* matrix4x4Calc = Matrix4x4Calc::GetInstance();
	ParticleManager* particleManager = ParticleManager::GetInstance();

	ParticleForGPU particleForGPU;
	particleForGPU.World = worldMatrix_;
	particleForGPU.WVP = matrix4x4Calc->Multiply(worldMatrix_, viewProjectionMatrix);
	particleForGPU.color = color_;

	return particleForGPU;

}

void IParticle::TimeElapsed()
{

	currentTime_ += kDeltaTime_;
	if (currentTime_ >= lifeTime_) {
		currentTime_ = lifeTime_;
	}

}

void IParticle::GraduallyDisappear()
{

	color_.w = 1.0f - (currentTime_ / lifeTime_);

}