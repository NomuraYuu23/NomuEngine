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
#include "VertexInfluence.h"
#include "TransformationMatrix.h"
#include "TransformStructure.h"

#include "Material.h"

#include "WorldTransform.h"

#include <list>

#include "../base/GraphicsPipelineState/GraphicsPipelineState.h"
#include "../Particle/ParticleManager.h"

#include "../Camera/BaseCamera.h"

#include "../Light/PointLight/PointLightManager.h"
#include "../Light/SpotLight/SpotLightManager.h"
#include "../Light/DirectionalLight/DirectionalLight.h"

#include "Mesh.h"
#include "ModelNode.h"
#include "../Animation/NodeAnimationData.h"
#include "../Animation/AnimationData.h"
#include "../Animation/LocalMatrixManager.h"

class Model
{

public:

	struct MaterialData {
		std::vector<std::string> textureFilePaths;
	};

	struct ModelData {

		// 頂点
		std::vector<VertexData> vertices;
		std::vector<VertexInfluence> vertexInfluences;
		// マテリアル
		MaterialData material;
		// ノード
		ModelNode rootNode;
		// ノードアニメーション
		std::vector<AnimationData> animations;
		// ノードアニメーション数
		uint32_t nodeAnimationNum;

	};

	/// <summary>
	/// 静的初期化
	/// </summary>
	/// <param name="device">デバイス</param>
	static void StaticInitialize(ID3D12Device* device);

	/// <summary>
	/// 3Dモデル生成
	/// </summary>
	/// <returns></returns>
	static Model* Create(
		const std::string& directoryPath, 
		const std::string& filename, 
		DirectXCommon* dxCommon);

	/// <summary>
	/// デフォルトマテリアル取得
	/// </summary>
	/// <returns></returns>
	static Material* GetDefaultMaterial() { return sDefaultMaterial.get(); };

private:

	// デバイス
	static ID3D12Device* sDevice;
	// デフォルトマテリアル
	static std::unique_ptr<Material> sDefaultMaterial;

public:

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(const std::string& directoryPath, const std::string& filename, DirectXCommon* dxCommon);

	/// <summary>
	/// テクスチャハンドルの設定
	/// </summary>
	/// <param name="textureHandle"></param>
	void SetTextureHandle(uint32_t textureHandle, uint32_t index);
	std::vector<UINT> GetTextureHandles() { return textureHandles_; }

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

	/// <summary>
	/// メッシュ取得
	/// </summary>
	/// <returns></returns>
	Mesh* GetMesh() { return mesh_.get(); }

	/// <summary>
	/// モデルデータ取得
	/// </summary>
	/// <returns></returns>
	Model::ModelData GetModelData() { return modelData_; }

	/// <summary>
	/// ファイルの名前取得
	/// </summary>
	/// <returns></returns>
	std::string GetFileName() { return fileName_; }

private:

	//モデル読み込み
	Model::ModelData modelData_;

	// メッシュ
	std::unique_ptr<Mesh> mesh_;

	//テクスチャ番号
	std::vector<UINT> textureHandles_;

	// リソース設定
	std::vector<D3D12_RESOURCE_DESC> resourceDescs_;

	// ファイルの名前
	std::string fileName_;

};
