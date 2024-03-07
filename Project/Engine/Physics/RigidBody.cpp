#include "RigidBody.h"

Vector3 RigidBody::TorqueCalc(
	const Vector3& centerOfGravity,
	const Vector3& pointOfAction,
	const Vector3& force)
{

	Vector3 result;
	Vector3 r = Vector3::Subtract(pointOfAction, centerOfGravity);
	result = Vector3::Cross(r, force);
	return result;

}

Matrix4x4 RigidBody::InertiaTensorCalc(
	const Matrix4x4& postureMatrix,
	const Matrix4x4& basicPostureInertiaTensor)
{

	Matrix4x4 result;

	Matrix4x4 transposePostureMatrix = Matrix4x4::Transpose(postureMatrix);

	result = Matrix4x4::Multiply(Matrix4x4::Multiply(postureMatrix, basicPostureInertiaTensor), transposePostureMatrix);

	return result;
}

Matrix4x4 RigidBody::PostureCalc(
	const Matrix4x4& postureMatrix,
	const Vector3& angularVelocity,
	float time)
{

	Vector3 postureX = { postureMatrix.m[0][0],postureMatrix.m[1][0], postureMatrix.m[2][0] };
	Vector3 postureY = { postureMatrix.m[0][1],postureMatrix.m[1][1], postureMatrix.m[2][1] };
	Vector3 postureZ = { postureMatrix.m[0][2],postureMatrix.m[1][2], postureMatrix.m[2][2] };

	postureX = Vector3::Cross(angularVelocity, postureX);
	postureY = Vector3::Cross(angularVelocity, postureY);
	postureZ = Vector3::Cross(angularVelocity, postureZ);

	Matrix4x4 result;
	result.m[0][0] = postureX.x;
	result.m[0][1] = postureY.x;
	result.m[0][2] = postureZ.x;
	result.m[0][3] = 0.0f;

	result.m[1][0] = postureX.y;
	result.m[1][1] = postureY.y;
	result.m[1][2] = postureZ.y;
	result.m[1][3] = 0.0f;

	result.m[2][0] = postureX.z;
	result.m[2][1] = postureY.z;
	result.m[2][2] = postureZ.z;
	result.m[2][3] = 0.0f;

	result.m[3][0] = 0.0f;
	result.m[3][1] = 0.0f;
	result.m[3][2] = 0.0f;
	result.m[3][3] = 1.0f;

	result = Matrix4x4::Add(result, postureMatrix);

	return result;

}

Vector3 RigidBody::AngularMomentumCalc(
	const Vector3& angularMomentum,
	const Vector3& torque,
	float time)
{

	Vector3 result;
	result = Vector3::Add(angularMomentum, Vector3::Multiply(torque, time));
	return result;

}

Vector3 RigidBody::AngularVelocityCalc(
	const Matrix4x4& inertiaTensor,
	const Vector3& angularMomentum)
{

	Vector3 result;
	result = Matrix4x4::TransformNormal(angularMomentum, Matrix4x4::Inverse(inertiaTensor));
	return result;

}
