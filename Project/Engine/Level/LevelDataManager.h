#pragma once

#include "LevelData.h"
#include "LevelDataLoader.h"

class LevelDataManager
{

public: // サブクラス

	/// <summary>
	/// シーンの種類
	/// </summary>
	enum SceneIndex {
		kSceneIndexSample, // サンプル
		kSceneIndexOfCount // 数を数える用
	};

public: //  関数

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

public: // アクセッサ

	/// <summary>
	/// データゲッター
	/// </summary>
	/// <param name="index">番号</param>
	/// <returns>データ</returns>
	LevelData* GetLevelDatas(SceneIndex index) { return levelDatas_[index].get(); }

private: // 定数

	// ファイルの名前
	const std::array<const std::string, SceneIndex::kSceneIndexOfCount> fileNames_{
		"test",
	};

private: // 変数

	// 格納データ群
	std::array<std::unique_ptr<LevelData>, SceneIndex::kSceneIndexOfCount> levelDatas_{};

};

