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

	Vector3 postureX = { postureMatrix.m[0][0],postureMatrix.m[0][1], postureMatrix.m[0][2] };
	Vector3 postureY = { postureMatrix.m[1][0],postureMatrix.m[1][1], postureMatrix.m[1][2] };
	Vector3 postureZ = { postureMatrix.m[2][0],postureMatrix.m[2][1], postureMatrix.m[2][2] };

	postureX = Vector3::Normalize(Vector3::Cross(angularVelocity, postureX) + postureX);
	postureY = Vector3::Normalize(Vector3::Cross(angularVelocity, postureY) + postureY);
	postureZ = Vector3::Normalize(Vector3::Cross(angularVelocity, postureZ) + postureZ);

	Matrix4x4 result;
	result.m[0][0] = postureX.x;
	result.m[0][1] = postureX.y;
	result.m[0][2] = postureX.z;
	result.m[0][3] = 0.0f;

	result.m[1][0] = postureY.x;
	result.m[1][1] = postureY.y;
	result.m[1][2] = postureY.z;
	result.m[1][3] = 0.0f;

	result.m[2][0] = postureZ.x;
	result.m[2][1] = postureZ.y;
	result.m[2][2] = postureZ.z;
	result.m[2][3] = 0.0f;

	result.m[3][0] = 0.0f;
	result.m[3][1] = 0.0f;
	result.m[3][2] = 0.0f;
	result.m[3][3] = 1.0f;

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

Vector3 RigidBody::PointVelocityCalc(
	const Vector3& angularVelocity,
	const Vector3& centerOfGravityVelocity,
	const Vector3& point,
	const Vector3& centerOfGravity)
{

	// 重心から剛体上の点pointに向かうベクトル
	Vector3 r = Vector3::Subtract(point, centerOfGravity);

	return Vector3::Add(centerOfGravityVelocity, Vector3::Cross(angularVelocity, r));

}

RigidBody::Jacobian RigidBody::JacobianCalc(
	const Vector3& normalize, 
	const Vector3& point, 
	const Vector3& centerOfGravity)
{

	// 重心から剛体上の点pointに向かうベクトル
	Vector3 r = Vector3::Subtract(point, centerOfGravity);
	
	Jacobian result{};

	result.n = normalize;
	result.nR = Vector3::Cross(r, normalize);

	return result;

}

bool RigidBody::RestraintConfirmationJoint(
	const Vector3& velocityA, 
	const Vector3& velocityB)
{

	// 速度が同じなら拘束出来ている
	if (velocityA.x == velocityB.x &&
		velocityA.y == velocityB.y &&
		velocityA.z == velocityB.z) {

		return true;

	}

	return false;

}

bool RigidBody::RestraintConfirmationNoPenetration(
	const Vector3& velocityA, 
	const Vector3& velocityB, 
	const Vector3& normalizeB)
{

	// 速度と法線で内積を出す
	float dot = Vector3::Dot(normalizeB, Vector3::Subtract(velocityA, velocityB));

	// 内積が0より大きいなら拘束出来ている
	if (dot >= 0.0f) {

		return true;

	}

	return false;

}
