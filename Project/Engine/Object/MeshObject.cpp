#include "MeshObject.h"

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

	// モデルマネージャーから名前の一致する物を探す
	// 在れば参照の値をとってくる
	// なければ、新しいモデルを作成、参照の値をとってくる

	// マテリアル
	material_.reset(Material::Create());

}
