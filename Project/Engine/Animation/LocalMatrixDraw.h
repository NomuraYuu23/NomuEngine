#pragma once
#include "LocalMatrixManager.h"
#include "../3D/WorldTransform.h"
#include "../2D/DrawLine.h"

class LocalMatrixDraw
{

public: // 関数

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="localMatrixManager">ローカル行列マネージャー</param>
	/// <param name="worldTransform">ワールドトランスフォーム</param>
	void Initialize(
		LocalMatrixManager* localMatrixManager,
		WorldTransform* worldTransform);

	/// <summary>
	/// 描画用マップ
	/// </summary>
	/// <param name="drawLine">線描画ポインタ</param>
	void DrawMap(DrawLine* drawLine);

private: // 変数

	// ローカル行列マネージャー
	LocalMatrixManager* localMatrixManager_;

	// ワールドトランスフォーム
	WorldTransform* worldTransform_;

};

