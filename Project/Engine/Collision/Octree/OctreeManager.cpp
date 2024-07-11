#include "OctreeManager.h"
#include "../Collision.h"
#include "../CollisionData.h"

void OctreeManager::Initialize()
{

	// マネージャーの初期化
	massNum_ = static_cast<int32_t>(std::powf(2.0f, static_cast<float>(division_)));

	// 八分木の初期化
	octree_ = std::make_unique<Octree>();
	octree_->Initialize(massNum_, division_, length_, center_);

	// 空間の初期化
	uint32_t cellNum = 1;
	std::vector<uint32_t> checkNums;
	checkNums.push_back(0);
	for (uint32_t i = 0; i < division_; ++i) {
		cellNum += static_cast<int32_t>(std::powf(8.0f, static_cast<float>(i + 1)));
		checkNums.emplace_back(cellNum - 1);
	}


	uint32_t checkNum = checkNums[0];
	uint32_t level = 0;

	// 空間生成
	octreeCells_.resize(cellNum);
	for (uint32_t i = 0; i < cellNum; ++i) {

		OctreeCell* octreeCell = new OctreeCell();

		octreeCells_[i].reset(octreeCell);

	}

	// リスト
	std::list<OctreeCell*> cellList;
	// 下にある空間取得
	for (uint32_t i = 0; i < cellNum; ++i) {

		if (checkNum < i) {
			level++;
			checkNum = checkNums[level];
		}

		uint32_t sub = 0;
		for (uint32_t i = 0; i < level; ++i) {
			sub += static_cast<int32_t>(std::powf(8.0f, static_cast<float>(i)));
		}

		// 自分の所属している空間での番号
		uint32_t num = cellNum - sub;

		// 空間リスト作成
		uint32_t startIndex = sub + static_cast<int32_t>(std::powf(8.0f, static_cast<float>(level))) + num * 8;
		cellList.clear();
		if (startIndex < octreeCells_.size()) {
			for (uint32_t i = 0; i < 8; ++i) {
				cellList.push_back(octreeCells_[startIndex + i].get());
			}
		}

		octreeCells_[i]->Initialize(cellList);

	}

}

void OctreeManager::ListClear()
{

	for (uint32_t i = 0; i < octreeCells_.size(); ++i) {
		octreeCells_[i]->ObjListClear();
	}

}

void OctreeManager::ListRegister(ColliderShape* collider)
{

	Vector3 lt;
	Vector3 rb;
	GetLtRb(lt, rb, collider);

	Vector3 pos =  octree_->OctreeSpaceTransformation(lt);
	int32_t ltMorton = octree_->Get3DMortonOrder(static_cast<int32_t>(pos.x), static_cast<int32_t>(pos.y), static_cast<int32_t>(pos.z));
	pos = octree_->OctreeSpaceTransformation(rb);
	int32_t rbMorton = octree_->Get3DMortonOrder(static_cast<int32_t>(pos.x), static_cast<int32_t>(pos.y), static_cast<int32_t>(pos.z));
	
	int32_t i = octree_->BelongingSpace(ltMorton, rbMorton);

	octreeCells_[i]->ObjListRegister(collider);

}

void OctreeManager::CollisionSystem()
{

	std::list<ColliderShape*> objList;

	CheakAllCollision(objList, octreeCells_[0].get());

}

void OctreeManager::CheakAllCollision(std::list<ColliderShape*> objList, OctreeCell* cell)
{

	// 同じ空間内を総当たり
	std::list<ColliderShape*>::iterator itrA = cell->GetObjList().begin();
	for (; itrA != cell->GetObjList().end(); ++itrA) {
		// イテレータAからコライダーAを取得する
		ColliderShape* colliderA = *itrA;
		
		// イテレータBはイテレータAの次の要素から回す(重複判定を回避)
		std::list<ColliderShape*>::iterator itrB = itrA;
		itrB++;

		for (; itrB != cell->GetObjList().end(); ++itrB) {
			// イテレータBからコライダーBを取得する
			ColliderShape* colliderB = *itrB;

			// ペアの当たり判定
			CheckCollisionPair(colliderA, colliderB);
		}

		// イテレータCは上の空間内
		std::list<ColliderShape*>::iterator itrC = objList.begin();
		
		for (; itrC != objList.end(); ++itrC) {
			// イテレータBからコライダーBを取得する
			ColliderShape* colliderC = *itrC;

			// ペアの当たり判定
			CheckCollisionPair(colliderA, colliderC);
		}

	}

	// 下の空間があるか
	if (!cell->GetCellList().empty()) {

		std::list<OctreeCell*> cellList = cell->GetCellList();

		// 渡すオブジェクトリスト
		std::list<ColliderShape*> parentObjList = objList;
		for (std::list<ColliderShape*>::iterator objItr = cell->GetObjList().begin();
			objItr != cell->GetObjList().end(); ++objItr) {
			parentObjList.push_back(*objItr);
		}

		// 
		for (std::list<OctreeCell*>::iterator it = cellList.begin();
			it != cellList.end(); ++it) {

			CheakAllCollision(parentObjList, *it);

		}

	}

}

void OctreeManager::CheckCollisionPair(ColliderShape* colliderA, ColliderShape* colliderB)
{

	std::visit([](auto& a, auto& b) {
		// 衝突フィルタリング

		if (!(a.GetCollisionAttribute() & b.GetCollisionMask()) ||
			!(b.GetCollisionAttribute() & a.GetCollisionMask())) {
			return;
		}
		Vector3 p1 = {};
		Vector3 p2 = {};
		float t1 = 0.0f;
		float t2 = 0.0f;
		float pushBackDist = 0.0f;
		if (Collision::IsCollision(a, b, p1, p2, t1, t2, pushBackDist)) {
			// 衝突処理
			std::visit([=](const auto& x, const auto& y) {
				CollisionData collisionData = { p1, t1, pushBackDist ,p2 };
				x->OnCollision(y, collisionData);
				collisionData = { p2, t2, pushBackDist ,p1 };
				y->OnCollision(x, collisionData);
				}, a.GetParentObject(), b.GetParentObject());
		}
		}, *colliderA, *colliderB);

}

void OctreeManager::GetLtRb(Vector3& lt, Vector3& rb, ColliderShape* collider)
{

	if (std::holds_alternative<OBB>(*collider)) {
		OBB obb = std::get<OBB>(*collider);
		GetLtRbsOBB(lt, rb, obb);
	}
	else if (std::holds_alternative<Sphere>(*collider)) {
		Sphere sphere = std::get<Sphere>(*collider);
		GetLtRbsSphere(lt, rb, sphere);
	}
	
}

void OctreeManager::GetLtRbsOBB(Vector3& lt, Vector3& rb, const OBB& collider)
{

	Vector3 center = collider.center_;

	//頂点

	//回転させる
	Vector3 obbVertex[8];

	obbVertex[0] = {
		+collider.size_.x,
		+collider.size_.y,
		+collider.size_.z };

	obbVertex[1] = {
		+collider.size_.x,
		+collider.size_.y,
		-collider.size_.z };

	obbVertex[2] = {
		+collider.size_.x,
		-collider.size_.y,
		+collider.size_.z };

	obbVertex[3] = {
		+collider.size_.x,
		-collider.size_.y,
		-collider.size_.z };

	obbVertex[4] = {
		-collider.size_.x,
		+collider.size_.y,
		+collider.size_.z };

	obbVertex[5] = {
		-collider.size_.x,
		+collider.size_.y,
		-collider.size_.z };

	obbVertex[6] = {
		-collider.size_.x,
		-collider.size_.y,
		+collider.size_.z };

	obbVertex[7] = {
		-collider.size_.x,
		-collider.size_.y,
		-collider.size_.z };

	Matrix4x4 obbRotateMatrix = {
		collider.otientatuons_[0].x,collider.otientatuons_[0].y,collider.otientatuons_[0].z,0.0f,
		collider.otientatuons_[1].x,collider.otientatuons_[1].y,collider.otientatuons_[1].z,0.0f,
		collider.otientatuons_[2].x,collider.otientatuons_[2].y,collider.otientatuons_[2].z,0.0f,
		0.0f,0.0f,0.0f,1.0f };

	for (uint32_t i = 0; i < 8; i++) {

		obbVertex[i] = Matrix4x4::Transform(obbVertex[i], obbRotateMatrix);
		obbVertex[i] = Vector3::Add(obbVertex[i], collider.center_);

	}

	Vector3 min = obbVertex[0];
	Vector3 max = obbVertex[0];

	for (uint32_t i = 1; i < 8; i++) {

		if (min.x > obbVertex[i].x) {
			min.x = obbVertex[i].x;
		}
		if (min.y > obbVertex[i].y) {
			min.y = obbVertex[i].y;
		}
		if (min.z > obbVertex[i].z) {
			min.z = obbVertex[i].z;
		}

		if (max.x < obbVertex[i].x) {
			max.x = obbVertex[i].x;
		}
		if (max.y < obbVertex[i].y) {
			max.y = obbVertex[i].y;
		}
		if (max.z < obbVertex[i].z) {
			max.z = obbVertex[i].z;
		}

	}

	lt = min;
	rb = max;

}

void OctreeManager::GetLtRbsSphere(Vector3& lt, Vector3& rb, const Sphere& collider)
{

	Vector3 center = collider.center_;

	float radius = collider.radius_;

	lt = { center.x - radius, center.y - radius, center.z - radius };
	rb = { center.x + radius, center.y + radius, center.z + radius };

}
