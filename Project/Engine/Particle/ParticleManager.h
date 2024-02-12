#pragma once
#include <cstdint>
#include "../3D/TransformationMatrix.h"
#include "../base/BufferResource.h"
#include "IParticle.h"
#include "IEmitter.h"
#include <list>
#include <memory>
#include <array>
#include "ParticleForGPU.h"
#include "../../Application/Particle/ParticleModelIndex.h"

class Model;
class BaseCamera;

class ParticleManager
{

public: // サブクラス

	struct StartInstanceId {
		int32_t num;
		//float padding[3];
	};

	//パーティクルリスト
	struct ParticleData
	{
		// 描画するインスタンス数
		uint32_t instanceIndex_;
		//スタートインスタンス用のリソースを作る。
		Microsoft::WRL::ComPtr<ID3D12Resource> startInstanceIdBuff_;
		//書き込むためのアドレスを取得
		StartInstanceId* startInstanceIdMap_{};
		// パーティクルリスト
		std::list<IParticle*> particles_;
		// モデル
		Model* model_;
	};

public: // 静的メンバ変数

	// パーティクル最大数
	static uint32_t kNumInstanceMax_;

public: // メンバ関数

	/// <summary>
	/// インスタンス取得
	/// </summary>
	/// <returns></returns>
	static ParticleManager* GetInstance();

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// SRVを作る
	/// </summary>
	void SRVCreate();

	/// <summary>
	/// 更新
	/// </summary>
	void Update(BaseCamera& camera);

	/// <summary>
	/// 描画
	/// </summary>
	/// <param name="viewProjection"></param>
	void Draw();

	/// <summary>
	/// マッピング
	/// </summary>
	/// <param name="viewProjection"></param>
	void Map(const Matrix4x4& viewProjectionMatrix);

	/// <summary>
	/// 後処理
	/// </summary>
	void Finalize();

	/// <summary>
	/// モデル作成
	/// </summary>
	void ModelCreate(std::array<Model*, kCountofParticleModelIndex> model);

	/// <summary>
	/// ビルボード更新
	/// </summary>
	/// <param name="cameraMatrix4x4"></param>
	void BillBoardUpdate(BaseCamera& camera);

	/// <summary>
	/// エミッタ生成
	/// </summary>
	/// <param name="transform"></param>
	/// <param name="lifeTime"></param>
	void MakeEmitter(const TransformStructure& transform, uint32_t instanceCount,
		float frequency, float lifeTime,
		uint32_t particleModelNum, uint32_t paeticleName, uint32_t emitterName);

	/// <summary>
	/// エミッタ更新
	/// </summary>
	void EmitterUpdate();

	/// <summary>
	/// パーティクル追加
	/// </summary>
	void AddParticles(std::list<IParticle*> particles, uint32_t particleModelNum);

	/// <summary>
	/// パーティクル更新
	/// </summary>
	void ParticlesUpdate();

	/// <summary>
	/// 死んでるのを削除
	/// </summary>
	void DeadDelete();

public: // アクセッサ

	D3D12_CPU_DESCRIPTOR_HANDLE GetInstancingSrvHandleCPU() { return instancingSrvHandleCPU_; }

	D3D12_GPU_DESCRIPTOR_HANDLE GetInstancingSrvHandleGPU() { return instancingSrvHandleGPU_; }

	ParticleForGPU* GetParticleForGPUMap() { return particleForGPUMap_; }

	ID3D12Resource* GetParticleForGPUBuff() { return particleForGPUBuff_.Get(); }

	uint32_t GetCurrentInstanceIndex() { return particleDatas_[currentModel_].instanceIndex_; }

	Matrix4x4 GetBillBoardMatrix() { return billBoardMatrix_; }

	ID3D12Resource* GetCurrentStartInstanceIdBuff() { return particleDatas_[currentModel_].startInstanceIdBuff_.Get(); }

private: // メンバ変数

	ParticleManager() = default;
	~ParticleManager() = default;
	ParticleManager(const ParticleManager&) = delete;
	const ParticleManager& operator=(const ParticleManager&) = delete;

	//WVP用のリソースを作る。
	Microsoft::WRL::ComPtr<ID3D12Resource> particleForGPUBuff_;
	//書き込むためのアドレスを取得
	ParticleForGPU* particleForGPUMap_{};

	D3D12_CPU_DESCRIPTOR_HANDLE instancingSrvHandleCPU_;

	D3D12_GPU_DESCRIPTOR_HANDLE instancingSrvHandleGPU_;

	// パーティクル
	std::array<ParticleData, kCountofParticleModelIndex> particleDatas_;

	// ビルボード
	Matrix4x4 billBoardMatrix_;
	Matrix4x4 billBoardMatrixX_;
	Matrix4x4 billBoardMatrixY_;
	Matrix4x4 billBoardMatrixZ_;

	// エミッタ
	std::list<IEmitter*> emitters_;

	// 現在のモデル
	uint32_t currentModel_ = 0u;

};

