#pragma once
#include "TransformStructure.h"
#include "../Math/Matrix4x4.h"

#include "TransformationMatrix.h"
#include <wrl.h>
#include <d3d12.h>

struct WorldTransform
{

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


	void Initialize();

	void UpdateMatrix();

	void Map(const Matrix4x4& viewProjectionMatrix);

	void MapSprite();

	Vector3 GetWorldPosition();

	void SetParent(WorldTransform* parent) { parent_ = parent; }

};
