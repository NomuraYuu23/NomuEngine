#include "Vector2.h"

Vector2 Vector2::Add(const Vector2& v1, const Vector2& v2)
{

	Vector2 result;

	result = { v1.x + v2.x, v1.y + v2.y };

	return result;

}

Vector2 Vector2::Subtract(const Vector2& v1, const Vector2& v2)
{

	Vector2 result;

	result = { v1.x - v2.x, v1.y - v2.y };

	return result;

}

Vector2 Vector2::Multiply(const Vector2& v, float scalar)
{
	
	Vector2 result;

	result = { scalar * v.x, scalar * v.y };

	return result;

}

float Vector2::Dot(const Vector2& v1, const Vector2& v2)
{

	float result;

	result = v1.x * v2.x + v1.y * v2.y;

	return result;

}

float Vector2::Length(const Vector2& v)
{
	
	float result;

	result = sqrtf(powf(v.x, 2.0f) + powf(v.y, 2.0f));

	return result;

}

Vector2 Vector2::Normalize(const Vector2& v)
{

	Vector2 result;

	float num = Length(v);
	result.x = v.x / num;
	result.y = v.y / num;

	return result;

}

float Vector2::Cross(const Vector2& v1, const Vector2& v2)
{
	
	float result;

	result = v1.x * v2.y - v1.y * v2.x;

	return result;

}

Vector2 Vector2::operator+(const Vector2& v)
{

	Vector2 result = { x, y };

	result = Add(result, v);

	return result;

}

void Vector2::operator+=(const Vector2& v)
{
	
	Vector2 result = { x, y };

	result = Add(result, v);

	x = result.x;
	y = result.y;

}

Vector2 Vector2::operator-(const Vector2& v)
{

	Vector2 result = { x, y };

	result = Subtract(result, v);

	return result;

}

void Vector2::operator-=(const Vector2& v)
{

	Vector2 result = { x, y };

	result = Subtract(result, v);

	x = result.x;
	y = result.y;

}

Vector2 Vector2::operator*(float v)
{

	Vector2 result = { x, y };

	result = Multiply(result, v);

	return result;

}

void Vector2::operator*=(float v)
{

	Vector2 result = { x, y };

	result = Multiply(result, v);

	x = result.x;
	y = result.y;

}
