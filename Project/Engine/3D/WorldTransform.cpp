#include "WorldTransform.h"

#include "../base/BufferResource.h"
#include "../base/WinApp.h"
#include "../base/DirectXCommon.h"

void WorldTransform::Initialize() {

	Matrix4x4Calc* matrix4x4Calc = Matrix4x4Calc::GetInstance();

	// 回転行列
	rotateMatrix_ = matrix4x4Calc->MakeRotateXYZMatrix(transform_.rotate);

	// 方向ベクトルで回転行列
	usedDirection_ = false;

	// スケールを考えない
	parentMatrix_ = matrix4x4Calc->MakeAffineMatrix(Vector3{1.0f,1.0f,1.0f}, transform_.rotate, transform_.translate);

	//WVP用のリソースを作る。Matrix4x4 1つ分のサイズを用意する
	transformationMatrixBuff_ = BufferResource::CreateBufferResource(DirectXCommon::GetInstance()->GetDevice(), (sizeof(TransformationMatrix) + 0xff) & ~0xff);
	//書き込むためのアドレスを取得
	transformationMatrixBuff_->Map(0, nullptr, reinterpret_cast<void**>(&transformationMatrixMap_));

	transformationMatrixMap_->World = matrix4x4Calc->MakeIdentity4x4();
	transformationMatrixMap_->WVP = matrix4x4Calc->MakeIdentity4x4();
	transformationMatrixMap_->WorldInverseTranspose = matrix4x4Calc->MakeIdentity4x4();
	transformationMatrixMap_->ScaleInverse = matrix4x4Calc->MakeIdentity4x4();
	UpdateMatrix();

}

void WorldTransform::UpdateMatrix() {

	Matrix4x4Calc* matrix4x4Calc = Matrix4x4Calc::GetInstance();

	//拡大縮小行列
	Matrix4x4 scaleMatrix = matrix4x4Calc->MakeScaleMatrix(transform_.scale);
	// どう回転行列作るか
	if (usedDirection_) {
		// 回転行列
		rotateMatrix_ = matrix4x4Calc->DirectionToDirection(Vector3{0.0f,0.0f,1.0f}, direction_);
	}
	else {
		// 回転行列
		rotateMatrix_ = matrix4x4Calc->MakeRotateXYZMatrix(transform_.rotate);
	}

	//平行移動行列
	Matrix4x4 translateMatrix = matrix4x4Calc->MakeTranslateMatrix(transform_.translate);

	// ワールド行列
	worldMatrix_ = matrix4x4Calc->Multiply(scaleMatrix, matrix4x4Calc->Multiply(rotateMatrix_, translateMatrix));

	//拡大縮小行列
	scaleMatrix = matrix4x4Calc->MakeScaleMatrix(Vector3{ 1.0f,1.0f,1.0f });
	// 親子関係用
	parentMatrix_ = matrix4x4Calc->Multiply(scaleMatrix, matrix4x4Calc->Multiply(rotateMatrix_, translateMatrix));

	// 親子関係
	if (parent_) {
		worldMatrix_ = matrix4x4Calc->Multiply(worldMatrix_, parent_->parentMatrix_);
		parentMatrix_ = matrix4x4Calc->Multiply(parentMatrix_, parent_->parentMatrix_);
	}

}

void WorldTransform::MapSprite()
{

	Matrix4x4Calc* matrix4x4Calc = Matrix4x4Calc::GetInstance();

	//Sprite用のWorldViewProjectionMatrixを作る
	Matrix4x4 viewMatrixSprite = matrix4x4Calc->MakeIdentity4x4();
	Matrix4x4 projectionMatrixSprite = matrix4x4Calc->MakeOrthographicMatrix(0.0f, 0.0f, float(WinApp::kWindowWidth), float(WinApp::kWindowHeight), 0.0f, 100.0f);
	Matrix4x4 worldViewProjectionMatrixSprite = matrix4x4Calc->Multiply(worldMatrix_, matrix4x4Calc->Multiply(viewMatrixSprite, projectionMatrixSprite));
	transformationMatrixMap_->WVP = worldViewProjectionMatrixSprite;
	transformationMatrixMap_->World = worldMatrix_;
	transformationMatrixMap_->WorldInverseTranspose = matrix4x4Calc->Inverse(worldMatrix_);

}

Vector3 WorldTransform::GetWorldPosition()
{

	Vector3 position;

	position.x = worldMatrix_.m[3][0];
	position.y = worldMatrix_.m[3][1];
	position.z = worldMatrix_.m[3][2];

	return position;

}

void WorldTransform::Map(const Matrix4x4& viewProjectionMatrix)
{

	Matrix4x4Calc* matrix4x4Calc = Matrix4x4Calc::GetInstance();

	transformationMatrixMap_->World = worldMatrix_;
	transformationMatrixMap_->WVP = matrix4x4Calc->Multiply(worldMatrix_, viewProjectionMatrix);
	transformationMatrixMap_->WorldInverseTranspose = matrix4x4Calc->Inverse(worldMatrix_);
	transformationMatrixMap_->ScaleInverse = matrix4x4Calc->Inverse(matrix4x4Calc->MakeScaleMatrix(transform_.scale));
}
