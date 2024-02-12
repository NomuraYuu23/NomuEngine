#include "BaseCharacter.h"

void BaseCharacter::Initialize(const std::vector<Model*>& models,
	const std::vector<Material*>& materials)
{

	//モデルデータ配列
	models_ = models;
	//マテリアルデータ配列
	materials_ = materials;
	//ワールド変換データの初期化
	worldTransform_.Initialize();

}

void BaseCharacter::Update()
{

	// ワールド変換データの行列更新
	worldTransform_.UpdateMatrix();

}

void BaseCharacter::Draw(BaseCamera& camera){

	//モデル描画
	for (Model* model : models_) {
		model->Draw(worldTransform_, camera);
	}

}
