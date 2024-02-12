#pragma once
#include "../3D/TransformStructure.h"
#include "../Math/Matrix4x4.h"
#include <wrl.h>
#include <d3d12.h>
#include "CameraForGPU.h"

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
	/// 
	/// </summary>
	/// <returns></returns>
	Matrix4x4 GetViewProjectionMatrix() { return viewProjectionMatrix_; }

	/// <summary>
	/// 
	/// </summary>
	/// <returns></returns>
	Matrix4x4 GetTransformMatrix() { return transformMatrix_; }

	/// <summary>
	/// 
	/// </summary>
	/// <returns></returns>
	Matrix4x4 GetViewMatrix() { return viewMatrix_; }

	/// <summary>
	/// 
	/// </summary>
	/// <returns></returns>
	Matrix4x4 GetProjectionMatrix() { return projectionMatrix_; }

	void SetTransform(const TransformStructure& transform) { transform_ = transform; }
	//TransformStructure GetTransform() { return transform_; }

	Vector3 GetTransform() { return transform_.translate; };

	Vector3 GetRotate() { return transform_.rotate; };

	ID3D12Resource* GetWorldPositionBuff() { return worldPositionBuff_.Get(); }



protected:

	TransformStructure transform_{ { 1.0f, 1.0f, 1.0f},{ 0.0f, 0.0f, 0.0f},{ 0.0f, 0.0f, -10.0f} };

	float fovY_;

	float aspectRatio_;

	float nearClip_;

	float farClip_;

	Matrix4x4 viewProjectionMatrix_;

	Matrix4x4 transformMatrix_;
	Matrix4x4 viewMatrix_;
	Matrix4x4 projectionMatrix_;

	//WVP用のリソースを作る。Matrix4x4 1つ分のサイズを用意する
	Microsoft::WRL::ComPtr<ID3D12Resource> worldPositionBuff_;
	//書き込むためのアドレスを取得
	CameraForGPU* worldPositionMap_{};

};

