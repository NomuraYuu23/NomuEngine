#include "NodeAnimation.h"
#include "../Math/DeltaTime.h"
#include "../Math/Ease.h"

void NodeAnimation::Initialize(
	const std::vector<NodeAnimationData>& nodeAnimationDatas,
	const std::vector<Vector3>& initPositions,
	const std::vector<Quaternion>& initRotations,
	const std::vector<Vector3>& initScalings)
{

	// アニメーションデータ
	nodeAnimationCalcDataNum_ = static_cast<uint32_t>(nodeAnimationDatas.size());
	nodeAnimationCalcDatas_.resize(nodeAnimationDatas.size());
	for (uint32_t i = 0; i < nodeAnimationCalcDataNum_; ++i) {
		nodeAnimationCalcDatas_[i].nodeAnimation = nodeAnimationDatas[i];
		nodeAnimationCalcDatas_[i].isRun = false;
		nodeAnimationCalcDatas_[i].timer = 0.0;

		nodeAnimationCalcDatas_[i].position = { 0.0f, 0.0f, 0.0f };
		nodeAnimationCalcDatas_[i].rotation = { 0.0f, 0.0f, 0.0f, 0.0f };
		nodeAnimationCalcDatas_[i].scaling = { 1.0f, 1.0f, 1.0f };

		nodeAnimationCalcDatas_[i].isLoop = false;
		nodeAnimationCalcDatas_[i].isFinished = false;

	}

	// 初期データ
	initPositions_ = initPositions;
	initRotations_ = initRotations;
	initScalings_ = initScalings;
	nodeNum_ = static_cast<uint32_t>(initPositions_.size());

	// 計算データ
	positions_.resize(initPositions_.size());
	rotations_.resize(initPositions_.size());
	scalings_.resize(initPositions_.size());
	for (uint32_t i = 0; i < nodeNum_; ++i) {
		positions_[i] = initPositions_[i];
		rotations_[i] = initRotations_[i];
		scalings_[i] = initScalings_[i];
	}

	// その他
	animationSpeed_ = static_cast<double>(kDeltaTime_);

}

std::vector<Matrix4x4> NodeAnimation::Animation()
{

	std::vector<Matrix4x4> result;
	result.resize(nodeNum_);

	// 位置 初期行列と同じ分だけ
	std::vector<Vector3> targetPositions;
	targetPositions.resize(nodeNum_);
	std::vector<uint32_t> positionAddCount;
	// 回転 初期行列と同じ分だけ
	std::vector<Quaternion> targetRotations;
	targetRotations.resize(nodeNum_);
	std::vector<uint32_t> rotationAddCount;
	// 大きさ 初期行列と同じ分だけ
	std::vector<Vector3> targetScalings;
	targetScalings.resize(nodeNum_);
	std::vector<uint32_t> scalingAddCount;

	for (uint32_t i = 0; i < nodeNum_; ++i) {
		positionAddCount[i] = 0;
		rotationAddCount[i] = 0;
		scalingAddCount[i] = 0;
	}

	// アニメーション
	for (uint32_t i = 0; i < nodeAnimationCalcDataNum_; ++i) {
		
		nodeAnimationCalcDatas_[i].isFinished = false;

		// アニメーションするか
		if (nodeAnimationCalcDatas_[i].isRun) {
			// タイマーを進める
			nodeAnimationCalcDatas_[i].timer += animationSpeed_;

			// タイマー超えてるか
			if (nodeAnimationCalcDatas_[i].nodeAnimation.endTime_ < nodeAnimationCalcDatas_[i].timer) {
				// ループ
				if (nodeAnimationCalcDatas_[i].isLoop) {
					nodeAnimationCalcDatas_[i].timer -= nodeAnimationCalcDatas_[i].nodeAnimation.endTime_;
				}
				// 終了
				else {
					nodeAnimationCalcDatas_[i].isFinished = true;
				}
			}
			if (!nodeAnimationCalcDatas_[i].isFinished) {

				// どのデータを持ってくるか
				// 位置
				for (uint32_t j = 0; i < nodeAnimationCalcDatas_[i].nodeAnimation.positionKeyNum_; ++j) {
					if (nodeAnimationCalcDatas_[i].nodeAnimation.positions_[j].time_ > nodeAnimationCalcDatas_[i].timer) {
						// 補間係数
						float t = static_cast<float>(
							(nodeAnimationCalcDatas_[i].timer -
							nodeAnimationCalcDatas_[i].nodeAnimation.positions_[j - 1].time_) /
							(nodeAnimationCalcDatas_[i].nodeAnimation.positions_[j].time_ -
								nodeAnimationCalcDatas_[i].nodeAnimation.positions_[j - 1].time_) );
						// 値
						nodeAnimationCalcDatas_[i].position = 
							Ease::Easing(Ease::EaseName::Lerp,
								nodeAnimationCalcDatas_[i].nodeAnimation.positions_[j - 1].value_,
								nodeAnimationCalcDatas_[i].nodeAnimation.positions_[j].value_,
								t);

						// ノードの名前がヒット
						uint32_t name = 0;
						targetPositions[name] += nodeAnimationCalcDatas_[i].position;
						positionAddCount[name]++;
						break;
					}
				}

				// 回転
				for (uint32_t j = 0; i < nodeAnimationCalcDatas_[i].nodeAnimation.rotationKeyNum_; ++j) {
					if (nodeAnimationCalcDatas_[i].nodeAnimation.rotations_[j].time_ > nodeAnimationCalcDatas_[i].timer) {
						nodeAnimationCalcDatas_[i].rotation = nodeAnimationCalcDatas_[i].nodeAnimation.rotations_[j].value_;
						// 補間係数
						float t = static_cast<float>(
							(nodeAnimationCalcDatas_[i].timer -
							nodeAnimationCalcDatas_[i].nodeAnimation.rotations_[j - 1].time_) /
							(nodeAnimationCalcDatas_[i].nodeAnimation.rotations_[j].time_ -
								nodeAnimationCalcDatas_[i].nodeAnimation.rotations_[j - 1].time_) );
						// 値
						nodeAnimationCalcDatas_[i].rotation =
							Quaternion::Slerp(
								nodeAnimationCalcDatas_[i].nodeAnimation.rotations_[j - 1].value_,
								nodeAnimationCalcDatas_[i].nodeAnimation.rotations_[j].value_,
								t);

						// ノードの名前がヒット
						uint32_t name = 0;
						targetRotations[name] *= nodeAnimationCalcDatas_[i].rotation;
						rotationAddCount[name]++;
						break;
					}
				}

				// 大きさ
				for (uint32_t j = 0; i < nodeAnimationCalcDatas_[i].nodeAnimation.scalingKeyNum_; ++j) {
					if (nodeAnimationCalcDatas_[i].nodeAnimation.scalings_[j].time_ > nodeAnimationCalcDatas_[i].timer) {
						nodeAnimationCalcDatas_[i].scaling = nodeAnimationCalcDatas_[i].nodeAnimation.scalings_[j].value_;

						// 補間係数
						float t = static_cast<float>(
							(nodeAnimationCalcDatas_[i].timer -
							nodeAnimationCalcDatas_[i].nodeAnimation.scalings_[j - 1].time_) /
							(nodeAnimationCalcDatas_[i].nodeAnimation.scalings_[j].time_ -
								nodeAnimationCalcDatas_[i].nodeAnimation.scalings_[j - 1].time_) );
						// 値
						nodeAnimationCalcDatas_[i].scaling =
							Ease::Easing(Ease::EaseName::Lerp,
								nodeAnimationCalcDatas_[i].nodeAnimation.scalings_[j - 1].value_,
								nodeAnimationCalcDatas_[i].nodeAnimation.scalings_[j].value_,
								t);

						
						// ノードの名前がヒット
						uint32_t name = 0;
						targetScalings[name] += nodeAnimationCalcDatas_[i].scaling;
						scalingAddCount[name]++;
						break;
					}
				}

			}

		}
	}

	// 目標値の設定、現在値を確定、行列へ
	for (uint32_t i = 0; i < nodeNum_; ++i) {

		// 目標値 
		
		// カウントされている
		if (positionAddCount[i] != 0) {
			targetPositions[i] *= (1.0f / positionAddCount[i]);
		}
		else {
			targetPositions[i] = initPositions_[i];
		}

		// カウントされている
		if (rotationAddCount[i] == 0) {
			targetRotations[i] = initRotations_[i];
		}

		// カウントされている
		if (scalingAddCount[i] != 0) {
			targetScalings[i] *= (1.0f / scalingAddCount[i]);
		}
		else {
			targetScalings[i] = initScalings_[i];
		}

		// 現在値
		positions_[i] = targetPositions[i];
		rotations_[i] = targetRotations[i];
		scalings_[i] = targetScalings[i];

		// 行列
		result[i] = Matrix4x4::MakeAffineMatrix(scalings_[i], rotations_[i], positions_[i]);

	}

	return result;

}

void NodeAnimation::startAnimation(uint32_t animationNum, bool isLoop)
{

	// ノードの名前がヒット
	uint32_t name = 0;

	nodeAnimationCalcDatas_[animationNum].isRun = true;
	nodeAnimationCalcDatas_[animationNum].position = initPositions_[name];
	nodeAnimationCalcDatas_[animationNum].rotation = initRotations_[name];
	nodeAnimationCalcDatas_[animationNum].scaling = initScalings_[name];
	nodeAnimationCalcDatas_[animationNum].timer = 0.0;
	nodeAnimationCalcDatas_[animationNum].isLoop = isLoop;

}

void NodeAnimation::stopAnimation(uint32_t animationNum)
{

	nodeAnimationCalcDatas_[animationNum].isRun = false;

}

std::vector<bool> NodeAnimation::FinishedAnimations()
{

	std::vector<bool> result;

	for (uint32_t i = 0; i < nodeAnimationCalcDataNum_; ++i) {
		result.push_back(nodeAnimationCalcDatas_[i].isFinished);
	}

	return result;
}
