#pragma once

#include "../Math/Vector3.h"
#include "../Physics/massPoint.h"

class Spring {

public: 

	// 初期化
	void initialize(
		const Vector3& anchor,
		float naturalLength,
		float stiffness,
		float dampingCoefficient,
		const MassPoint& massPoint);

	void Update();

	void SetAnchor(const Vector3& anchor) { anchor_ = anchor; }
	void SetNaturalLength(float naturalLength) { naturalLength_ = naturalLength; }
	void SetStiffness(float stiffness) { stiffness_ = stiffness; }
	void SetDampingCoefficient(float dampingCoefficient) { dampingCoefficient_ = dampingCoefficient; }

private:

	//アンカー。固定された端の位置
	Vector3 anchor_;
	float naturalLength_; // 自然長
	float stiffness_; //剛性。バネ定数k
	float dampingCoefficient_; //減衰係数

	MassPoint massPoint_; // 質点

};