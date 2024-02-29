#pragma once
#include "TransformStructure.h"
#include "../Math/Matrix4x4.h"

#include "TransformationMatrix.h"
#include <wrl.h>
#include <d3d12.h>
#include "ModelNode.h"

class WorldTransform
{

public:

	struct NodeData
	{
		Matrix4x4 localMatrix;
		uint32_t meshNum;
	};

public:

	void Initialize();

	void UpdateMatrix();

	void Map(const Matrix4x4& viewProjectionMatrix, const Matrix4x4& modelLocalMatrix);

	void MapSprite();

	Vector3 GetWorldPosition();

	void SetParent(WorldTransform* parent) { parent_ = parent; }

public:

	//トランスフォーム
	TransformStructure transform_{ {1.0f, 1.0f, 1.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f} };

	//ワールド行列
	Matrix4x4 worldMatrix_;

	// 回転行列
	Matrix4x4 rotateMatrix_;

	// 方向ベクトルで回転行列
	bool usedDirection_;

	// 方向ベクトル
	Vector3 direction_ = {0.0f,0.0f,1.0f};

	// スケールを考えない
	Matrix4x4 parentMatrix_;

	//親
	WorldTransform* parent_ = nullptr;


	//WVP用のリソースを作る。Matrix4x4 1つ分のサイズを用意する
	Microsoft::WRL::ComPtr<ID3D12Resource> transformationMatrixBuff_;
	//書き込むためのアドレスを取得
	TransformationMatrix* transformationMatrixMap_{};

public: // お試し

	// コマンドリスト
	static ID3D12GraphicsCommandList* sCommandList;

	//WVP用のリソースを作る。Matrix4x4
	Microsoft::WRL::ComPtr<ID3D12Resource> transformationMatrixesBuff_;
	//書き込むためのアドレスを取得
	TransformationMatrix* transformationMatrixesMap_{};

	D3D12_CPU_DESCRIPTOR_HANDLE instancingSrvHandleCPU_;

	D3D12_GPU_DESCRIPTOR_HANDLE instancingSrvHandleGPU_;

	uint32_t indexDescriptorHeap_ = 0;

	std::vector<NodeData> nodeDatas_;

	void Initialize(const ModelNode& modelNode);
	
	void TmpMap(const Matrix4x4& viewProjectionMatrix, const Matrix4x4& modelLocalMatrix);
	
	/// <summary>
	/// SRVを作る
	/// </summary>
	void SRVCreate();

	void SetGraphicsRootDescriptorTable(ID3D12GraphicsCommandList* cmdList, uint32_t rootParameterIndex);

	void SetNodeDatas(const ModelNode& modelNode);

	void Finalize();

};
