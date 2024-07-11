#pragma once
#include "Octree.h"
#include "OctreeCell.h"

class OctreeManager
{

private:

	// マスの数 (2 べき乗)
	uint32_t massNum_ = 0;

	// N が分割数
	const uint32_t division_ = 3;

	// 元の辺の長さ
	const float length_ = 80.0f;

	// 中央
	const Vector3 center_ = {0.0f, 0.0f, 0.0f};

public:

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// リストのクリア
	/// </summary>
	void ListClear();

	/// <summary>
	/// リスト登録
	/// </summary>
	/// <param name="collider"></param>
	void ListRegister(ColliderShape* collider);

	/// <summary>
	/// 衝突判定と応答
	/// </summary>
	void CollisionSystem();

private:

	/// <summary>
	/// 衝突判定と応答
	/// </summary>
	void CheakAllCollision(std::list<ColliderShape*> objList, OctreeCell* cell);

	/// <summary>
	/// コライダー2つの衝突判定と応答
	/// </summary>
	/// <param name="colliderA">コライダーA</param>
	/// <param name="colliderB">コライダーB</param>
	void CheckCollisionPair(ColliderShape* colliderA, ColliderShape* colliderB);

	/// <summary>
	/// ltrb取得
	/// </summary>
	/// <param name="lt">左上</param>
	/// <param name="rb">右下</param>
	/// <param name="collider">コライダー</param>
	void GetLtRb(Vector3& lt, Vector3& rb, ColliderShape* collider);

	void GetLtRbsOBB(Vector3& lt, Vector3& rb, const OBB& collider);

	void GetLtRbsSphere(Vector3& lt, Vector3& rb, const Sphere& collider);


private: // 

	// 八分木計算システム
	std::unique_ptr<Octree> octree_;

	// 空間
	std::vector<std::unique_ptr<OctreeCell>> octreeCells_;


};

