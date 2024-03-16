#include "Sphere.h"

void Sphere::Initialize(const Vector3& center, float radius, ColliderParentObject parentObject)
{

	Collider::Initialize(parentObject);

	center_ = center; //中心点
	radius_ = radius;   //半径

}

void Sphere::worldTransformUpdate()
{

	worldTransform_.SetTranslate(center_);
	worldTransform_.SetScale({ radius_, radius_, radius_ });
	worldTransform_.UpdateMatrix();

}
