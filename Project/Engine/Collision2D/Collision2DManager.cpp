#include "Collision2DManager.h"

void Collision2DManager::Initialize()
{

	ListClear();

}

void Collision2DManager::ListClear()
{
	colliders_.clear();
}

void Collision2DManager::ListRegister(ColliderShape2D collider)
{

	colliders_.push_back(collider);

}

void Collision2DManager::CheakAllCollision()
{

	// リスト内のペアを総当たり
	std::list<ColliderShape2D>::iterator itrA = colliders_.begin();
	for (; itrA != colliders_.end(); ++itrA) {
		// イテレータAからコライダーAを取得する
		ColliderShape2D colliderA = *itrA;
		// イテレータBはイテレータAの次の要素から回す(重複判定を回避)
		std::list<ColliderShape2D>::iterator itrB = itrA;
		itrB++;

		for (; itrB != colliders_.end(); ++itrB) {
			// イテレータBからコライダーBを取得する
			ColliderShape2D colliderB = *itrB;

			// ペアの当たり判定
			CheckCollisionPair(colliderA, colliderB);
		}
	}

}

void Collision2DManager::CheckCollisionPair(ColliderShape2D colliderA, ColliderShape2D colliderB)
{



}
