#pragma once
#include "MassPoint.h"
#include "../Math/Matrix4x4.h"
#include "../Math/Quaternion.h"

class RigidBody
{

public: // 変数

	MassPoint massPoint; // 質点

	Vector3 torque;	// トルク(ひねり力)
	
	Matrix4x4 inertiaTensor; // 慣性テンソル
	Matrix4x4 basicPostureInertiaTensor; // 基本姿勢での慣性テンソル
	
	Matrix4x4 postureMatrix; // 姿勢行列
	Vector3 posture; // 姿勢

	Vector3 angularVelocity; // 角速度
	//Quaternion angularAcceleration; // 角加速度
	Vector3 angularMomentum; // 角運動量

public: // 関数

	/// <summary>
	/// トルク(ひねり力)計算
	/// </summary>
	/// <param name="centerOfGravity">重心</param>
	/// <param name="pointOfAction">作用点</param>
	/// <param name="force">力</param>
	/// <returns>トルク(ひねり力)</returns>
	static Vector3 TorqueCalc(
		const Vector3& centerOfGravity,
		const Vector3& pointOfAction,
		const Vector3& force);

	/// <summary>
	/// 慣性テンソル計算
	/// </summary>
	/// <param name="postureMatrix">姿勢行列</param>
	/// <param name="basicPostureInertiaTensor">基本姿勢での慣性テンソル</param>
	/// <returns></returns>
	static Matrix4x4 InertiaTensorCalc(
		const Matrix4x4& postureMatrix,
		const Matrix4x4& basicPostureInertiaTensor);

	// 姿勢の更新
	static Matrix4x4 PostureCalc(
		const Matrix4x4& postureMatrix,
		const Vector3& angularVelocity,
		float time);

	// 角運動量の計算
	static Vector3 AngularMomentumCalc(
		const Vector3& angularMomentum,
		const Vector3& torque,
		float time);

	// 角速度の更新
	static Vector3 AngularVelocityCalc(
		const Matrix4x4& inertiaTensor,
		const Vector3& angularMomentum);

};

