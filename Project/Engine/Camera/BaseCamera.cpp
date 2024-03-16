#include "BaseCamera.h"
#include "../base/WinApp.h"
#include "../base/BufferResource.h"
#include "../base/DirectXCommon.h"

void BaseCamera::Initialize()
{

	transform_ = { { 1.0f, 1.0f, 1.0f},{ 0.0f, 0.0f, 0.0f},{ 0.0f, 0.0f, -10.0f} };

	fovY_ = 0.45f;

	aspectRatio_ = float(WinApp::kWindowWidth) / float(WinApp::kWindowHeight);

	nearClip_ = 0.1f;

	farClip_ = 1000.0f;

	//WVP用のリソースを作る。Matrix4x4 1つ分のサイズを用意する
	worldPositionBuff_ = BufferResource::CreateBufferResource(DirectXCommon::GetInstance()->GetDevice(), (sizeof(CameraForGPU) + 0xff) & ~0xff);
	//書き込むためのアドレスを取得
	worldPositionBuff_->Map(0, nullptr, reinterpret_cast<void**>(&worldPositionMap_));

	//WVP用のリソースを作る。Matrix4x4 1つ分のサイズを用意する
	viewProjectionMatrixBuff_ = BufferResource::CreateBufferResource(DirectXCommon::GetInstance()->GetDevice(), (sizeof(ViewProjectionMatrix) + 0xff) & ~0xff);
	viewProjectionMatrixBuff_->Map(0, nullptr, reinterpret_cast<void**>(&viewProjectionMatrix_));

	BaseCamera::Update();

}

void BaseCamera::Update()
{

	transformMatrix_ = Matrix4x4::MakeAffineMatrix(transform_.scale, transform_.rotate, transform_.translate);
	viewMatrix_ = Matrix4x4::Inverse(transformMatrix_);
	projectionMatrix_ = Matrix4x4::MakePerspectiveFovMatrix(fovY_, aspectRatio_, nearClip_, farClip_);

	viewProjectionMatrix_->matrix = Matrix4x4::Multiply(viewMatrix_, projectionMatrix_);

	worldPositionMap_->worldPosition = { transformMatrix_.m[3][0],transformMatrix_.m[3][1], transformMatrix_.m[3][2] };

}
