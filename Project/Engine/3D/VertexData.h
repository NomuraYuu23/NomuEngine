#pragma once
#include "../Math/Vector2.h"
#include "../Math/Vector3.h"
#include "../Math/Vector4.h"

struct VertexData {

	Vector4 position;
	Vector2 texcoord;
	Vector3 normal;

	float wegihts[3]; // ボーンの重み(一つ目、二つ目、三つ目、四つ目は (1 - w1 - w2 - w3))
	uint32_t matrixIndex[4]; // ボーン番号

};