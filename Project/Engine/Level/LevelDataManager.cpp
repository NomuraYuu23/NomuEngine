#include "LevelDataManager.h"

void LevelDataManager::Initialize()
{

	// ローダーの初期化
	LevelDataLoader::Initialize();

	// データ格納
	for (uint32_t i = 0; i < SceneIndex::kSceneIndexOfCount; ++i) {
		levelDatas_[i].reset(LevelDataLoader::Load(fileNames_[i]));
	}

}
