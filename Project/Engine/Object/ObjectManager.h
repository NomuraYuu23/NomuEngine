#pragma once

#include "IObject.h"
#include "../Camera/BaseCamera.h"

#include <vector>
#include "../Level/LevelIndex.h"
#include "../Level/LevelDataManager.h"
#include "../Collider/ColliderDebugDraw/ColliderDebugDraw.h"
#include "../Collision/CollisionManager.h"

class ObjectManager
{

public:

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="levelIndex">レベル番号</param>
	/// <param name="levelDataManager">レベルデータマネージャー</param>
	void Initialize(LevelIndex levelIndex, LevelDataManager* levelDataManager);

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	/// <param name="camera">カメラ</param>
	void Draw(BaseCamera& camera);

	/// <summary>
	/// 描画(デバッグ)
	/// </summary>
	/// <param name="camera">カメラ</param>
	/// <param name="drawLine">線描画クラス</param>
	void Draw(BaseCamera& camera, DrawLine * drawLine);

	/// <summary>
	///	ImGui描画
	/// </summary>
	void ImGuiDraw();

	/// <summary>
	/// オブジェクト検索
	/// </summary>
	/// <param name="name"></param>
	/// <returns></returns>
	IObject* GetObjectPointer(const std::string name);

	/// <summary>
	/// コライダー登録
	/// </summary>
	/// <param name="collisionManager"></param>
	void CollisionListRegister(CollisionManager* collisionManager);

private:

	using ObjectPair = std::pair<std::string, std::unique_ptr<IObject>>;

	// オブジェクト
	std::vector<ObjectPair> objects_{};

	// コライダーのデバッグ描画
	std::unique_ptr<ColliderDebugDraw> colliderDebugDraw_;

};

