#pragma once
#include "NodeAnimationData.h"

class NodeAnimation
{

public:

	/// <summary>
	/// 計算データ
	/// </summary>
	struct NodeAnimationCalcData {

		NodeAnimationData nodeAnimation; // アニメーションデータ
		bool isRun; // 実行しているか
		double timer; // タイマー

		Vector3 position; // 位置
		Quaternion rotation; // 回転
		Vector3 scaling; // 大きさ

		//bool isChangePosition; // 位置変わっているか
		//bool isChangeRotation; // 回転変わっているか
		//bool isChangeScaling; // 大きさ変わっているか

		bool isLoop; // ループか
		bool isFinished; // 終了したか

	};



public:

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="nodeAnimationDatas">アニメーションデータ</param>
	/// <param name="initPositions">初期位置(ノード数)</param>
 	/// <param name="initRotations_">初期回転(ノード数)</param>
 	/// <param name="initScalings">初期大きさ(ノード数)</param>
	void Initialize(
		const std::vector<NodeAnimationData>& nodeAnimationDatas,
		const std::vector<Vector3>& initPositions,
		const std::vector<Quaternion>& initRotations_,
		const std::vector<Vector3>& initScalings );

	/// <summary>
	/// アニメーション
	/// </summary>
	/// <returns></returns>
	std::vector<Matrix4x4> Animation();

	/// <summary>
	/// アニメーションを開始させる
	/// </summary>
	/// <param name="animationNum">アニメーション番号</param>
	void startAnimation(uint32_t animationNum);

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

	// 計算データ
	std::vector<NodeAnimationCalcData> nodeAnimationCalcDatas_;
	// ノードアニメーション数
	uint32_t nodeAnimationCalcDataNum_;

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

	// 何かアニメーションしているか
	bool isAnimation_;

	// アニメーション速度
	double animationSpeed_;

};

