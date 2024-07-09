#include "Octree.h"

// L が辺の長さ (マスの数) (2 べき乗)
uint32_t Octree::massNum_ = 8;

// N が分割数
uint32_t Octree::division_ = 3;

// 元の辺の長さ
float Octree::length_ = 80.0f;

// 中央
Vector3 Octree::center_ = { 0.0f,0.0f,0.0f };

uint32_t Octree::MortonOrder(const Vector3& position)
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

	// 桁数
	float digits = std::log2(static_cast<float>(massNum_));

	// 確認で使う用
	uint32_t check = 1;
	for (uint32_t i = 1; i < static_cast<uint32_t>(digits); ++i) {
		check = check << 1;
	}

	// 
	uint32_t weight = static_cast<uint32_t>(std::powf(2.0f, (3.0f * digits) - 1.0f));

	// リザルト
	uint32_t result = 0;
	for(uint32_t i = 0; i < static_cast<uint32_t>(digits); ++i) {
		
		// z
		result += (z & check) / check * weight;
		weight = weight >> 1;

		// y
		result += (y & check) / check * weight;
		weight = weight >> 1;

		//x
		result += (x & check) / check * weight;
		weight = weight >> 1;

		check = check >> 1;

	}

	return result;

}
