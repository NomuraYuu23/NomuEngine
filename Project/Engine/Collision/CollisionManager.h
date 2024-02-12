#pragma once
// コライダー

#include "../../Engine/Math/Vector3.h"
#include "../../Engine/Math/Matrix4x4.h"
#include "../Collider/Collider.h"
#include <variant>
#include "../Collider/AABB/AABB.h"
#include "../Collider/OBB/OBB.h"
#include "../Collider/Plane/Plane.h"
#include "../Collider/Sphere/Sphere.h"
#include "../Collider/Triangle/Triangle.h"
#include "../Collider/Capsule/Capsule.h"

class CollisionManager
{

public:

	using ColliderShape = std::variant<AABB*, OBB*, Sphere*, Capsule* >;

public: // メンバ関数

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	// リストのクリア
	void ListClear();

	// リスト登録
	void ListRegister(ColliderShape collider);

	// 衝突判定と応答
	void CheakAllCollision();

private:

	// コライダーリスト
	std::list<ColliderShape> colliders_;

	// コライダー2つの衝突判定と応答
	void CheckCollisionPair(ColliderShape colliderA, ColliderShape colliderB);

};
