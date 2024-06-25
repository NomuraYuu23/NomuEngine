#include "ObjectManager.h"
#include "../../Application/Object/ObjectFactory.h"
#include "MeshObject.h"

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
		std::unique_ptr<IObject> object;
		object.reset(objectFactory->CreateObject(objectData));

		if (object) {
			// vectorへ
			objects_.push_back(std::move(object));
		}

	}

}

void ObjectManager::Update()
{

	for (std::vector<std::unique_ptr<IObject>>::iterator it = objects_.begin();
		it != objects_.end(); ++it) {
		it->get()->Update();
	}

}

void ObjectManager::Draw(BaseCamera& camera)
{

	for (std::vector<std::unique_ptr<IObject>>::iterator it = objects_.begin();
		it != objects_.end(); ++it) {
		static_cast<MeshObject*>(it->get())->Draw(camera);
	}

}

