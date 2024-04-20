#include "String.h"
#include <cassert>

void String::Initialize(
	const Vector3& anchor, 
	float naturalLength, 
	float stiffness, 
	float dampingCoefficient, 
	float mass, 
	const Vector3& distance, 
	uint32_t springNum)
{

	assert(springNum > 0);

	spring_.resize(springNum);

	MassPoint initMassPoint;
	initMassPoint.position = Vector3::Add(anchor, distance);
	initMassPoint.mass = mass;

	initMassPoint.acceleration = { 0.0f,0.0f,0.0f };
	initMassPoint.velocity = { 0.0f,0.0f,0.0f };
	initMassPoint.force = { 0.0f,0.0f,0.0f };

	spring_[0].Initialize(
		anchor,
		naturalLength,
		stiffness,
		dampingCoefficient,
		initMassPoint);

	for (uint32_t i = 1; i < springNum; ++i) {
		initMassPoint.position = Vector3::Add(spring_[i - 1].GetMassPoint().position, distance);
		spring_[i].Initialize(
			spring_[i - 1].GetMassPoint().position,
			naturalLength,
			stiffness,
			dampingCoefficient,
			initMassPoint);
	}

	debugLines_.resize(springNum);
	for (uint32_t i = 0; i < springNum; ++i) {
		debugLines_[i].reset(DrawLine::Create());
	}

}

void String::Update(const Vector3& anchor)
{

	// 一番上を更新
	spring_[0].SetAnchor(anchor);
	spring_[0].Update();

	// それ以外を更新
	for (uint32_t i = 1; i < spring_.size(); ++i) {
		spring_[i].SetAnchor(spring_[i - 1].GetMassPoint().position);
		spring_[i].Update();
	}

}

void String::DebugDraw(BaseCamera& camera)
{

	for (uint32_t i = 0; i < debugLines_.size(); ++i) {
		debugLines_[i]->Draw(
			spring_[i].GetAnchor(),
			spring_[i].GetMassPoint().position,
			Vector4{ 1.0f,1.0f,1.0f,1.0f },
			Vector4{ 1.0f,1.0f,1.0f,1.0f },
			camera);
	}

}
