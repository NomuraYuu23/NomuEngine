#include "Collision2D.h"
#include "../Math/Matrix3x3.h"
#include <numbers>

bool Collision2D::IsCollision(const Box& box1, const Box& box2)
{

	//if ((box1.right_ >= box2.left_) &&
	//	(box1.left_ <= box2.right_) &&
	//	(box1.bottom_ >= box2.top_) &&
	//	(box1.top_ <= box2.bottom_)) {

	//	return true;

	//}


	return false;

}

bool Collision2D::IsCollision(const Circle& circle1, const Circle& circle2)
{

	float d = Vector2::Length(Vector2::Subtract(circle1.position_, circle2.position_));

	float r = circle1.radius_ + circle2.radius_;

	if (d <= r) {

		return true;

	}

	return false;

}

bool Collision2D::IsCollision(const Box& box, const Circle& circle)
{
	
	bool result = false;

	float thata = box.rotation_ * static_cast<float>(std::numbers::pi) / 180.0f;
	Matrix3x3 boxWorldMatrix = Matrix3x3::MakeRotateMatrix(thata);
	boxWorldMatrix = Matrix3x3::Multiply(boxWorldMatrix, Matrix3x3::MakeTranslateMatrix(box.position_));

	Matrix3x3 boxWorldMatrixInverse = Matrix3x3::Inverse(boxWorldMatrix);

	Vector2 centerInBoxLocalSpace = Matrix3x3::Transform(circle.position_, boxWorldMatrixInverse);

	// 基準の矩形
	float left = -box.scale_.x / 2.0f;
	float right = box.scale_.x / 2.0f;
	float top = -box.scale_.y / 2.0f;
	float bottom = box.scale_.y / 2.0f;

	// 矩形に対応した円
	Circle newCircre;
	newCircre.position_ = centerInBoxLocalSpace;
	newCircre.radius_ = circle.radius_;

	// 四角形の4辺に対して円の半径分だけ足したとき円が重なっていたら
	if ((newCircre.position_.x >= left - newCircre.radius_) &&
		(newCircre.position_.x <= right + newCircre.radius_) &&
		(newCircre.position_.y >= top - newCircre.radius_) &&
		(newCircre.position_.y <= bottom + newCircre.radius_)) {

		result = true;
		Vector2 boxPoint = { 0.0f, 0.0f };

		// 左
		if (newCircre.position_.x < left) {

			boxPoint.x = left;

			// 左上
			if (newCircre.position_.y < top) {

				boxPoint.y = top;

				if (Vector2::Length(boxPoint - newCircre.position_) >= newCircre.radius_) {
					result = false;
				}

			}
			// 左下
			else if(newCircre.position_.y > bottom){

				boxPoint.y = bottom;

				if (Vector2::Length(boxPoint - newCircre.position_) >= newCircre.radius_) {
					result = false;
				}

			}

		}
		else if (newCircre.position_.x > right) {

			boxPoint.x = right;

			// 右上
			if (newCircre.position_.y < top) {

				boxPoint.y = top;

				if (Vector2::Length(boxPoint - newCircre.position_) >= newCircre.radius_) {
					result = false;
				}

			}
			// 右下
			else if (newCircre.position_.y > bottom) {

				boxPoint.y = bottom;

				if (Vector2::Length(boxPoint - newCircre.position_) >= newCircre.radius_) {
					result = false;
				}

			}

		}

	}

	return result;

}

bool Collision2D::IsCollision(const Circle& circle, const Box& box)
{
	return IsCollision(box, circle);
}
