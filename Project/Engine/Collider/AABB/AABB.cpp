#include "AABB.h"

void AABB::Initialize(const Vector3& min, const Vector3& max, ColliderParentObject parentObject)
{

	Collider::Initialize(parentObject);

	min_ = min;
	max_ = max;

}

void AABB::worldTransformUpdate()
{

	Vector3Calc* v3Calc = Vector3Calc::GetInstance();

	worldTransform_.transform_.translate = v3Calc->Multiply(0.5f, v3Calc->Add(max_, min_));
	worldTransform_.transform_.scale = v3Calc->Multiply(0.5f, v3Calc->Subtract(max_, min_));
	worldTransform_.UpdateMatrix();

}
