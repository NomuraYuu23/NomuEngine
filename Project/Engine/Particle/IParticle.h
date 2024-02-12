#pragma once
#include "../3D/TransformStructure.h"
#include "../Math/Matrix4x4.h"

#include "../3D/TransformationMatrix.h"
#include <wrl.h>
#include <d3d12.h>
#include "../Math/Vector4.h"
#include "ParticleForGPU.h"
#include "../../Application/Particle/ParticleName.h"

class IParticle
{

public: // サブクラス

	enum BillBoardNameIndex {
		kBillBoardNameIndexAllAxis,	// 全軸
		kBillBoardNameIndexXAxis,	// x軸
		kBillBoardNameIndexYAxis,	// y軸
		kBillBoardNameIndexZAxis,	// z軸
		kBillBoardNameIndexOfCount
	};


public:

	/// <summary>
	/// デストラクタ
	/// </summary>
	virtual ~IParticle();

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="numInstance"></param>
	virtual void Initialize(const Vector3& position, const Vector3& size);

	/// <summary>
	/// 更新
	/// </summary>
	/// <param name="cameraMatrix4x4"></param>
	virtual void Update(const Matrix4x4& billBoardMatrix);

	/// <summary>
	/// 行列更新
	/// </summary>
	/// <param name="num"></param>
	void UpdateMatrix(const Matrix4x4& billBoardMatrix);

	/// <summary>
	/// マッピング
	/// </summary>
	/// <param name="viewProjection"></param>
	ParticleForGPU Map(const Matrix4x4& viewProjectionMatrix);

	/// <summary>
	/// 時間経過
	/// </summary>
	/// <param name="num"></param>
	void TimeElapsed();

	/// <summary>
	/// 徐々に消える
	/// </summary>
	/// <param name="num"></param>
	void GraduallyDisappear();

public: // アクセッサ

	/// <summary>
	/// 死んでいるか
	/// </summary>
	/// <returns></returns>
	bool IsDead() { return isDead_; }

	/// <summary>
	///	使うビルボード
	/// </summary>
	/// <returns></returns>
	BillBoardNameIndex GetBillBoardName() { return billBoardName_; }

protected: // メンバ変数

	// 基本的な変数
	// トランスフォーム
	TransformStructure transform_;
	// 行列
	Matrix4x4 worldMatrix_;
	// 速度
	Vector3 velocity_;
	// 色
	Vector4 color_;
	// 生存時間
	float lifeTime_;
	// 発生してからの経過時間
	float currentTime_;

	// ビルボードを使うか
	bool useBillBoard_;

	// 使うビルボード
	BillBoardNameIndex billBoardName_;

	// 死んでいるか
	bool isDead_;

};

