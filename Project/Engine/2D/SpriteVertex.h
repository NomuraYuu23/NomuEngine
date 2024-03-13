#pragma once
#include "../Math/Vector4.h"
#include "../Math/Vector2.h"
#include "../Math/Vector3.h"

struct SpriteVertex
{
	Vector4 position;
	Vector2 texcoord;
	Vector3 normal;
};