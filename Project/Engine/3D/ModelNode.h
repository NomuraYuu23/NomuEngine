#pragma once
#include "../Math/Matrix4x4.h"
#include <string>
#include <vector>

class ModelNode
{

public:

	Matrix4x4 localMatrix;
	std::string name;
	std::vector<ModelNode> children;
	uint32_t meshNum;

};

