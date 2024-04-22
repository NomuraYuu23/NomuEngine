#include "Animation.h"
#include "../Math/DeltaTime.h"
#include "../Math/Ease.h"

void Animation::Initialize(
	const std::vector<AnimationData>& animationDatas,
	const std::vector<QuaternionTransform>& initTransform,
	const std::vector<std::string>& nodeNames)
{

	// アニメーションデータ
	animationCalcDataNum_ = static_cast<uint32_t>(animationDatas.size());
	animationDatas_.resize(animationDatas.size());
	for (uint32_t i = 0; i < animationCalcDataNum_; ++i) {
		animationDatas_[i].animation = animationDatas[i];
		animationDatas_[i].isRun = false;
		animationDatas_[i].timer = 0.0;

		animationDatas_[i].isLoop = false;
		animationDatas_[i].isFinished = false;

	}

	// 初期データ
	nodeNum_ = static_cast<uint32_t>(initTransform.size());

	// 計算データ
	positions_.resize(initTransform.size());
	rotations_.resize(initTransform.size());
	scalings_.resize(initTransform.size());
	for (uint32_t i = 0; i < nodeNum_; ++i) {
		positions_[i] = initTransform[i].translate;
		rotations_[i] = initTransform[i].rotate;
		scalings_[i] = initTransform[i].scale;
	}

	// その他
	animationSpeed_ = static_cast<double>(kDeltaTime_);
	nodeNames_ = nodeNames;

}

std::vector<Matrix4x4> Animation::AnimationUpdate()
{

	std::vector<Matrix4x4> result;
	result.resize(nodeNum_);
	for (uint32_t i = 0; i < nodeNum_; ++i) {
		result[i] = Matrix4x4::MakeIdentity4x4();
	}

	// 位置 初期行列と同じ分だけ
	targetPositions_.clear();
	targetPositions_.resize(nodeNum_);
	positionAddCount_.clear();
	positionAddCount_.resize(nodeNum_);
	// 回転 初期行列と同じ分だけ
	targetRotations_.clear();
	targetRotations_.resize(nodeNum_);
	rotationAddCount_.clear();
	rotationAddCount_.resize(nodeNum_);
	// 大きさ 初期行列と同じ分だけ
	targetScalings_.clear();
	targetScalings_.resize(nodeNum_);
	scalingAddCount_.clear();
	scalingAddCount_.resize(nodeNum_);

	for (uint32_t i = 0; i < nodeNum_; ++i) {
		positionAddCount_[i] = 0;
		rotationAddCount_[i] = 0;
		scalingAddCount_[i] = 0;
	}

	for (uint32_t i = 0; i < animationCalcDataNum_; ++i) {

		animationDatas_[i].isFinished = false;

		// アニメーションするか
		if (animationDatas_[i].isRun) {
			// タイマーを進める
			animationDatas_[i].timer += animationSpeed_;

			// タイマー超えてるか
			if (animationDatas_[i].animation.endTime_ < animationDatas_[i].timer) {
				// ループ
				if (animationDatas_[i].isLoop) {
					animationDatas_[i].timer -= animationDatas_[i].animation.endTime_;
				}
				// 終了
				else {
					animationDatas_[i].isFinished = true;
					animationDatas_[i].isRun = false;
				}
			}
			if (!animationDatas_[i].isFinished) {

				// ノードアニメーション分アップデート
				NodeAnimationUpdate(i , animationDatas_[i].timer);

			}

		}
	}

	// 目標値の設定、現在値を確定、行列へ
	for (uint32_t i = 0; i < nodeNum_; ++i) {

		// 目標値 

		// カウントされている
		if (positionAddCount_[i] != 0) {
			targetPositions_[i] *= (1.0f / positionAddCount_[i]);
		}
		else {
			targetPositions_[i] = positions_[i];
		}

		// カウントされている
		if (rotationAddCount_[i] != 0) {
			targetRotations_[i] *= (1.0f / rotationAddCount_[i]);
		}
		else {
			targetRotations_[i] = rotations_[i];
		}

		// カウントされている
		if (scalingAddCount_[i] != 0) {
			targetScalings_[i] *= (1.0f / scalingAddCount_[i]);
		}
		else {
			targetScalings_[i] = scalings_[i];
		}

		positions_[i] = Ease::Easing(Ease::EaseName::Lerp, positions_[i], targetPositions_[i], moveT_);
		rotations_[i] = Quaternion::Slerp(rotations_[i], targetRotations_[i], moveT_);
		scalings_[i] = Ease::Easing(Ease::EaseName::Lerp, scalings_[i], targetScalings_[i], moveT_);

		// 行列
		result[i] = Matrix4x4::MakeAffineMatrix(scalings_[i], rotations_[i], positions_[i]);

	}

	return result;
}

void Animation::StartAnimation(uint32_t animationNum, bool isLoop)
{

	animationDatas_[animationNum].isRun = true;
	animationDatas_[animationNum].timer = 0.0;
	animationDatas_[animationNum].isLoop = isLoop;

}

void Animation::StopAnimation(uint32_t animationNum)
{

	animationDatas_[animationNum].isRun = false;

}

std::vector<bool> Animation::FinishedAnimations()
{
	std::vector<bool> result;

	for (uint32_t i = 0; i < animationCalcDataNum_; ++i) {
		result.push_back(animationDatas_[i].isFinished);
	}

	return result;
}

std::vector<bool> Animation::GetRunningAnimations()
{
	
	std::vector<bool> result;

	for (uint32_t i = 0; i < animationCalcDataNum_; ++i) {
		result.push_back(animationDatas_[i].isRun);
	}

	return result;

}

void Animation::NodeAnimationUpdate(uint32_t index, double timer)
{

	Vector3 position; // 位置
	Quaternion rotation; // 回転
	Vector3 scaling; // 大きさ

	// アニメーション
	for (uint32_t i = 0; i < animationDatas_[index].animation.nodeAnimationNum_ ; ++i) {

		NodeAnimationData data = animationDatas_[index].animation.nodeAnimationDatas_[i];

		// どのデータを持ってくるか
		// 位置
		
		// データが一つしかない
		if (data.positionKeyNum_ == 1) {
			// 値
			position = data.positions_[0].value_;
			// ノードの名前がヒット
			uint32_t name = 0;
			for (uint32_t k = 0; k < nodeNames_.size(); ++k) {
				if (nodeNames_[k] == data.nodeName_) {
					name = k;
				}
			}
			targetPositions_[name] += position;
			positionAddCount_[name]++;
		}
		// データが一つ以上
		else {
			for (uint32_t j = 1; j < data.positionKeyNum_; ++j) {
				if (data.positions_[j].time_ > timer) {
					// 補間係数
					float t = static_cast<float>(
						(timer -
							data.positions_[j - 1].time_) /
						(data.positions_[j].time_ -
							data.positions_[j - 1].time_));
					// 値
					position =
						Ease::Easing(Ease::EaseName::Lerp,
							data.positions_[j - 1].value_,
							data.positions_[j].value_,
							t);

					// ノードの名前がヒット
					uint32_t name = 0;
					for (uint32_t k = 0; k < nodeNames_.size(); ++k) {
						if (nodeNames_[k] == data.nodeName_) {
							name = k;
						}
					}
					targetPositions_[name] += position;
					positionAddCount_[name]++;
					break;
				}
			}
		}

		// 回転

		// データが一つしかない
		if (data.rotationKeyNum_ == 1) {
			// 値
			rotation = data.rotations_[0].value_;
			// ノードの名前がヒット
			uint32_t name = 0;
			for (uint32_t k = 0; k < nodeNames_.size(); ++k) {
				if (nodeNames_[k] == data.nodeName_) {
					name = k;
				}
			}
			targetRotations_[name] += rotation;
			rotationAddCount_[name]++;
		}
		// データが一つ以上
		else {
			for (uint32_t j = 1; j < data.rotationKeyNum_; ++j) {
				if (data.rotations_[j].time_ > timer) {
					// 補間係数
					float t = static_cast<float>(
						(timer -
							data.rotations_[j - 1].time_) /
						(data.rotations_[j].time_ -
							data.rotations_[j - 1].time_));
					// 値
					rotation =
						Quaternion::Slerp(
							data.rotations_[j - 1].value_,
							data.rotations_[j].value_,
							t);

					// ノードの名前がヒット
					uint32_t name = 0;
					for (uint32_t k = 0; k < nodeNames_.size(); ++k) {
						if (nodeNames_[k] == data.nodeName_) {
							name = k;
						}
					}

					targetRotations_[name] += rotation;
					rotationAddCount_[name]++;
					break;
				}
			}
		}

		// 大きさ
		
		// データが一つしかない
		if (data.scalingKeyNum_ == 1) {
			// 値
			scaling = data.scalings_[0].value_;
			// ノードの名前がヒット
			uint32_t name = 0;
			for (uint32_t k = 0; k < nodeNames_.size(); ++k) {
				if (nodeNames_[k] == data.nodeName_) {
					name = k;
				}
			}
			targetScalings_[name] += scaling;
			scalingAddCount_[name]++;
		}
		// データが一つ以上
		else {
			for (uint32_t j = 1; j < data.scalingKeyNum_; ++j) {
				if (data.scalings_[j].time_ > timer) {

					// 補間係数
					float t = static_cast<float>(
						(timer -
							data.scalings_[j - 1].time_) /
						(data.scalings_[j].time_ -
							data.scalings_[j - 1].time_));
					// 値
					scaling =
						Ease::Easing(Ease::EaseName::Lerp,
							data.scalings_[j - 1].value_,
							data.scalings_[j].value_,
							t);

					// ノードの名前がヒット
					uint32_t name = 0;
					for (uint32_t k = 0; k < nodeNames_.size(); ++k) {
						if (nodeNames_[k] == data.nodeName_) {
							name = k;
						}
					}
					targetScalings_[name] += scaling;
					scalingAddCount_[name]++;
					break;
				}
			}
		}

	}

}
