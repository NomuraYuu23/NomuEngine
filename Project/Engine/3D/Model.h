#pragma once
#include <DirectXMath.h>
#include <Windows.h>
#include <d3d12.h>
#include <string>
#include <wrl.h>
#include <dxcapi.h>

#pragma comment(lib, "dxcompiler.lib")

#include "../base/DirectXCommon.h"

#include "../Math/Vector2.h"
#include "../Math/Vector3.h"
#include "../Math/Vector4.h"
#include "../Math/Matrix4x4.h"
#include "VertexData.h"
#include "TransformationMatrix.h"
#include "TransformStructure.h"

#include "Material.h"

#include "WorldTransform.h"

#include <list>

#include "../base/GraphicsPipelineState/GraphicsPipelineState.h"
#include "../Particle/ParticleManager.h"

#include "../Camera/BaseCamera.h"

#include "../base/ITextureHandleManager.h"
#include "OutLineData.h"
#include "../Light/PointLight/PointLightManager.h"
#include "../Light/SpotLight/SpotLightManager.h"

#include "Mesh.h"
#include "ModelNode.h"
#include "../Animation/NodeAnimationData.h"
#include "../Animation/AnimationData.h"

class Model
{

public:

	struct MaterialData {
		std::vector<std::string> textureFilePaths;
	};

	struct ModelData {

		// 頂点
		std::vector<VertexData> vertices;
		// マテリアル
		MaterialData material;
		// ノード
		ModelNode rootNode;
		// ノードアニメーション
		std::vector<AnimationData> animations;
		// ノードアニメーション数
		uint32_t nodeAnimationNum;

	};

	enum PipelineStateName {
		kPipelineStateNameModel,
		kPipelineStateNameParticle,
		kPipelineStateNameOutLine,
		kPipelineStateNameManyModels,
		kPipelineStateNameOfCount
	};

	/// <summary>
	/// 静的初期化
	/// </summary>
	/// <param name="device">デバイス</param>
	static void StaticInitialize(ID3D12Device* device,
		const std::array<ID3D12RootSignature*, PipelineStateName::kPipelineStateNameOfCount>& rootSignature,
		const std::array<ID3D12PipelineState*, PipelineStateName::kPipelineStateNameOfCount>& pipelineState);

	/// <summary>
	/// 静的前処理
	/// </summary>
	/// <param name="cmdList">描画コマンドリスト</param>
	static void PreDraw(ID3D12GraphicsCommandList* cmdList, PointLightManager* pointLightManager = nullptr, SpotLightManager* spotLightManager = nullptr);

	/// <summary>
	/// 静的前処理
	/// </summary>
	/// <param name="cmdList">描画コマンドリスト</param>
	static void PreParticleDraw(ID3D12GraphicsCommandList* cmdList, const Matrix4x4& viewProjectionMatrix);

	/// <summary>
	/// 静的前処理
	/// </summary>
	/// <param name="cmdList">描画コマンドリスト</param>
	static void PreDrawOutLine(ID3D12GraphicsCommandList* cmdList);

	/// <summary>
	/// 描画後処理
	/// </summary>
	static void PostDraw();

	/// <summary>
	/// 3Dモデル生成
	/// </summary>
	/// <returns></returns>
	static Model* Create(const std::string& directoryPath, const std::string& filename, DirectXCommon* dxCommon, ITextureHandleManager* textureHandleManager);

private:

	// デバイス
	static ID3D12Device* sDevice;
	// ディスクリプタサイズ
	static UINT sDescriptorHandleIncrementSize;
	// コマンドリスト
	static ID3D12GraphicsCommandList* sCommandList;
	// ルートシグネチャ
	static ID3D12RootSignature* sRootSignature[PipelineStateName::kPipelineStateNameOfCount];
	// パイプラインステートオブジェクト
	static ID3D12PipelineState* sPipelineState[PipelineStateName::kPipelineStateNameOfCount];
	// ポイントライトマネージャ
	static PointLightManager* pointLightManager_;
	//	スポットライトマネージャ
	static SpotLightManager* spotLightManager_;

public:

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(const std::string& directoryPath, const std::string& filename, DirectXCommon* dxCommon, ITextureHandleManager* textureHandleManager);

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw(WorldTransform& worldTransform, BaseCamera& camera);
	void Draw(WorldTransform& worldTransform, BaseCamera& camera, Material* material);
	void Draw(WorldTransform& worldTransform, BaseCamera& camera, Material* material,uint32_t textureHandle);

	void Draw(
		const D3D12_GPU_DESCRIPTOR_HANDLE& localMatrixesHandle,
		const D3D12_GPU_DESCRIPTOR_HANDLE& transformationMatrixesHandle,
		BaseCamera& camera,
		uint32_t numInstance);
	void Draw(
		const D3D12_GPU_DESCRIPTOR_HANDLE& localMatrixesHandle,
		const D3D12_GPU_DESCRIPTOR_HANDLE& transformationMatrixesHandle,
		BaseCamera& camera,
		uint32_t numInstance,
		Material* material);
	void Draw(
		const D3D12_GPU_DESCRIPTOR_HANDLE& localMatrixesHandle,
		const D3D12_GPU_DESCRIPTOR_HANDLE& transformationMatrixesHandle,
		BaseCamera& camera,
		uint32_t numInstance,
		Material* material,
		uint32_t textureHandle);

	void ParticleDraw();
	void OutLineDraw(WorldTransform& worldTransform, BaseCamera& camera,OutLineData& outLineData);

	/// <summary>
	/// テクスチャハンドルの設定
	/// </summary>
	/// <param name="textureHandle"></param>
	void SetTextureHandle(uint32_t textureHandle, uint32_t index);
	std::vector<UINT> GetTextureHandle() { return textureHandles_; }

	/// <summary>
	/// ローカルマトリックス取得
	/// </summary>
	Matrix4x4 GetRootNodeLocalMatrix() { return modelData_.rootNode.localMatrix; }

	/// <summary>
	/// ローカルマトリックス取得
	/// </summary>
	ModelNode GetRootNode() { return modelData_.rootNode; }

	/// <summary>
	/// ノードアニメーションデータ取得
	/// </summary>
	/// <returns></returns>
	std::vector<AnimationData> GetNodeAnimationData() { return modelData_.animations; }

private:

	//モデル読み込み
	Model::ModelData modelData_;

	// メッシュ
	std::unique_ptr<Mesh> mesh_;

	//テクスチャ番号
	std::vector<UINT> textureHandles_;

	// リソース設定
	std::vector<D3D12_RESOURCE_DESC> resourceDescs_;

	// デフォルトマテリアル
	std::unique_ptr<Material> defaultMaterial_;

};
