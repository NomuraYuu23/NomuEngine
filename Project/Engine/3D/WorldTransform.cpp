#include "WorldTransform.h"

#include "../base/BufferResource.h"
#include "../base/WinApp.h"
#include "../base/DirectXCommon.h"
#include "../base/DescriptorHerpManager.h"

// コマンドリスト
ID3D12GraphicsCommandList* WorldTransform::sCommandList = nullptr;

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


	//WVP用のリソースを作る。
	transformationMatrixesBuff_ = BufferResource::CreateBufferResource(DirectXCommon::GetInstance()->GetDevice(), ((sizeof(TransformationMatrix) + 0xff) & ~0xff) * 1);
	//書き込むためのアドレスを取得
	transformationMatrixesBuff_->Map(0, nullptr, reinterpret_cast<void**>(&transformationMatrixesMap_));

	transformationMatrixesMap_->World = matrix4x4Calc->MakeIdentity4x4();
	transformationMatrixesMap_->WVP = matrix4x4Calc->MakeIdentity4x4();
	transformationMatrixesMap_->WorldInverseTranspose = matrix4x4Calc->MakeIdentity4x4();
	transformationMatrixesMap_->ScaleInverse = matrix4x4Calc->MakeIdentity4x4();

	SRVCreate();

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

void WorldTransform::Initialize(const ModelNode& modelNode)
{

	SetNodeDatas(modelNode);

	Matrix4x4Calc* matrix4x4Calc = Matrix4x4Calc::GetInstance();

	// 回転行列
	rotateMatrix_ = matrix4x4Calc->MakeRotateXYZMatrix(transform_.rotate);

	// 方向ベクトルで回転行列
	usedDirection_ = false;

	// スケールを考えない
	parentMatrix_ = matrix4x4Calc->MakeAffineMatrix(Vector3{ 1.0f,1.0f,1.0f }, transform_.rotate, transform_.translate);

	//WVP用のリソースを作る。Matrix4x4 1つ分のサイズを用意する
	transformationMatrixBuff_ = BufferResource::CreateBufferResource(DirectXCommon::GetInstance()->GetDevice(), (sizeof(TransformationMatrix) + 0xff) & ~0xff);
	//書き込むためのアドレスを取得
	transformationMatrixBuff_->Map(0, nullptr, reinterpret_cast<void**>(&transformationMatrixMap_));

	transformationMatrixMap_->World = matrix4x4Calc->MakeIdentity4x4();
	transformationMatrixMap_->WVP = matrix4x4Calc->MakeIdentity4x4();
	transformationMatrixMap_->WorldInverseTranspose = matrix4x4Calc->MakeIdentity4x4();
	transformationMatrixMap_->ScaleInverse = matrix4x4Calc->MakeIdentity4x4();

	// ノードカウント
	uint32_t nodeCount = static_cast<uint32_t>(nodeDatas_.size());
	assert(nodeCount);

	//WVP用のリソースを作る。
	transformationMatrixesBuff_ = BufferResource::CreateBufferResource(DirectXCommon::GetInstance()->GetDevice(), ((sizeof(TransformationMatrix) + 0xff) & ~0xff) * nodeCount);
	//書き込むためのアドレスを取得
	transformationMatrixesBuff_->Map(0, nullptr, reinterpret_cast<void**>(&transformationMatrixesMap_));

	for (uint32_t i = 0; i < nodeCount; ++i) {
		transformationMatrixesMap_[nodeDatas_[i].meshNum].World = matrix4x4Calc->MakeIdentity4x4();
		transformationMatrixesMap_[nodeDatas_[i].meshNum].WVP = matrix4x4Calc->MakeIdentity4x4();
		transformationMatrixesMap_[nodeDatas_[i].meshNum].WorldInverseTranspose = matrix4x4Calc->MakeIdentity4x4();
		transformationMatrixesMap_[nodeDatas_[i].meshNum].ScaleInverse = matrix4x4Calc->MakeIdentity4x4();
	}

	UpdateMatrix();

	SRVCreate();

}

void WorldTransform::TmpMap(const Matrix4x4& viewProjectionMatrix, const Matrix4x4& modelLocalMatrix)
{

	Matrix4x4Calc* matrix4x4Calc = Matrix4x4Calc::GetInstance();

	for (uint32_t i = 0; i < nodeDatas_.size(); ++i) {
		transformationMatrixesMap_[nodeDatas_[i].meshNum].World = matrix4x4Calc->Multiply(nodeDatas_[i].localMatrix, worldMatrix_);
		transformationMatrixesMap_[nodeDatas_[i].meshNum].WVP = matrix4x4Calc->Multiply(matrix4x4Calc->Multiply(nodeDatas_[i].localMatrix, worldMatrix_), viewProjectionMatrix);
		transformationMatrixesMap_[nodeDatas_[i].meshNum].WorldInverseTranspose = matrix4x4Calc->Multiply(nodeDatas_[i].localMatrix, matrix4x4Calc->Inverse(worldMatrix_));

		transformationMatrixesMap_[nodeDatas_[i].meshNum].ScaleInverse = matrix4x4Calc->Inverse(matrix4x4Calc->MakeScaleMatrix(transform_.scale)); // objファイルのみ対応
	}

	if (nodeDatas_.size() == 0) {
		transformationMatrixesMap_[0].World = matrix4x4Calc->Multiply(modelLocalMatrix, worldMatrix_);
		transformationMatrixesMap_[0].WVP = matrix4x4Calc->Multiply(matrix4x4Calc->Multiply(modelLocalMatrix, worldMatrix_), viewProjectionMatrix);
		transformationMatrixesMap_[0].WorldInverseTranspose = matrix4x4Calc->Multiply(modelLocalMatrix, matrix4x4Calc->Inverse(worldMatrix_));

		transformationMatrixesMap_[0].ScaleInverse = matrix4x4Calc->Inverse(matrix4x4Calc->MakeScaleMatrix(transform_.scale)); // objファイルのみ対応

	}
}

void WorldTransform::SRVCreate()
{

	D3D12_SHADER_RESOURCE_VIEW_DESC instancingSrvDesc{};
	instancingSrvDesc.Format = DXGI_FORMAT_UNKNOWN;
	instancingSrvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	instancingSrvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
	instancingSrvDesc.Buffer.FirstElement = 0;
	instancingSrvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;

	if (nodeDatas_.size() == 0){
		instancingSrvDesc.Buffer.NumElements = 1;
	}
	else {
		instancingSrvDesc.Buffer.NumElements = static_cast<UINT>(nodeDatas_.size());
	}

	instancingSrvDesc.Buffer.StructureByteStride = sizeof(TransformationMatrix);
	instancingSrvHandleCPU_ = DescriptorHerpManager::GetCPUDescriptorHandle();
	instancingSrvHandleGPU_ = DescriptorHerpManager::GetGPUDescriptorHandle();
	DescriptorHerpManager::NextIndexDescriptorHeapChange();
	DirectXCommon::GetInstance()->GetDevice()->CreateShaderResourceView(transformationMatrixesBuff_.Get(), &instancingSrvDesc, instancingSrvHandleCPU_);

}

void WorldTransform::SetGraphicsRootDescriptorTable(ID3D12GraphicsCommandList* cmdList, uint32_t rootParameterIndex)
{

	assert(sCommandList == nullptr);

	sCommandList = cmdList;

	sCommandList->SetGraphicsRootDescriptorTable(rootParameterIndex, instancingSrvHandleGPU_);

	// コマンドリストを解除
	sCommandList = nullptr;

}

void WorldTransform::SetNodeDatas(const ModelNode& modelNode)
{

	NodeData nodeData;

	nodeData.localMatrix = modelNode.localMatrix;
	nodeData.meshNum = modelNode.meshNum;
	if(nodeData.meshNum != -1){
		nodeDatas_.push_back(nodeData);
	}

	for (uint32_t childIndex = 0; childIndex < modelNode.children.size(); ++childIndex) {
		// 再帰的に読んで階層構造を作る
		SetNodeDatas(modelNode.children[childIndex]);
	}

}

void WorldTransform::Map(const Matrix4x4& viewProjectionMatrix, const Matrix4x4& modelLocalMatrix)
{

	Matrix4x4Calc* matrix4x4Calc = Matrix4x4Calc::GetInstance();

	transformationMatrixMap_->World = matrix4x4Calc->Multiply(modelLocalMatrix, worldMatrix_);
	transformationMatrixMap_->WVP = matrix4x4Calc->Multiply(matrix4x4Calc->Multiply(modelLocalMatrix, worldMatrix_), viewProjectionMatrix);
	transformationMatrixMap_->WorldInverseTranspose = matrix4x4Calc->Multiply(modelLocalMatrix, matrix4x4Calc->Inverse(worldMatrix_));

	transformationMatrixMap_->ScaleInverse = matrix4x4Calc->Inverse(matrix4x4Calc->MakeScaleMatrix(transform_.scale)); // objファイルのみ対応

	// お試し
	TmpMap(viewProjectionMatrix, modelLocalMatrix);

}
