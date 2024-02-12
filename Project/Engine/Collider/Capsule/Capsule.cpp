#include "Capsule.h"

void Capsule::Initialize(const Segment& segment, float radius, ColliderParentObject parentObject)
{

	Collider::Initialize(parentObject);

	segment_ = segment;
	radius_ = radius;

}

void Capsule::worldTransformUpdate()
{

	Vector3Calc* v3Calc = Vector3Calc::GetInstance();

	worldTransform_.transform_.translate = v3Calc->Multiply(0.5f, v3Calc->Add(segment_.origin_, v3Calc->Multiply(0.5f, segment_.diff_)));
	worldTransform_.transform_.scale = v3Calc->Multiply(0.5f, segment_.diff_);
	worldTransform_.UpdateMatrix();

}
