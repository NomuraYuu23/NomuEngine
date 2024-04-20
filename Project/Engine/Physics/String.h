#pragma once
#include <vector>
#include "Spring.h"
#include "../2D/DrawLine.h"

class String
{

public:

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="anchor">アンカー</param>
	/// <param name="naturalLength">自然長</param>
	/// <param name="stiffness">剛性。バネ定数k</param>
	/// <param name="dampingCoefficient">減衰係数</param>
	/// <param name="mass">質量(質点)</param>
	/// <param name="distance">距離</param>
	/// <param name="springNum">バネの数</param>
	void Initialize(
		const Vector3& anchor,
		float naturalLength,
		float stiffness,
		float dampingCoefficient,
		float mass,
		const Vector3& distance,
		uint32_t springNum);

	/// <summary>
	/// 更新
	/// </summary>
	/// <param name="anchor">アンカー</param>
	void Update(const Vector3& anchor);

	/// <summary>
	/// デバッグ描画
	/// </summary>
	/// <param name="camera">カメラ</param>
	void DebugDraw(BaseCamera& camera);

public: // アクセッサ

	std::vector<Spring> GetSpring() { return spring_; }

private: // 変数

	//バネ
	std::vector<Spring> spring_;

private: // デバッグ

	std::vector<std::unique_ptr<DrawLine>> debugLines_;

};

