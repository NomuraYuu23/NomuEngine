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

	Update();

}

void BaseCamera::Update()
{

	Matrix4x4Calc* calc = Matrix4x4Calc::GetInstance();

	transformMatrix_ = calc->MakeAffineMatrix(transform_.scale, transform_.rotate, transform_.translate);
	viewMatrix_ = calc->Inverse(transformMatrix_);
	projectionMatrix_ = calc->MakePerspectiveFovMatrix(fovY_, aspectRatio_, nearClip_, farClip_);

	viewProjectionMatrix_ = calc->Multiply(viewMatrix_, projectionMatrix_);

	worldPositionMap_->worldPosition = { transformMatrix_.m[3][0],transformMatrix_.m[3][1], transformMatrix_.m[3][2] };

}
