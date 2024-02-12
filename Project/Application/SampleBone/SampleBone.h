#pragma once
#include "../../Engine/Animation/IBone.h"
class SampleBone {

public:

	void Initialize(Model* model);

	void Update();

	void Draw(BaseCamera& camera);

private:

	std::unique_ptr<IBone> bone_;

	Model* model_ = nullptr;
	std::string objectName_;
	std::string partName_;
	std::vector<std::string> motionNames_;

};

