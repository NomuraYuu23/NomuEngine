#include "Quaternion.h"
#include <cmath>

QuaternionCalc* QuaternionCalc::GetInstance()
{
	static QuaternionCalc instance;
	return &instance;
}

Quaternion QuaternionCalc::Add(const Quaternion& q0, const Quaternion& q1)
{

	Quaternion result = {};

	result.x = q0.x + q1.x;
	result.y = q0.y + q1.y;
	result.z = q0.z + q1.z;
	result.w = q0.w + q1.w;

	return result;
}

Quaternion QuaternionCalc::Multiply(const Quaternion& lhs, const Quaternion& rhs)
{

	Quaternion result = {};
	Vector3Calc* v3Calc = Vector3Calc::GetInstance();

	Vector3 q = { lhs.x, lhs.y, lhs.z, };
	Vector3 r = { rhs.x, rhs.y, rhs.z, };
	Vector3 resultVector = v3Calc->Add(v3Calc->Cross(q, r), v3Calc->Add(v3Calc->Multiply(rhs.w, q), v3Calc->Multiply(lhs.w, r)));

	result.x = resultVector.x;
	result.y = resultVector.y;
	result.z = resultVector.z;
	result.w = lhs.w * rhs.w - v3Calc->Dot(q, r);

	return result;
}

Quaternion QuaternionCalc::Multiply(const Quaternion& q, float s)
{

	Quaternion result = {};
	result.x = q.x * s;
	result.y = q.y * s;
	result.z = q.z * s;
	result.w = q.w * s;

	return result;
}

Quaternion QuaternionCalc::Multiply(float s, const Quaternion& q)
{
	return Multiply(q, s);
}

Quaternion QuaternionCalc::IdentityQuaternion()
{
	Quaternion result = { 0.0f,0.0f,0.0f,1.0f };
	return result;
}

Quaternion QuaternionCalc::Conjugate(const Quaternion& quaternion)
{
	Quaternion result = { -quaternion.x, -quaternion.y, -quaternion.z, quaternion.w };
	return result;
}

float QuaternionCalc::Norm(const Quaternion& quaternion)
{
	float result = sqrtf(quaternion.x * quaternion.x +
		quaternion.y * quaternion.y +
		quaternion.z * quaternion.z +
		quaternion.w * quaternion.w);

	return result;
}

Quaternion QuaternionCalc::Normalize(const Quaternion& quaternion)
{
	Quaternion result = {};
	float norm = Norm(quaternion);

	if (quaternion.x != 0.0) {
		result.x = quaternion.x / norm;
	}
	else {
		result.x = 0.0f;
	}

	if (quaternion.y != 0.0) {
		result.y = quaternion.y / norm;
	}
	else {
		result.y = 0.0f;
	}

	if (quaternion.z != 0.0) {
		result.z = quaternion.z / norm;
	}
	else {
		result.z = 0.0f;
	}

	if (quaternion.w != 0.0) {
		result.w = quaternion.w / norm;
	}
	else {
		result.w = 0.0f;
	}

	return result;

}

Quaternion QuaternionCalc::Inverse(const Quaternion& quaternion)
{

	Quaternion result = {};
	Quaternion conjugate = Conjugate(quaternion);

	float norm = Norm(quaternion);

	if (norm != 0.0f) {
		result.x = conjugate.x / (norm * norm);
		result.y = conjugate.y / (norm * norm);
		result.z = conjugate.z / (norm * norm);
		result.w = conjugate.w / (norm * norm);
	}

	return result;
}

Quaternion QuaternionCalc::MakeRotateAxisAngleQuaternion(const Vector3& axis, float angle)
{

	Quaternion result = { };
	Vector3 vector = Vector3Calc::GetInstance()->Multiply(std::sinf(angle / 2.0f), axis);
	result = { vector.x, vector.y, vector.z, std::cosf(angle / 2.0f) };

	return result;

}

Vector3 QuaternionCalc::RotateVector(const Vector3& vector, const Quaternion& quaternion)
{

	Vector3 result = {};

	Quaternion r = { vector.x, vector.y, vector.z, 0.0f };
	Quaternion conjugate = Conjugate(quaternion);
	Quaternion newQuaternion = Multiply(Multiply(quaternion, r), conjugate);

	result.x = newQuaternion.x;
	result.y = newQuaternion.y;
	result.z = newQuaternion.z;

	return result;
}

Matrix4x4 QuaternionCalc::MakeRotateMatrix(const Quaternion& quaternion)
{

	Matrix4x4 result = {};
	float x = quaternion.x;
	float y = quaternion.y;
	float z = quaternion.z;
	float w = quaternion.w;

	result.m[0][0] = std::powf(w, 2.0f) + std::powf(x, 2.0f) - std::powf(y, 2.0f) - std::powf(z, 2.0f);
	result.m[0][1] = 2 * (x * y + w * z);
	result.m[0][2] = 2 * (x * z - w * y);
	result.m[0][3] = 0.0f;
	result.m[1][0] = 2 * (x * y - w * z);
	result.m[1][1] = std::powf(w, 2.0f) - std::powf(x, 2.0f) + std::powf(y, 2.0f) - std::powf(z, 2.0f);
	result.m[1][2] = 2 * (y * z + w * x);
	result.m[1][3] = 0.0f;
	result.m[2][0] = 2 * (x * z + w * y);
	result.m[2][1] = 2 * (y * z - w * x);
	result.m[2][2] = std::powf(w, 2.0f) - std::powf(x, 2.0f) - std::powf(y, 2.0f) + std::powf(z, 2.0f);
	result.m[2][3] = 0.0f;
	result.m[3][0] = 0.0f;
	result.m[3][1] = 0.0f;
	result.m[3][2] = 0.0f;
	result.m[3][3] = 1.0f;

	return result;

}

Quaternion QuaternionCalc::Slerp(const Quaternion& q0, const Quaternion& q1, float t)
{

	float dot = q0.x * q1.x + q0.y * q1.y + q0.z * q1.z + q0.w * q1.w; // q0とq1の内積

	Quaternion use_q0 = q0;

	if (dot < 0.0f) {
		use_q0 = Multiply(q0, -1.0f);
		dot = -dot;
	}

	// なす角を求める
	float theta = std::acosf(dot);

	// thetaとsinを使って補間係数scale0,scale1を求める
	float scale0 = std::sinf((1.0f - t) * theta) / std::sinf(theta);
	float scale1 = std::sinf(t * theta) / std::sinf(theta);

	Quaternion result = Add(Multiply(use_q0, scale0), Multiply(q1, scale1));

	return result;

}
