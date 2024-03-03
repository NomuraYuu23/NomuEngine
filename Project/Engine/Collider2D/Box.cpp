#include "Box.h"

void Box::Initialize(const Vector2& center, float width, float height, ColliderParentObject2D parentObject)
{

	Collider2D::Initialize(parentObject);

	position_ = center;
	scale_.x = width;
	scale_.y = height;

	left_ = position_.x - scale_.x / 2.0f;
	top_ = position_.y - scale_.y / 2.0f;
	right_ = position_.x + scale_.x / 2.0f;
	bottom_ = position_.y + scale_.y / 2.0f;

}

void Box::Update(const Vector2& center, float width, float height)
{

	position_ = center;
	scale_.x = width;
	scale_.y = height;

	left_ = position_.x - scale_.x / 2.0f;
	top_ = position_.y - scale_.y / 2.0f;
	right_ = position_.x + scale_.x / 2.0f;
	bottom_ = position_.y + scale_.y / 2.0f;

}
