#pragma once
#include "../../../Engine/3D/Model.h"
#include "../../../Engine/Physics/RigidBody.h"
#include "../../../Engine/Animation/Animation.h"
#include "../../../Engine/PostEffect/Velocity2DData.h"
#include "../../../Engine/Animation/LocalMatrixDraw.h"
#include "../../../Engine/3D/Outline.h"

class SampleObject
{

public:

	~SampleObject();

	void Initialize(Model* model);

	void Update();

	void Draw(BaseCamera& camera);

	void ImGuiDraw();

	/// <summary>
	/// デバッグ描画用マッピング
	/// </summary>
	/// <param name="drawLine">線描画ポインタ</param>
	void DebugDrawMap(DrawLine* drawLine);

	ID3D12Resource* GetVelocity2DData() { return velocity2DDataBuff_.Get(); }

private:

	/// <summary>
	/// 外部変数登録
	/// </summary>
	virtual void RegisteringGlobalVariables();

	/// <summary>
	/// 外部変数適用
	/// </summary>
	virtual void ApplyGlobalVariables();

private:

	// モデル
	Model* model_ = nullptr;
	// マテリアル
	std::unique_ptr<Material> material_ = nullptr;
	// ワールドトランスフォーム
	WorldTransform worldTransform_{};

	// ローカル行列
	std::unique_ptr<LocalMatrixManager> localMatrixManager_ = nullptr;

	int32_t enableLighting_ = 0;

	float shininess_ = 0.0f;
	float environmentCoefficient_ = 1.0f;

	//ノードアニメーション
	Animation animation_{};

	RigidBody rigidBody_{};

	// 速度バッファ
	Microsoft::WRL::ComPtr<ID3D12Resource> velocity2DDataBuff_;

	Velocity2DData* velocity2DDataMap_{};

	Vector3 velocity_{};

	std::unique_ptr<Outline> outline_;

};

