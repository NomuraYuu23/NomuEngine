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

	for (std::vector<std::unique_ptr<IObject>>::iterator it = objects_.begin();
		it != objects_.end(); ++it) {
		it->get()->Update();

#ifdef _DEBUG

		// コライダー登録
		colliderDebugDraw_->AddCollider(*static_cast<MeshObject*>(it->get())->GetCollider());

#endif // _DEBUG

	}

}

void ObjectManager::Draw(BaseCamera& camera)
{

	for (std::vector<std::unique_ptr<IObject>>::iterator it = objects_.begin();
		it != objects_.end(); ++it) {
		static_cast<MeshObject*>(it->get())->Draw(camera);
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

