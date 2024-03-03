#pragma once
#include "../Math/Matrix4x4.h"

struct Collider2DDebugDrawForGPU
{

	Matrix4x4 WVP;
	Matrix4x4 World;
	uint32_t textureNum;

};
