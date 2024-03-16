#pragma once
#include "TransformStructure.h"
#include "../Math/Matrix4x4.h"

#include "TransformationMatrix.h"
#include <wrl.h>
#include <d3d12.h>
#include "ModelNode.h"
#include "../Math/Quaternion.h"

class WorldTransform
{

public: // 構造体

	/// <summary>
	/// ノードデータ
	/// </summary>
	struct NodeData
	{
		Matrix4x4 localMatrix; // ローカル行列
		uint32_t meshNum; // メッシュ番号
		std::string name; // 名前
		//WorldTransform::NodeData* parent; // 親
		int32_t parentIndex; // 親番号
		Matrix4x4 matrix; //最終的なワールド行列
		Matrix4x4 offsetMatrix; // オフセット
	};

public: // 変数

	// コマンドリスト
	static ID3D12GraphicsCommandList* sCommandList;

public: // 関数

	/// <summary>
	/// デストラクタ
	/// </summary>
	~WorldTransform();

	/// <summary>
	/// 初期化(モデル表示で使わない)
	/// </summary>
	void Initialize();

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="modelNode"></param>
	void Initialize(const ModelNode& modelNode);

	/// <summary>
	/// 行列更新
	/// </summary>
	void UpdateMatrix();

	/// <summary>
	/// マップ
	/// </summary>
	/// <param name="viewProjectionMatrix">ビュープロジェクション</param>
	void Map(const Matrix4x4& viewProjectionMatrix);

	/// <summary>
	/// SRVを作る
	/// </summary>
	void SRVCreate();

	/// <summary>
	/// GPUに送る
	/// </summary>
	/// <param name="cmdList">コマンドリスト</param>
	/// <param name="rootParameterIndex">ルートパラメータインデックス</param>
	void SetGraphicsRootDescriptorTable(ID3D12GraphicsCommandList* cmdList, uint32_t rootParameterIndex);

	/// <summary>
	/// ノードデータ設定
	/// </summary>
	/// <param name="modelNode">モデルのノード</param>
	void SetNodeDatas(const ModelNode& modelNode, int32_t parentIndex);

	/// <summary>
	/// 消す前に行う処理
	/// </summary>
	void Finalize();

	/// <summary>
	/// ワールドポジション取得
	/// </summary>
	/// <returns></returns>
	Vector3 GetWorldPosition();

	/// <summary>
	/// 親設定
	/// </summary>
	/// <param name="parent"></param>
	void SetParent(WorldTransform* parent) { parent_ = parent; }

	/// <summary>
	/// ノードデータ取得
	/// </summary>
	/// <returns></returns>
	std::vector<NodeData> GetNodeDatas() { return nodeDatas_; }

	/// <summary>
	/// ノード名前
	/// </summary>
	/// <returns></returns>
	std::vector<std::string> GetNodeNames();

	/// <summary>
	/// ローカル行列設定
	/// </summary>
	void SetNodeLocalMatrix(const std::vector<Matrix4x4> matrix);

private: // 変数

	//トランスフォーム
	TransformStructure transform_{ {1.0f, 1.0f, 1.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f} };

	// クォータニオン
	Quaternion quaternion_ = { 0.0f,0.0f,0.0f,1.0f };

	//ワールド行列
	Matrix4x4 worldMatrix_;

	// 回転行列
	Matrix4x4 rotateMatrix_;

	// スケールを考えない
	Matrix4x4 parentMatrix_;

	//親
	WorldTransform* parent_ = nullptr;

	//WVP用のリソースを作る。
	Microsoft::WRL::ComPtr<ID3D12Resource> transformationMatrixesBuff_;
	
	//書き込むためのアドレスを取得
	TransformationMatrix* transformationMatrixesMap_{};

	// CPUハンドル
	D3D12_CPU_DESCRIPTOR_HANDLE instancingSrvHandleCPU_;

	// GPUハンドル
	D3D12_GPU_DESCRIPTOR_HANDLE instancingSrvHandleGPU_;

	// ディスクリプタヒープの位置
	uint32_t indexDescriptorHeap_ = 0;

	// ノードデータ
	std::vector<NodeData> nodeDatas_;

public: // アクセッサ

	/// <summary>
	/// 位置取得
	/// </summary>
	/// <returns></returns>
	Vector3 GetTranslate() { return transform_.translate; }

	/// <summary>
	/// 位置設定
	/// </summary>
	/// <param name="translate"></param>
	void SetTranslate(const Vector3 translate) {  transform_.translate = translate; }

	/// <summary>
	/// 回転取得
	/// </summary>
	Vector3 GetRoatateVector() { return transform_.rotate; }

	/// <summary>
	/// 回転取得
	/// </summary>
	Quaternion GetQuaternion() { return quaternion_; }

	/// <summary>
	/// 回転設定
	/// </summary>
	/// <param name="rotate"></param>
	void SetRoatate(const Vector3 rotate);

	/// <summary>
	/// 回転設定
	/// </summary>
	/// <param name="quaternion"></param>
	void SetRoatate(const Quaternion quaternion);

	/// <summary>
	/// スケール取得
	/// </summary>
	/// <returns></returns>
	Vector3 GetScale() { return transform_.scale; }

	/// <summary>
	/// スケール設定
	/// </summary>
	/// <param name="scale">スケール</param>
	void SetScale(const Vector3 scale) { transform_.scale = scale; };

};
