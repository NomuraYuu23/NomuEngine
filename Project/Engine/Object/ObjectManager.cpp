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
			objects_.emplace_back(object->GetName(), std::move(object));
		}

	}

	// コライダーのデバッグ描画
	colliderDebugDraw_ = std::make_unique<ColliderDebugDraw>();
	colliderDebugDraw_->Initialize();

}

void ObjectManager::Update()
{

#ifdef _DEBUG

	// リストをクリア
	colliderDebugDraw_->ListClear();

#endif // _DEBUG

	for (std::vector<ObjectPair>::iterator it = objects_.begin();
		it != objects_.end(); ++it) {
		it->second->Update();
	}

}

void ObjectManager::Draw(BaseCamera& camera)
{

	for (std::vector<ObjectPair>::iterator it = objects_.begin();
		it != objects_.end(); ++it) {
		static_cast<MeshObject*>(it->second.get())->Draw(camera);
	}

}

void ObjectManager::Draw(BaseCamera& camera, DrawLine* drawLine)
{

	Draw(camera);

	// コライダーのデバッグ描画
	colliderDebugDraw_->DrawMap(drawLine);

}

void ObjectManager::ImGuiDraw()
{

	colliderDebugDraw_->ImGuiDraw();

}

IObject* ObjectManager::GetObjectPointer(const std::string name)
{

	IObject* result = nullptr;

	for (std::vector<ObjectPair>::iterator it = objects_.begin();
		it != objects_.end(); ++it) {

		if (it->first == name) {
			result = it->second.get();
			return result;
		}

	}

	return nullptr;

}

void ObjectManager::CollisionListRegister(CollisionManager* collisionManager)
{

	bool isDebug = false;

#ifdef _DEBUG
	isDebug = true;
#endif // _DEBUG

	for (std::vector<ObjectPair>::iterator it = objects_.begin();
		it != objects_.end(); ++it) {

		if (isDebug) {
			static_cast<MeshObject*>(it->second.get())->CollisionListRegister(collisionManager, colliderDebugDraw_.get());
		}
		else {
			static_cast<MeshObject*>(it->second.get())->CollisionListRegister(collisionManager);
		}

	}

}

