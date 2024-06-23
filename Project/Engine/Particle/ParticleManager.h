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

public: // 定数

	// 最大数
	const uint32_t kNumInstanceMax_ = 256;

public: // メンバ関数

	/// <summary>
	/// インスタンス取得
	/// </summary>
	/// <returns></returns>
	static ParticleManager* GetInstance();

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="rootSignature">ルートシグネチャ</param>
	/// <param name="pipelineState">パイプライン</param>
	void Initialize(ID3D12RootSignature* rootSignature,
		ID3D12PipelineState* pipelineState);

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
	/// <param name="viewProjectionMatrix">ビュープロジェクション行列</param>
	/// <param name="commandList">コマンドリスト</param>
	void Draw(const Matrix4x4& viewProjectionMatrix,
		ID3D12GraphicsCommandList* commandList);

	/// <summary>
	/// マッピング
	/// </summary>
	/// <param name="viewProjection">ビュープロジェクション</param>
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
	/// エミッタ生成
	/// </summary>
    /// <param name="emitterDesc">エミッタ引数</param>
    /// <param name="emitterName">エミッタの名前</param>
	void MakeEmitter(EmitterDesc* emitterDesc, uint32_t emitterName);

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
	/// <param name="camera">カメラ</param>
	void ParticlesUpdate(BaseCamera& camera);

	/// <summary>
	/// 死んでるのを削除
	/// </summary>
	void DeadDelete();

private: // メンバ変数

	//WVP用のリソースを作る。
	Microsoft::WRL::ComPtr<ID3D12Resource> particleForGPUBuff_;
	//書き込むためのアドレスを取得
	ParticleForGPU* particleForGPUMap_{};

	D3D12_CPU_DESCRIPTOR_HANDLE instancingSrvHandleCPU_;

	D3D12_GPU_DESCRIPTOR_HANDLE instancingSrvHandleGPU_;

	// パーティクル
	std::array<ParticleData, kCountofParticleModelIndex> particleDatas_;

	// エミッタ
	std::list<IEmitter*> emitters_;

	// ルートシグネチャ
	ID3D12RootSignature* rootSignature_;
	// パイプラインステートオブジェクト
	ID3D12PipelineState* pipelineState_;

private: // シングルトン

	ParticleManager() = default;
	~ParticleManager() = default;
	ParticleManager(const ParticleManager&) = delete;
	const ParticleManager& operator=(const ParticleManager&) = delete;

};

