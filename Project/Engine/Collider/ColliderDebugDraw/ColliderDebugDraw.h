#pragma once
#include "../Sphere/Sphere.h"
#include "../Plane/Plane.h"
#include "../Triangle/Triangle.h"
#include "../AABB/AABB.h"
#include "../OBB/OBB.h"

#include <list>
#include <vector>
#include "../../3D/Model.h"
#include "../../3D/Material.h"

/// <summary>
/// コライダーの描画関数デバッグ用
/// </summary>
class ColliderDebugDraw
{

public: // サブクラス

	enum class ModelNo {
		kSphere,
		//kPlane,
		//kTriangle,
		kAABB,
		kOBB,
		kEnd // 使わない
	};

public:

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="models"></param>
	/// <param name="material"></param>
	void Initialize(const std::vector<Model*> models, Material* material);

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	/// <param name="viewProjection"></param>
	void Draw(BaseCamera& camera);

	/// <summary>
	/// コライダー追加
	/// </summary>
	void AddCollider(Sphere* sphere);
	//void AddCollider(Plane* plane);
	//void AddCollider(Triangle* triangle);
	void AddCollider(AABB* aabb);
	void AddCollider(OBB* obb);

private: // メンバ関数

	void ImGuiDraw();

private: // メンバ変数

	// 球
	std::list<Sphere*> spheres_;
	// 平面
	//std::list<Plane*> planes_;
	// 三角形
	//std::list<Triangle*> triangles_;
	// AABB
	std::list<AABB*> aabbs_;
	// OBB
	std::list<OBB*> obbs_;

	// モデル
	std::vector<Model*> models_;

	// マテリアル
	Material* material_;

	// 表示するか
	bool isDraw_;

};

