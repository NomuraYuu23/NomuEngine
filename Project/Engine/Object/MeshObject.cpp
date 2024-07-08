#include "MeshObject.h"
#include "../3D/ModelManager.h"
#include "../3D/ModelDraw.h"

void MeshObject::Initialize(LevelData::MeshData* data)
{

	// 名前
	name_ = data->name;

	// ファイルの名前
	fileName_ = data->flieName;

	// ディレクトリパス
	directoryPath_ = data->directoryPath;

	// モデル
	model_ = ModelManager::GetInstance()->GetModel(directoryPath_, fileName_);

	// ワールドトランスフォーム
	worldTransform_.Initialize(model_->GetRootNode());
	worldTransform_.transform_ = data->transform;
	worldTransform_.UpdateMatrix();

	// マテリアル
	material_.reset(Material::Create());


	ColliderInitialize(data->collider);

}

void MeshObject::Draw(BaseCamera& camera)
{


	ModelDraw::NormalObjectDesc desc;

	desc.model = model_;
	desc.material = material_.get();
	desc.camera = &camera;
	desc.worldTransform = &worldTransform_;

	ModelDraw::NormalObjectDraw(desc);

}

void MeshObject::CollisionListRegister(CollisionManager* collisionManager)
{

	collisionManager->ListRegister(collider_.get());

}

void MeshObject::CollisionListRegister(CollisionManager* collisionManager, ColliderDebugDraw* colliderDebugDraw)
{

	collisionManager->ListRegister(collider_.get());

	colliderDebugDraw->AddCollider(*collider_.get());

}

void MeshObject::ColliderInitialize(ColliderShape collider)
{

	// 値があるなら
	if (std::holds_alternative<OBB>(collider) || std::holds_alternative<Sphere>(collider)) {

		ColliderShape* colliderShape = new ColliderShape();

		*colliderShape = collider;

		collider_.reset(colliderShape);

	}

}
