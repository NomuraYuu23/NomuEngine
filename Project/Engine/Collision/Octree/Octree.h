#pragma once
#include <cstdint>
#include "../../Math/Vector3.h"

class Octree
{

private:

	// マスの数 (2 べき乗)
	uint32_t massNum_;

	// N が分割数
	uint32_t division_;

	// 元の辺の長さ
	float length_;

	// 中央
	Vector3 center_;

public:


	void Initialize(uint32_t massNum, uint32_t division, float length, Vector3 center);

	int32_t BitSeparateFor3D(int32_t n);

	int32_t Get3DMortonOrder(int32_t x, int32_t y, int32_t z);

	Vector3 OctreeSpaceTransformation(const Vector3& position);

	int32_t BelongingSpace(int32_t lt, int32_t rb);

};

