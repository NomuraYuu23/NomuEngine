#pragma once

#include "IObject.h"
#include "../Camera/BaseCamera.h"

#include <vector>
#include "../Level/LevelIndex.h"
#include "../Level/LevelDataManager.h"

class ObjectManager
{

public:

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="levelIndex">レベル番号</param>
	/// <param name="levelDataManager">レベルデータマネージャー</param>
	void Initialize(LevelIndex levelIndex, LevelDataManager* levelDataManager);

private:

	// オブジェクト
	std::vector<std::unique_ptr<IObject>> objects_{};

};

