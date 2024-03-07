#include "massPoint.h"

Vector3 massPoint::AccelerationCalc(
	const Vector3& force,
	float mass)
{

	Vector3 result;
	result = Vector3::Multiply(force,(1.0f / mass));
	return result;

}

Vector3 massPoint::VelocityCalc(
	const Vector3& velocity, 
	const Vector3& acceleration, 
	float time)
{

	Vector3 result;
	result = Vector3::Add(Vector3::Multiply(acceleration, time), velocity);
	return result;

}

Vector3 massPoint::PositionCalc(
	const Vector3& position, 
	const Vector3& velocity, 
	float time)
{

	Vector3 result;
	result = Vector3::Add(Vector3::Multiply(velocity, time), position);
	return result;

}
