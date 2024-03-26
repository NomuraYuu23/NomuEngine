#pragma once
#include <cstdint>
#include <list>
#include <wrl.h>
#include <d3d12.h>
#include <memory>
#include "TransformationMatrix.h"
#include "LocalMatrix.h"
#include "ModelNodeData.h"
#include "Model.h"
#include "OneOfManyObjects.h"

/// <summary>
/// 同じアニメーションをする、またはアニメーションをしない
/// 大量のオブジェクトを管理する基底クラス
/// </summary>
class LargeNumberOfObjects
{

public: // 定数

	// 最大数
	static const uint32_t kNumInstanceMax_ = 1024;

public:

	/// <summary>
	/// デストラクタ
	/// </summary>
	~LargeNumberOfObjects();

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="model">モデル</param>
	virtual void Initialize(Model* model);

	/// <summary>
	/// 更新
	/// </summary>
	virtual void Update();

	/// <summary>
	/// マッピング
	/// </summary>
	void Map();

	/// <summary>
	/// 描画
	/// </summary>
	/// <param name="camera">カメラ</param>
	void Draw(BaseCamera& camera);

	/// <summary>
	/// ノードデータ設定
	/// </summary>
	/// <param name="modelNode">モデルのノード</param>
	void SetNodeDatas(const ModelNode& modelNode, int32_t parentIndex);

	/// <summary>
	/// 死んでるのを削除
	/// </summary>
	void DeadDelete();

	/// <summary>
	/// オブジェクトの追加
	/// </summary>
	/// <param name="object">オブジェクト</param>
	void AddObject(OneOfManyObjects* object);

protected: // トランスフォーム関係

	// トランスフォームマトリックス
	Microsoft::WRL::ComPtr<ID3D12Resource> transformationMatrixesBuff_;
	//書き込むためのアドレスを取得
	TransformationMatrix* transformationMatrixesMap_{};
	// CPUハンドル
	D3D12_CPU_DESCRIPTOR_HANDLE transformationMatrixesHandleCPU_;
	// GPUハンドル
	D3D12_GPU_DESCRIPTOR_HANDLE transformationMatrixesHandleGPU_;
	// ディスクリプタヒープの位置
	uint32_t transformationMatrixesIndexDescriptorHeap_ = 0;

	// オブジェクト一つ
	std::list<OneOfManyObjects*> objects_;
	
	//インスタンス数
	uint32_t numInstance_ = 0;

protected: // モデル関係

	// ローカル行列
	Microsoft::WRL::ComPtr<ID3D12Resource> localMatrixesBuff_;
	//書き込むためのアドレスを取得
	LocalMatrix* localMatrixesMap_{};
	// CPUハンドル
	D3D12_CPU_DESCRIPTOR_HANDLE localMatrixesHandleCPU_;
	// GPUハンドル
	D3D12_GPU_DESCRIPTOR_HANDLE localMatrixesHandleGPU_;
	// ディスクリプタヒープの位置
	uint32_t localMatrixesIndexDescriptorHeap_ = 0;
	// ノードデータ
	std::vector<NodeData> nodeDatas_;

	// モデル
	Model* model_;

	//	マテリアル
	std::unique_ptr<Material> material_;

};

