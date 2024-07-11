#pragma once
// コライダー

#include "../../Engine/Math/Vector3.h"
#include "../../Engine/Math/Matrix4x4.h"
#include "../Collider/Collider.h"
#include "../Collider/ColliderShape.h"
#include "Octree/OctreeManager.h"

class CollisionManager
{

public: // メンバ関数

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	// リストのクリア
	void ListClear();

	// リスト登録
	void ListRegister(ColliderShape* collider);

	// 衝突判定と応答
	void CheakAllCollision();

private:

	// コライダーリスト
	//std::list<ColliderShape*> colliders_;

	// 八分木
	std::unique_ptr<OctreeManager> octreeManager_;

	// コライダー2つの衝突判定と応答
	void CheckCollisionPair(ColliderShape* colliderA, ColliderShape* colliderB);

};
