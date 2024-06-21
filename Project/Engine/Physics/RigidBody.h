#pragma once
#include "MassPoint.h"
#include "../Math/Matrix4x4.h"
#include "../Math/Quaternion.h"

class RigidBody
{

	/// <summary>
	/// ヤコビアン(拘束力の方向)
	/// </summary>
	struct Jacobian
	{
		Vector3 n; // 接触点での法線
		Vector3 nR; // 接触点での法線と重心から剛体上の点pに向かうベクトルのクロス積
	};

public: // 変数

	MassPoint massPoint; // 質点

	Vector3 torque;	// トルク(ひねり力)
	
	Matrix4x4 inertiaTensor; // 慣性テンソル
	Matrix4x4 basicPostureInertiaTensor; // 基本姿勢での慣性テンソル
	
	Matrix4x4 postureMatrix; // 姿勢行列

	Vector3 angularVelocity; // 角速度
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
	/// 慣性テンソル更新
	/// </summary>
	/// <param name="postureMatrix">姿勢行列</param>
	/// <param name="basicPostureInertiaTensor">基本姿勢での慣性テンソル</param>
	/// <returns>慣性テンソル</returns>
	static Matrix4x4 InertiaTensorCalc(
		const Matrix4x4& postureMatrix,
		const Matrix4x4& basicPostureInertiaTensor);

	/// <summary>
	/// 姿勢の更新
	/// </summary>
	/// <param name="postureMatrix">姿勢行列</param>
	/// <param name="angularVelocity">角速度</param>
	/// <param name="time">時間(delta)</param>
	/// <returns>姿勢行列</returns>
	static Matrix4x4 PostureCalc(
		const Matrix4x4& postureMatrix,
		const Vector3& angularVelocity,
		float time);

	/// <summary>
	/// 角運動量の更新
	/// </summary>
	/// <param name="angularMomentum">角運動量</param>
	/// <param name="torque">トルク(ひねり力)</param>
	/// <param name="time">時間(delta)</param>
	/// <returns>angularMomentum</returns>
	static Vector3 AngularMomentumCalc(
		const Vector3& angularMomentum,
		const Vector3& torque,
		float time);

	/// <summary>
	/// 角速度の更新
	/// </summary>
	/// <param name="inertiaTensor">慣性テンソル</param>
	/// <param name="angularMomentum">角運動量</param>
	/// <returns>角速度</returns>
	static Vector3 AngularVelocityCalc(
		const Matrix4x4& inertiaTensor,
		const Vector3& angularMomentum);

	/// <summary>
	/// 剛体上の点の速度を求める
	/// </summary>
	/// <param name="angularVelocity">角速度</param>
	/// <param name="centerOfGravityVelocity">重心位置の速度</param>
	/// <param name="point">剛体上の点</param>
	/// <param name="centerOfGravity">重心位置</param>
	/// <returns>剛体上の点の速度</returns>
	static Vector3 PointVelocityCalc(
		const Vector3& angularVelocity,
		const Vector3& centerOfGravityVelocity,
		const Vector3& point,
		const Vector3& centerOfGravity);

	/// <summary>
	/// ヤコビアンを求める
	/// </summary>
	/// <param name="normalize">法線</param>
	/// <param name="point">点</param>
	/// <param name="centerOfGravity">重心</param>
	/// <returns></returns>
	static Jacobian JacobianCalc(
		const Vector3& normalize,
		const Vector3& point,
		const Vector3& centerOfGravity);

public: // 確認用関数

	/// <summary>
	/// 拘束表現確認(関節)
	/// </summary>
	/// <param name="velocityA">物体Aの速度</param>
	/// <param name="velocityB">物体Bの速度</param>
	/// <returns></returns>
	bool RestraintConfirmationJoint(
		const Vector3& velocityA, 
		const Vector3& velocityB);

	/// <summary>
	/// 拘束表現確認(貫通無し)
	/// </summary>
	/// <param name="velocityA">物体Aの速度</param>
	/// <param name="velocityB">物体Bの速度</param>
	/// <param name="normalizeB">接触点での物体Bの法線</param>
	/// <returns></returns>
	bool RestraintConfirmationNoPenetration(
		const Vector3& velocityA,
		const Vector3& velocityB,
		const Vector3& normalizeB);

};

