#pragma once
#include "../Collider2D/ColliderShape2D.h"

class Collision2D
{

public: // メンバ関数

	// Box Box
	static bool IsCollision(const Box& box1, const Box& box2);

	// Circle Circle
	static bool IsCollision(const Circle& circle1, const Circle& circle2);

	// Box Circle
	static bool IsCollision(const Box& box, const Circle& circle);
	static bool IsCollision(const Circle& circle, const Box& box);

};

