#include "ObjectManager.h"
#include "../../Application/Object/ObjectFactory.h"

void ObjectManager::Initialize(LevelIndex levelIndex, LevelDataManager* levelDataManager)
{

	ObjectFactory* objectFactory = ObjectFactory::GetInstance();

	// レベルデータの取得
	LevelData* levelData = levelDataManager->GetLevelDatas(levelIndex);

	// レベルデータのオブジェクトを走査
	for (std::vector<LevelData::ObjectData>::iterator it = levelData->objectsData_.begin();
		it != levelData->objectsData_.end(); ++it) {

		// オブジェクトの参照
		LevelData::ObjectData objectData = *it;

		// 型にあわせてInitialize
		objects_.emplace_back(nullptr);
		std::unique_ptr<IObject> object;
		object.reset(objectFactory->CreateObject(objectData));

		if (object) {
			// vectorへ
			objects_.push_back(std::move(object));
		}

	}

}

