#pragma once
#include "MassPoint.h"

class StructuralSpring
{

public:
	
	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="point0">質点0</param>
	/// <param name="point1">質点1</param>
	/// <param name="naturalLength">自然長</param>
	/// <param name="stiffness">剛性。バネ定数k</param>
	/// <param name="dampingCoefficient">減衰係数</param>
	void Initialize(
		const MassPoint& point0,
		const MassPoint& point1,
		float naturalLength,
		float stiffness,
		float dampingCoefficient);

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

public: // アクセッサ

	void SetPoint0(const MassPoint& point0) { point0_ = point0; }
	void SetPoint1(const MassPoint& point1) { point1_ = point1; }
	void SetNaturalLength(float naturalLength) { naturalLength_ = naturalLength; }
	void SetStiffness(float stiffness) { stiffness_ = stiffness; }
	void SetDampingCoefficient(float dampingCoefficient) { dampingCoefficient_ = dampingCoefficient; }
	void SetFixPoint0(bool fixPoint0) { fixPoint0_ = fixPoint0; }
	void SetFixPoint1(bool fixPoint1) { fixPoint1_ = fixPoint1; }

	MassPoint GetPoint0() { return point0_; }
	MassPoint GetPoint1() { return point1_; }
	float GetNaturalLength() { return naturalLength_; }
	float GetStiffness() { return stiffness_; }
	float GetDampingCoefficient() { return dampingCoefficient_; }
	bool GetFixPoint0() { return fixPoint0_; }
	bool GetFixPoint1() { return fixPoint1_; }

private: // 変数

	// 質点0
	MassPoint point0_;
	// 質点1
	MassPoint point1_;
	// 自然長
	float naturalLength_;
	// 剛性。バネ定数k
	float stiffness_;
	// 減衰係数
	float dampingCoefficient_;

	// 質点0を固定するか
	bool fixPoint0_;
	// 質点1を固定するか
	bool fixPoint1_;

};

