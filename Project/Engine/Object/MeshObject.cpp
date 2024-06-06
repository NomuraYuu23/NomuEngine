#include "MeshObject.h"
#include "../3D/ModelManager.h"

void MeshObject::Initialize(LevelData::MeshData* data)
{

	// 名前
	name_ = data->name;

	// ファイルの名前
	fileNmae_ = data->flieName;

	// ワールドトランスフォーム
	worldTransform_.Initialize();
	worldTransform_.transform_ = data->transform;
	worldTransform_.UpdateMatrix();

	// モデル
	// ディレクトリパスがまだ
	model_ = ModelManager::GetInstance()->GetModel("Resources/default", fileNmae_);

	// マテリアル
	material_.reset(Material::Create());

}
