#pragma once
#include "Vector3.h"
#include "Matrix4x4.h"

struct Quaternion
{
	float x;
	float y;
	float z;
	float w;

};

class QuaternionCalc
{
public:

	/// <summary>
	/// シングルトンインスタンスの取得
	/// </summary>
	/// <returns></returns>
	static QuaternionCalc* GetInstance();

	static Quaternion Add(const Quaternion& q0, const Quaternion& q1);

	static Quaternion Multiply(const Quaternion& lhs, const Quaternion& rhs);

	static Quaternion Multiply(const Quaternion& q, float s);

	static Quaternion Multiply(float s, const Quaternion& q);

	static Quaternion IdentityQuaternion();

	static Quaternion Conjugate(const Quaternion& quaternion);

	static float Norm(const Quaternion& quaternion);

	static Quaternion Normalize(const Quaternion& quaternion);

	static Quaternion Inverse(const Quaternion& quaternion);

	static Quaternion MakeRotateAxisAngleQuaternion(const Vector3& axis, float angle);

	static Vector3 RotateVector(const Vector3& vector, const Quaternion& quaternion);

	static Matrix4x4 MakeRotateMatrix(const Quaternion& quaternion);

	static Quaternion Slerp(const Quaternion& q0, const Quaternion& q1, float t);

private:
	QuaternionCalc() = default;
	~QuaternionCalc() = default;
	QuaternionCalc(const QuaternionCalc&) = delete;
	QuaternionCalc& operator=(const QuaternionCalc&) = delete;

};
