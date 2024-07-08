#pragma once
#include "IObject.h"
#include "../3D/Model.h"
#include "../Level/LevelData.h"
#include "../Collision/CollisionManager.h"
#include "../Collider/ColliderDebugDraw/ColliderDebugDraw.h"

class MeshObject :
    public IObject
{

public: // 関数

    /// <summary>
    /// デストラクタ
    /// </summary>
    virtual ~MeshObject() = default;

    /// <summary>
    /// 初期化
    /// </summary>
    /// <param name="data">メッシュデータ</param>
    virtual void Initialize(LevelData::MeshData* data);

    /// <summary>
    /// 更新処理
    /// </summary>
    virtual void Update() {};

    /// <summary>
    /// 描画
    /// </summary>
    /// <param name="camera">カメラ</param>
    virtual void Draw(BaseCamera& camera);
    
    /// <summary>
    /// コライダー登録
    /// </summary>
    /// <param name="collisionManager"></param>
    virtual void CollisionListRegister(CollisionManager* collisionManager);

    /// <summary>
    /// コライダー登録
    /// </summary>
    /// <param name="collisionManager"></param>
    /// <param name="colliderDebugDraw"></param>
    virtual void CollisionListRegister(CollisionManager* collisionManager, ColliderDebugDraw* colliderDebugDraw);

protected: // 関数

    /// <summary>
    /// コライダーの初期化
    /// </summary>
    /// <param name="collider">コライダー</param>
    virtual void ColliderInitialize(ColliderShape collider);

public: // アクセッサ

    ColliderShape* GetCollider() { return collider_.get(); };

protected: // 変数

    // ファイル名前
    std::string fileName_;

    // ディレクトリパス
    std::string directoryPath_;

    // モデル
    Model* model_ = nullptr;

    // マテリアル
    std::unique_ptr<Material> material_;

    // コライダー
    std::unique_ptr<ColliderShape> collider_;

};

