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
	initMassPoint.position = anchor;
	initMassPoint.mass = mass;
	initMassPoint.acceleration = { 0.0f,0.0f,0.0f };
	initMassPoint.velocity = { 0.0f,0.0f,0.0f };
	initMassPoint.force = { 0.0f,0.0f,0.0f };
	MassPoint initMassPoint1;
	initMassPoint1.position = Vector3::Add(anchor, distance);
	initMassPoint1.mass = mass;
	initMassPoint1.acceleration = { 0.0f,0.0f,0.0f };
	initMassPoint1.velocity = { 0.0f,0.0f,0.0f };
	initMassPoint1.force = { 0.0f,0.0f,0.0f };

	spring_[0].Initialize(
		initMassPoint,
		initMassPoint1,
		naturalLength,
		stiffness,
		dampingCoefficient);

	for (uint32_t i = 1; i < springNum; ++i) {
		initMassPoint.position = spring_[0].GetPoint1().position;
		initMassPoint1.position = Vector3::Add(initMassPoint.position, distance);
		spring_[i].Initialize(
			initMassPoint,
			initMassPoint1,
			naturalLength,
			stiffness,
			dampingCoefficient);
	}


	debugLines_.resize(springNum);
	for (uint32_t i = 0; i < springNum; ++i) {
		debugLines_[i].reset(DrawLine::Create());
	}

}

void String::Update()
{
	//0番目更新
	spring_[0].SetPoint1(spring_[1].GetPoint0());
	spring_[0].Update();

	// それ以外を更新
	for (uint32_t i = 1; i < spring_.size() - 1; ++i) {
		spring_[i].SetPoint0(spring_[i - 1].GetPoint1());
		spring_[i].SetPoint1(spring_[i + 1].GetPoint0());
		spring_[i].Update();
	}

	//最後を更新
	spring_[spring_.size() - 1].SetPoint0(spring_[spring_.size() - 2].GetPoint1());
	spring_[spring_.size() - 1].Update();

}

void String::DebugDraw(BaseCamera& camera)
{

	for (uint32_t i = 0; i < debugLines_.size(); ++i) {
		debugLines_[i]->Draw(
			spring_[i].GetPoint0().position,
			spring_[i].GetPoint1().position,
			Vector4{ 1.0f,1.0f,1.0f,1.0f },
			Vector4{ 0.0f,1.0f,1.0f,1.0f },
			camera);
	}

}

void String::SetAnchor(uint32_t pointIndex, bool fixPoint)
{

	assert(pointIndex < spring_.size() + 1);

	if (pointIndex == 0) {
		spring_[pointIndex].SetFixPoint0(fixPoint);
	}
	else if (pointIndex == spring_.size()) {
		spring_[pointIndex - 1].SetFixPoint1(fixPoint);
	}
	else {
		spring_[pointIndex].SetFixPoint0(fixPoint);
		spring_[pointIndex - 1].SetFixPoint1(fixPoint);
	}

}

void String::SetPosition(uint32_t pointIndex, const Vector3& position)
{

	assert(pointIndex < spring_.size() + 1);

	MassPoint massPoint;

	if (pointIndex == 0) {
		massPoint = spring_[pointIndex].GetPoint0();
		massPoint.position = position;
		spring_[pointIndex].SetPoint0(massPoint);
	}
	else if (pointIndex == spring_.size()) {
		massPoint = spring_[pointIndex - 1].GetPoint1();
		massPoint.position = position;
		spring_[pointIndex - 1].SetPoint1(massPoint);
	}
	else {
		massPoint = spring_[pointIndex].GetPoint0();
		massPoint.position = position;
		spring_[pointIndex].SetPoint0(massPoint);

		massPoint = spring_[pointIndex - 1].GetPoint1();
		massPoint.position = position;
		spring_[pointIndex - 1].SetPoint1(massPoint);
	}

}
