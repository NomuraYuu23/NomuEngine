#include "ShadowManager.h"
#include <algorithm>

void ShadowManager::Initialize(Model* model)
{

	// モデル
	model_ = model;

	// リスト
	ListClear();

	// ワールドトランスフォーム,マテリアル
	Vector4 color = { 1.0f,1.0f,1.0f, 0.5f };
	for (uint32_t i = 0; i < worldTransforms_.size(); ++i) {
		worldTransforms_[i].Initialize();
		materials_[i].reset(Material::Create());
		materials_[i]->SetColor(color);
	}

	// 影を出す数
	shadowCount_ = 0u;

	isShadowMax_ = false;

}

void ShadowManager::ListClear()
{

	castsShadowObjList_.clear();
	shadowAppearsObjList_.clear();

}

void ShadowManager::CastsShadowObjListRegister(const Vector3& position, const Vector3& size)
{

	ShadowObj shadowObj = { position , size };

	castsShadowObjList_.push_back(shadowObj);

}

void ShadowManager::ShadowAppearsObjListRegister(const Vector3& position, const Vector3& size)
{

	ShadowObj shadowObj = { position , size };

	shadowAppearsObjList_.push_back(shadowObj);

}

void ShadowManager::SeeShadow()
{

	shadowCount_ = 0u;
	isShadowMax_ = false;

	// 影を発生させるオブジェクトリスト
	std::list<ShadowObj>::iterator itrCastsShadowObj = castsShadowObjList_.begin();
	for (; itrCastsShadowObj != castsShadowObjList_.end(); ++itrCastsShadowObj) {

		// 影が現れるオブジェクトリスト	
		std::list<ShadowObj>::iterator itrShadowAppearsObj = shadowAppearsObjList_.begin();
		for (; itrShadowAppearsObj != shadowAppearsObjList_.end(); ++itrShadowAppearsObj) {

			// 影が出るか確認
			if (CollisionCheck(*itrCastsShadowObj, *itrShadowAppearsObj)) {
				break;
			}

		}

		// 影が最大
		if (isShadowMax_) {
			return;
		}

	}


}

void ShadowManager::Draw(BaseCamera& baseCamera)
{

	for (uint32_t i = 0; i < shadowCount_; ++i) {
		model_->Draw(worldTransforms_[i], baseCamera, materials_[i].get());
	}

}

bool ShadowManager::OverlapY(const ShadowObj& a, const ShadowObj& b)
{
	
	// 高さ確認
	if (a.position_.y + a.size_.y >= b.position_.y + b.size_.y) {
		
		// 完全に内包している

		if (a.position_.x - a.size_.x >= b.position_.x - b.size_.x &&
			a.position_.x + a.size_.x <= b.position_.x + b.size_.x &&
			a.position_.z - a.size_.z >= b.position_.z - b.size_.z &&
			a.position_.z + a.size_.z <= b.position_.z + b.size_.z) {

			return true;

		}
	}

	return false;

}

bool ShadowManager::CollisionCheck(const ShadowObj& a, const ShadowObj& b)
{

	// 高さ確認
	if (a.position_.y + a.size_.y >= b.position_.y + b.size_.y) {

		// 完全に内包している
		if (a.position_.x - a.size_.x >= b.position_.x - b.size_.x &&
			a.position_.x + a.size_.x <= b.position_.x + b.size_.x &&
			a.position_.z - a.size_.z >= b.position_.z - b.size_.z &&
			a.position_.z + a.size_.z <= b.position_.z + b.size_.z) {
			CompriseOnCollision(a,b);
		}
		// 完全に内包していないが衝突
		else if(a.position_.x + a.size_.x >= b.position_.x - b.size_.x &&
				a.position_.x - a.size_.x <= b.position_.x + b.size_.x &&
				a.position_.z + a.size_.z >= b.position_.z - b.size_.z &&
				a.position_.z - a.size_.z <= b.position_.z + b.size_.z){
			NotCompriseOnCollision(a, b);
		}
		// 衝突していない
		else {
			return false;
		}
		// 影の数制限
		ShadowLimit();
		return true;
	}		

	return false;

}

void ShadowManager::CompriseOnCollision(const ShadowObj& a, const ShadowObj& b)
{
	// ワールドトランスフォーム設定
	worldTransforms_[shadowCount_].transform_.translate.x = a.position_.x;
	worldTransforms_[shadowCount_].transform_.translate.y = b.position_.y + b.size_.y;
	worldTransforms_[shadowCount_].transform_.translate.z = a.position_.z;
	worldTransforms_[shadowCount_].transform_.scale.x = a.size_.x;
	worldTransforms_[shadowCount_].transform_.scale.y = 1.0f;
	worldTransforms_[shadowCount_].transform_.scale.z = a.size_.z;
	worldTransforms_[shadowCount_].UpdateMatrix();

	TransformStructure transformStructure = {
		0.33f, 0.33f, 1.0f,
		0.0f, 0.0f, 0.0f,
		0.33f, 0.33f, 0.0f
	};
	materials_[shadowCount_]->SetUvTransform(transformStructure);

}

void ShadowManager::NotCompriseOnCollision(const ShadowObj& a, const ShadowObj& b)
{

	worldTransforms_[shadowCount_].transform_.translate.x = a.position_.x;
	worldTransforms_[shadowCount_].transform_.translate.y = b.position_.y + b.size_.y;
	worldTransforms_[shadowCount_].transform_.translate.z = a.position_.z;
	worldTransforms_[shadowCount_].transform_.scale.x = a.size_.x;
	worldTransforms_[shadowCount_].transform_.scale.y = 1.0f;
	worldTransforms_[shadowCount_].transform_.scale.z = a.size_.z;

	Vector3 materialPosition = { 0.33f, 0.33f, 0.0f };
	float move = 0.0f;
	float size = 32.0f;

	// x
	if (a.position_.x + a.size_.x > b.position_.x - b.size_.x && 
		a.position_.x - a.size_.x < b.position_.x - b.size_.x) {
		worldTransforms_[shadowCount_].transform_.translate.x = b.position_.x - b.size_.x + a.size_.x;
		move = 0.33f * (std::fabsf((b.position_.x - b.size_.x) - (a.position_.x - a.size_.x)) / a.size_.x / 2.0f);
		materialPosition.x = 0.33f - move;
	}
	else if(a.position_.x - a.size_.x < b.position_.x + b.size_.x &&
			a.position_.x + a.size_.x > b.position_.x + b.size_.x){
		worldTransforms_[shadowCount_].transform_.translate.x = b.position_.x + b.size_.x - a.size_.x;
		move = 0.33f * (std::fabsf((b.position_.x + b.size_.x) - (a.position_.x + a.size_.x)) / a.size_.x / 2.0f);
		materialPosition.x = 0.33f + move;
	}

	// z
	if (a.position_.z + a.size_.z > b.position_.z - b.size_.z &&
		a.position_.z - a.size_.z < b.position_.z - b.size_.z) {
		worldTransforms_[shadowCount_].transform_.translate.z = b.position_.z - b.size_.z + a.size_.z;
		move = 0.33f * (std::fabsf((b.position_.z - b.size_.z) - (a.position_.z - a.size_.z)) / a.size_.z / 2.0f);
		materialPosition.y = 0.33f + move;
	}
	else if (a.position_.z - a.size_.z < b.position_.z + b.size_.z &&
		a.position_.z + a.size_.z > b.position_.z + b.size_.z) {
		worldTransforms_[shadowCount_].transform_.translate.z = b.position_.z + b.size_.z - a.size_.z;
		move = 0.33f * (std::fabsf((b.position_.z + b.size_.z) - (a.position_.z + a.size_.z)) / a.size_.z / 2.0f);
		materialPosition.y = 0.33f - move;
	}

	TransformStructure transformStructure = {
	0.33f, 0.33f, 1.0f,
	0.0f, 0.0f, 0.0f,
	materialPosition
	};
	materials_[shadowCount_]->SetUvTransform(transformStructure);

	worldTransforms_[shadowCount_].UpdateMatrix();

}

void ShadowManager::ShadowLimit()
{

	// 影の数を増やす
	shadowCount_++;

	// 影の作成最大値
	if (shadowCount_ == kShadowMax_) {
		isShadowMax_ = true;
	}

}
