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

		bool isLoop; // ループか
		bool isFinished; // 終了したか

	};

public:

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="nodeAnimationDatas">アニメーションデータ</param>
	/// <param name="initMatrix">初期行列(ノード数)</param>
	void Initialize(
		const std::vector<NodeAnimationData>& nodeAnimationDatas,
		const std::vector<Matrix4x4>& initMatrix);

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

	// 初期データ
	std::vector<Matrix4x4> initMatrix_;

	// 位置 初期行列と同じ分だけ
	std::vector <Vector3> positions_;
	// 回転 初期行列と同じ分だけ
	std::vector <Quaternion> rotations_;
	// 大きさ 初期行列と同じ分だけ
	std::vector <Vector3> scalings_;

};

