#pragma once
#include "AnimationData.h"

class Animation
{

public:

	/// <summary>
	/// 計算データ
	/// </summary>
	struct AnimationCalcData {

		AnimationData animation; // アニメーションデータ
		bool isRun; // 実行しているか
		double timer; // タイマー

		bool isLoop; // ループか
		bool isFinished; // 終了したか

	};

public:

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="AnimationData">アニメーションデータ</param>
	/// <param name="initPositions">初期位置(ノード数)</param>
	/// <param name="initRotations_">初期回転(ノード数)</param>
	/// <param name="initScalings">初期大きさ(ノード数)</param>
	void Initialize(
		const std::vector<AnimationData>& animationDatas,
		const std::vector<Vector3>& initPositions,
		const std::vector<Quaternion>& initRotations,
		const std::vector<Vector3>& initScalings,
		const std::vector<std::string>& nodeNames);

	/// <summary>
	/// アニメーション
	/// </summary>
	/// <returns></returns>
	std::vector<Matrix4x4> AnimationUpdate();

	/// <summary>
	/// アニメーションを開始させる
	/// </summary>
	/// <param name="animationNum">アニメーション番号</param>
	void startAnimation(uint32_t animationNum, bool isLoop);

	/// <summary>
	/// アニメーションを停止させる(リセット)
	/// </summary>
	/// <param name="animationNum">アニメーション番号</param>
	void stopAnimation(uint32_t animationNum);

	/// <summary>
	/// アニメーションが終了したか
	/// </summary>
	/// <returns></returns>
	std::vector<bool> FinishedAnimations();

private:

	void NodeAnimationUpdate(uint32_t index, double timer);

private:

	// 計算データ
	std::vector<AnimationCalcData> animationDatas_;
	// アニメーション数
	uint32_t animationCalcDataNum_;

	// ノード数
	uint32_t nodeNum_;

	// 位置 初期行列と同じ分だけ
	std::vector <Vector3> positions_;
	// 回転 初期行列と同じ分だけ
	std::vector <Quaternion> rotations_;
	// 大きさ 初期行列と同じ分だけ
	std::vector <Vector3> scalings_;

	// 強制的にすべてのアニメーションが止まった場合
	// 位置 初期行列と同じ分だけ
	std::vector <Vector3> initPositions_;
	// 回転 初期行列と同じ分だけ
	std::vector <Quaternion> initRotations_;
	// 大きさ 初期行列と同じ分だけ
	std::vector <Vector3> initScalings_;

	// アニメーション速度
	double animationSpeed_;

	// ノードの名前
	std::vector<std::string> nodeNames_;

	// 目指す位置 初期行列と同じ分だけ
	std::vector<Vector3> targetPositions_;
	std::vector<uint32_t> positionAddCount_;
	// 目指す回転 初期行列と同じ分だけ
	std::vector<Quaternion> targetRotations_;
	std::vector<uint32_t> rotationAddCount_;
	// 目指す大きさ 初期行列と同じ分だけ
	std::vector<Vector3> targetScalings_;
	std::vector<uint32_t> scalingAddCount_;

};


