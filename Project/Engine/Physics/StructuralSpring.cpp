#include "StructuralSpring.h"
#include "../Math/DeltaTime.h"

void StructuralSpring::Initialize(
	const MassPoint& point0, 
	const MassPoint& point1, 
	float naturalLength, 
	float stiffness, 
	float dampingCoefficient)
{

	// 質点0
	point0_ = point0;
	// 質点1
	point1_ = point1;
	// 自然長
	naturalLength_ = naturalLength;
	// 剛性。バネ定数k
	stiffness_ = stiffness;
	// 減衰係数
	dampingCoefficient_ = dampingCoefficient;

	// 質点0を固定するか
	fixPoint0_ = false;
	// 質点1を固定するか
	fixPoint1_ = false;

}

void StructuralSpring::Update()
{

	MassPoint point0 = point0_;
	MassPoint point1 = point1_;
	Vector3 gravity = Vector3{ 0.0f, -9.8f, 0.0f };

	// point0_
	if (!fixPoint0_) {
		Vector3 diff = point0.position - point1.position;
		float length = Vector3::Length(diff);
		if (length != 0.0f) {
			Vector3 direction = Vector3::Normalize(diff);
			Vector3 restPosition = point1.position + direction * naturalLength_;
			Vector3 displacement = Vector3::Multiply(length, (point0.position - restPosition));
			Vector3 restoringForce = Vector3::Multiply(-stiffness_, displacement);
			Vector3 dampingForce = Vector3::Multiply(-dampingCoefficient_, point0.velocity);
			Vector3 force = restoringForce + dampingForce;

			point0_.acceleration = Vector3::Multiply(force, 1.0f / point0_.mass)
				+ gravity * point0_.mass;
		}
		point0_.velocity = point0_.velocity + point0_.acceleration * kDeltaTime_;
	}
	else {
		point0_.velocity = { 0.0f,0.0f,0.0f };
	}

	// point1_
	if (!fixPoint1_) {
		Vector3 diff = point1.position - point0.position;
		float length = Vector3::Length(diff);
		if (length != 0.0f) {
			Vector3 direction = Vector3::Normalize(diff);
			Vector3 restPosition = point0.position + direction * naturalLength_;
			Vector3 displacement = Vector3::Multiply(length, (point1.position - restPosition));
			Vector3 restoringForce = Vector3::Multiply(-stiffness_, displacement);
			Vector3 dampingForce = Vector3::Multiply(-dampingCoefficient_, point1.velocity);
			Vector3 force = restoringForce + dampingForce;

			point1_.acceleration = Vector3::Multiply(force, 1.0f / point1_.mass)
				+ gravity * point1_.mass;
		}
		point1_.velocity = point1_.velocity + point1_.acceleration * kDeltaTime_;
	}
	else {
		point1_.velocity = { 0.0f,0.0f,0.0f };
	}

	// 位置変動
	point0_.position = point0_.position + point0_.velocity * kDeltaTime_;
	point1_.position = point1_.position + point1_.velocity * kDeltaTime_;

}
