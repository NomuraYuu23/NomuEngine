#include "Collision2D.h"

bool Collision2D::IsCollision(const Box& box1, const Box& box2)
{

	if ((box1.right_ >= box2.left_) &&
		(box1.left_ <= box2.right_) &&
		(box1.bottom_ >= box2.top_) &&
		(box1.top_ <= box2.bottom_)) {

		return true;

	}


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

	// 四角形の4辺に対して円の半径分だけ足したとき円が重なっていたら
	if ((circle.position_.x >= box.left_ - circle.radius_) &&
		(circle.position_.x <= box.right_ + circle.radius_) &&
		(circle.position_.y >= box.top_ - circle.radius_) &&
		(circle.position_.y <= box.bottom_ + circle.radius_)) {

		result = true;
		Vector2 boxPoint = { 0.0f, 0.0f };

		// 左
		if (circle.position_.x < box.left_) {

			boxPoint.x = box.left_;

			// 左上
			if (circle.position_.y < box.top_) {

				boxPoint.y = box.top_;

				if (Vector2::Length(boxPoint - circle.position_) >= circle.radius_) {
					result = false;
				}

			}
			// 左下
			else if(circle.position_.y > box.bottom_){

				boxPoint.y = box.bottom_;

				if (Vector2::Length(boxPoint - circle.position_) >= circle.radius_) {
					result = false;
				}

			}

		}
		else if (circle.position_.x > box.right_) {

			boxPoint.x = box.right_;

			// 右上
			if (circle.position_.y < box.top_) {

				boxPoint.y = box.top_;

				if (Vector2::Length(boxPoint - circle.position_) >= circle.radius_) {
					result = false;
				}

			}
			// 右下
			else if (circle.position_.y > box.bottom_) {

				boxPoint.y = box.bottom_;

				if (Vector2::Length(boxPoint - circle.position_) >= circle.radius_) {
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
