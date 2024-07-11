#include "Octree.h"
#include <cmath>

void Octree::Initialize(uint32_t massNum, uint32_t division, float length, Vector3 center)
{

	// L が辺の長さ (マスの数) (2 べき乗)
	massNum_ = massNum;

	// N が分割数
	division_ = division;

	// 元の辺の長さ
	length_ = length;

	// 中央
	center_ = center;

}

int32_t Octree::BitSeparateFor3D(int32_t n)
{

    int32_t result = n;

    result = (result | (result << 8) & 0x0000f00f);
    result = (result | (result << 4) & 0x000c30c3);
    result = (result | (result << 2) & 0x00249249);

    return result;
}

int32_t Octree::Get3DMortonOrder(int32_t x, int32_t y, int32_t z)
{
    return BitSeparateFor3D(x) | BitSeparateFor3D(y) | BitSeparateFor3D(z);
}

Vector3 Octree::OctreeSpaceTransformation(const Vector3& position)
{

	if (position.x >= center_.x + length_ / 2.0f ||
		position.y >= center_.y + length_ / 2.0f ||
		position.z >= center_.z + length_ / 2.0f ||

		position.x < center_.x - length_ / 2.0f ||
		position.y < center_.y - length_ / 2.0f ||
		position.z < center_.z - length_ / 2.0f) {

		assert(0);

	}

	// マスの一辺
	float mass = length_ / static_cast<float>(massNum_);

	// 切り捨て
	int32_t x = static_cast<int32_t>((position.x - center_.x + length_ / 2.0f) / mass);
	int32_t y = static_cast<int32_t>((position.y - center_.y + length_ / 2.0f) / mass);
	int32_t z = static_cast<int32_t>((position.z - center_.z + length_ / 2.0f) / mass);

	Vector3 result = { static_cast<float>(x), static_cast<float>(y), static_cast<float>(z)};

    return result;

}

int32_t Octree::BelongingSpace(int32_t lt, int32_t rb)
{
	
	int32_t XOR = lt ^ rb;

	// 空間レベル
	int32_t level = 0;

	// その空間の所属番号
	int32_t num = 0;

	int32_t i = 0;

	while (XOR != 0x00) {

		if ((XOR & 0x7) != 0) {
			level = division_ - i;
		}

		i++;

		XOR >>= 3;

	}

	num = lt >> (division_ - level) * 3;

	int32_t add = static_cast<int32_t>((std::powf(8.0f, static_cast<float>(level)) - 1) / 7.0f);

	num += add;

	return num;

}

