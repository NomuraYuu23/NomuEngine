#pragma once
#include "../3D/TransformStructure.h"
#include "../Math/Matrix4x4.h"
#include <wrl.h>
#include <d3d12.h>
#include "CameraForGPU.h"
#include "ViewProjectionMatrix.h"

class BaseCamera
{

public:

	/// <summary>
	/// 初期化
	/// </summary>
	virtual void Initialize();

	/// <summary>
	/// 更新
	/// </summary>
	virtual void Update();

	/// <summary>
	/// ビュープロジェクション行列取得
	/// </summary>
	/// <returns>ビュープロジェクション行列</returns>
	Matrix4x4 GetViewProjectionMatrix() { return viewProjectionMatrix_->matrix; }

	/// <summary>
	/// トランスフォーム行列取得
	/// </summary>
	/// <returns>トランスフォーム行列</returns>
	Matrix4x4 GetTransformMatrix() { return transformMatrix_; }

	/// <summary>
	/// ビュー行列取得
	/// </summary>
	/// <returns>ビュー行列</returns>
	Matrix4x4 GetViewMatrix() { return viewMatrix_; }

	/// <summary>
	/// プロジェクション行列取得
	/// </summary>
	/// <returns>プロジェクション行列</returns>
	Matrix4x4 GetProjectionMatrix() { return projectionMatrix_; }

	/// <summary>
	/// トランスフォーム構造体設定
	/// </summary>
	/// <param name="transform">トランスフォーム構造体</param>
	void SetTransform(const TransformStructure& transform) { transform_ = transform; }

	/// <summary>
	/// ローカル位置取得
	/// </summary>
	/// <returns>位置</returns>
	Vector3 GetTranslate() { return transform_.translate; };

	/// <summary>
	/// ローカル回転取得
	/// </summary>
	/// <returns>回転</returns>
	Vector3 GetRotate() { return transform_.rotate; };

	/// <summary>
	/// ワールドポジションバッファ取得
	/// </summary>
	/// <returns>ワールドポジションバッファ</returns>
	ID3D12Resource* GetWorldPositionBuff() { return worldPositionBuff_.Get(); }
	
	/// <summary>
	/// ビュープロジェクション行列バッファ取得
	/// </summary>
	/// <returns>ビュープロジェクション行列バッファ</returns>
	ID3D12Resource* GetViewProjectionMatriBuff() { return viewProjectionMatrixBuff_.Get(); }



protected:

	// トランスフォーム
	TransformStructure transform_{ { 1.0f, 1.0f, 1.0f},{ 0.0f, 0.0f, 0.0f},{ 0.0f, 0.0f, -10.0f} };

	// 透視投影で使う
	float fovY_;

	// アスペクト比
	float aspectRatio_;

	// 透視投影で使う
	float nearClip_;

	// 透視投影で使う
	float farClip_;

	// VP用のリソース
	Microsoft::WRL::ComPtr<ID3D12Resource> viewProjectionMatrixBuff_;

	// 書き込むためのアドレスを取得
	ViewProjectionMatrix* viewProjectionMatrix_;

	// WP用のリソース
	Microsoft::WRL::ComPtr<ID3D12Resource> worldPositionBuff_;
	// 書き込むためのアドレスを取得
	CameraForGPU* worldPositionMap_{};

	// トランスフォーム行列
	Matrix4x4 transformMatrix_;
	// ビュー行列
	Matrix4x4 viewMatrix_;
	// プロジェクション行列
	Matrix4x4 projectionMatrix_;

};

