#pragma once
#include "TransformStructure.h"
#include "../Math/Matrix4x4.h"

class OneOfManyObjects
{

public:

	/// <summary>
	/// 初期化
	/// </summary>
	virtual void Initialize();

	/// <summary>
	/// 更新
	/// </summary>
	virtual void Update();

	/// <summary>
	/// 死んでいるか
	/// </summary>
	/// <returns></returns>
	bool IsDead() { return isDead_; }

	/// <summary>
	/// ワールドポジション取得
	/// </summary>
	/// <returns></returns>
	Vector3 GetWorldPosition() { return Vector3(worldMatrix_.m[3][0], worldMatrix_.m[3][1], worldMatrix_.m[3][2]); }

public:

	//トランスフォーム
	EulerTransform transform_{ {1.0f, 1.0f, 1.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f} };
	//ワールド行列
	Matrix4x4 worldMatrix_;
	// 回転行列
	Matrix4x4 rotateMatrix_;
	// 方向ベクトルで回転行列
	bool usedDirection_;
	// 方向ベクトル
	Vector3 direction_ = { 0.0f,0.0f,1.0f };

	// 死んでるか
	bool isDead_;

};

