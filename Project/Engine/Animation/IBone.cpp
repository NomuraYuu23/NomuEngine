#include "IBone.h"
#include "../Math/Ease.h"
#include "../../externals/nlohmann/json.hpp"
#include <fstream>

AnimationFile* IBone::animationFile_ = AnimationFile::GetInstance();

void IBone::Initialize(Model* model, const std::string& objectName, const std::string& partName, const std::vector<std::string>& motionNames)
{

	model_ = model;

	objectName_ = objectName;

	partName_ = partName;

	worldTransform_.Initialize();

	PreRegistrationAnimationFile();

	RegistrationAnimationFile();

	ApplyAnimationFile(motionNames);

}

void IBone::Update(uint32_t frameCount)
{

	Animation(frameCount);

	worldTransform_.UpdateMatrix();

}

void IBone::Update(uint32_t frameCount, const std::vector<std::string>& motionNames)
{

	//return;

	Animation(frameCount);

	worldTransform_.UpdateMatrix();

#ifdef _DEBUG

	ApplyAnimationFile(motionNames);

#endif // _DEBUG

}

void IBone::Draw(BaseCamera& camera)
{

	model_->Draw(worldTransform_, camera);

}

void IBone::Draw(BaseCamera& camera, Material* material)
{

	model_->Draw(worldTransform_, camera, material);

}

void IBone::Animation(uint32_t frameCount)
{

	BoneData start = {};
	BoneData end = {};
	float t = 0.0f;
	uint32_t currentFrame = 0u;
	uint32_t endFrame = 0u;

	for (uint32_t i = 0; i < animationTransforms_.size(); i++) {
		if (animationTransforms_[i].endFrame_ >= frameCount) {
			// アニメーショントランスフォーム
			if (i == 0) {
				start = animationTransforms_[0];
			}
			else {
				start = animationTransforms_[i - 1];
			}
			end = animationTransforms_[i];
			break;
		}
	}

	//フレームカウント
	endFrame = end.endFrame_ - start.endFrame_;
	currentFrame = frameCount - start.endFrame_;
	if (endFrame != 0) {
		t = static_cast<float>(currentFrame) / static_cast<float>(endFrame);
	}

	// イージング
	worldTransform_.transform_.scale = Ease::Easing(static_cast<Ease::EaseName>(end.easeType_), start.transform_.scale, end.transform_.scale, t);
	worldTransform_.transform_.rotate = Ease::Easing(static_cast<Ease::EaseName>(end.easeType_), start.transform_.rotate, end.transform_.rotate, t);
	worldTransform_.transform_.translate = Ease::Easing(static_cast<Ease::EaseName>(end.easeType_), start.transform_.translate, end.transform_.translate, t);

}

void IBone::PreRegistrationAnimationFile()
{

	//グループを追加
	animationFile_->CreatePart(objectName_, partName_);

}

void IBone::RegistrationAnimationFile()
{

	for (std::map<std::string, std::vector<BoneData>>::iterator itAnimationMap = animationTransformDatas_.begin(); itAnimationMap != animationTransformDatas_.end(); ++itAnimationMap) {
		const std::string motionName = itAnimationMap->first;
		std::vector<BoneData>& motionData = itAnimationMap->second;
		animationFile_->AddMotion(objectName_, partName_, motionName, motionData);
	}

}

void IBone::ApplyAnimationFile(const std::vector<std::string>& motionNames)
{

	for (size_t i = 0; i < motionNames.size(); i++)
	{
		animationTransformDatas_[motionNames[i]] = animationFile_->GetValue(objectName_, partName_, motionNames[i]);
	}

}

uint32_t IBone::animationTransformChange(const std::string& motionName)
{

	//return 0;

	//各グループについて
	for (std::map<std::string, std::vector<BoneData>>::iterator itData = animationTransformDatas_.begin(); itData != animationTransformDatas_.end();
		++itData) {
		// グループ名を取得
		const std::string& dataKey = itData->first;
		if (motionName == dataKey) {
			// グループの参照を取得
			animationTransforms_.clear();
			animationTransforms_.shrink_to_fit();
			animationTransforms_ = itData->second;
			break;
		}
	}

	return (--animationTransforms_.end())->endFrame_;

}
