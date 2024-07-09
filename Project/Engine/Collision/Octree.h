#pragma once
#include <cstdint>
#include <cmath>
#include "../Math/Vector3.h"
class Octree
{

public:

	// L が辺の長さ (マスの数) (2 べき乗)
	static uint32_t massNum_;

	// N が分割数
	static uint32_t division_;

	// 元の辺の長さ
	static float length_;

	// 中央
	static Vector3 center_;

	/// <summary>
	/// モートンオーダー （一番下の空間を特定）
	/// </summary>
	static uint32_t MortonOrder(const Vector3& position);
	
};

