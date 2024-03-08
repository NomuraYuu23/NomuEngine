#include "WorldTransform.h"

#include "../base/BufferResource.h"
#include "../base/WinApp.h"
#include "../base/DirectXCommon.h"
#include "../base/DescriptorHerpManager.h"

// コマンドリスト
ID3D12GraphicsCommandList* WorldTransform::sCommandList = nullptr;

WorldTransform::~WorldTransform()
{

	Finalize();

}

void WorldTransform::Initialize()
{

	// 回転行列
	rotateMatrix_ = Matrix4x4::MakeRotateXYZMatrix(transform_.rotate);

	// 方向ベクトルで回転行列
	usedDirection_ = false;

	// スケールを考えない
	parentMatrix_ = Matrix4x4::MakeAffineMatrix(Vector3{ 1.0f,1.0f,1.0f }, transform_.rotate, transform_.translate);

	UpdateMatrix();

}

void WorldTransform::Initialize(const ModelNode& modelNode)
{

	SetNodeDatas(modelNode, nullptr);

	// 回転行列
	rotateMatrix_ = Matrix4x4::MakeRotateXYZMatrix(transform_.rotate);

	// 方向ベクトルで回転行列
	usedDirection_ = false;

	// スケールを考えない
	parentMatrix_ = Matrix4x4::MakeAffineMatrix(Vector3{ 1.0f,1.0f,1.0f }, transform_.rotate, transform_.translate);

	// ノードカウント
	uint32_t nodeCount = static_cast<uint32_t>(nodeDatas_.size());
	assert(nodeCount);

	//WVP用のリソースを作る。
	transformationMatrixesBuff_ = BufferResource::CreateBufferResource(DirectXCommon::GetInstance()->GetDevice(), ((sizeof(TransformationMatrix) + 0xff) & ~0xff) * nodeCount);
	//書き込むためのアドレスを取得
	transformationMatrixesBuff_->Map(0, nullptr, reinterpret_cast<void**>(&transformationMatrixesMap_));

	for (uint32_t i = 0; i < nodeCount; ++i) {
		transformationMatrixesMap_[nodeDatas_[i].meshNum].World = Matrix4x4::MakeIdentity4x4();
		transformationMatrixesMap_[nodeDatas_[i].meshNum].WVP = Matrix4x4::MakeIdentity4x4();
		transformationMatrixesMap_[nodeDatas_[i].meshNum].WorldInverseTranspose = Matrix4x4::MakeIdentity4x4();
		transformationMatrixesMap_[nodeDatas_[i].meshNum].ScaleInverse = Matrix4x4::MakeIdentity4x4();
	}

	UpdateMatrix();

	SRVCreate();

}

void WorldTransform::UpdateMatrix() {

	//拡大縮小行列
	Matrix4x4 scaleMatrix = Matrix4x4::MakeScaleMatrix(transform_.scale);
	// どう回転行列作るか
	if (usedDirection_) {
		// 回転行列
		rotateMatrix_ = Matrix4x4::DirectionToDirection(Vector3{0.0f,0.0f,1.0f}, direction_);
	}
	else {
		// 回転行列
		rotateMatrix_ = Matrix4x4::MakeRotateXYZMatrix(transform_.rotate);
	}

	//平行移動行列
	Matrix4x4 translateMatrix = Matrix4x4::MakeTranslateMatrix(transform_.translate);

	// ワールド行列
	worldMatrix_ = Matrix4x4::Multiply(scaleMatrix, Matrix4x4::Multiply(rotateMatrix_, translateMatrix));

	//拡大縮小行列
	scaleMatrix = Matrix4x4::MakeScaleMatrix(Vector3{ 1.0f,1.0f,1.0f });
	// 親子関係用
	parentMatrix_ = Matrix4x4::Multiply(scaleMatrix, Matrix4x4::Multiply(rotateMatrix_, translateMatrix));

	// 親子関係
	if (parent_) {
		worldMatrix_ = Matrix4x4::Multiply(worldMatrix_, parent_->parentMatrix_);
		parentMatrix_ = Matrix4x4::Multiply(parentMatrix_, parent_->parentMatrix_);
	}

}

void WorldTransform::UpdateMatrix(const Matrix4x4& rotateMatrix)
{

	//拡大縮小行列
	Matrix4x4 scaleMatrix = Matrix4x4::MakeScaleMatrix(transform_.scale);

	// 回転行列
	rotateMatrix_ = rotateMatrix;

	//平行移動行列
	Matrix4x4 translateMatrix = Matrix4x4::MakeTranslateMatrix(transform_.translate);

	// ワールド行列
	worldMatrix_ = Matrix4x4::Multiply(scaleMatrix, Matrix4x4::Multiply(rotateMatrix_, translateMatrix));

	//拡大縮小行列
	scaleMatrix = Matrix4x4::MakeScaleMatrix(Vector3{ 1.0f,1.0f,1.0f });
	// 親子関係用
	parentMatrix_ = Matrix4x4::Multiply(scaleMatrix, Matrix4x4::Multiply(rotateMatrix_, translateMatrix));

	// 親子関係
	if (parent_) {
		worldMatrix_ = Matrix4x4::Multiply(worldMatrix_, parent_->parentMatrix_);
		parentMatrix_ = Matrix4x4::Multiply(parentMatrix_, parent_->parentMatrix_);
	}

}

void WorldTransform::Map(const Matrix4x4& viewProjectionMatrix)
{

	for (uint32_t i = 0; i < nodeDatas_.size(); ++i) {
		transformationMatrixesMap_[nodeDatas_[i].meshNum].World = Matrix4x4::Multiply(nodeDatas_[i].localMatrix, worldMatrix_);
		transformationMatrixesMap_[nodeDatas_[i].meshNum].WVP = Matrix4x4::Multiply(Matrix4x4::Multiply(nodeDatas_[i].localMatrix, worldMatrix_), viewProjectionMatrix);
		transformationMatrixesMap_[nodeDatas_[i].meshNum].WorldInverseTranspose = Matrix4x4::Multiply(nodeDatas_[i].localMatrix, Matrix4x4::Inverse(worldMatrix_));

		transformationMatrixesMap_[nodeDatas_[i].meshNum].ScaleInverse = Matrix4x4::Inverse(Matrix4x4::MakeScaleMatrix(transform_.scale)); // objファイルのみ対応
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
		assert(0);
	}
	else {
		instancingSrvDesc.Buffer.NumElements = static_cast<UINT>(nodeDatas_.size());
	}

	instancingSrvDesc.Buffer.StructureByteStride = sizeof(TransformationMatrix);
	instancingSrvHandleCPU_ = DescriptorHerpManager::GetCPUDescriptorHandle();
	instancingSrvHandleGPU_ = DescriptorHerpManager::GetGPUDescriptorHandle();
	indexDescriptorHeap_ = DescriptorHerpManager::GetNextIndexDescriptorHeap();
	DescriptorHerpManager::NextIndexDescriptorHeapChange();
	DirectXCommon::GetInstance()->GetDevice()->CreateShaderResourceView(transformationMatrixesBuff_.Get(), &instancingSrvDesc, instancingSrvHandleCPU_);

}

void WorldTransform::SetGraphicsRootDescriptorTable(ID3D12GraphicsCommandList* cmdList, uint32_t rootParameterIndex)
{

	assert(sCommandList == nullptr);
	assert(nodeDatas_.size() > 0);

	sCommandList = cmdList;

	sCommandList->SetGraphicsRootDescriptorTable(rootParameterIndex, instancingSrvHandleGPU_);

	// コマンドリストを解除
	sCommandList = nullptr;

}

void WorldTransform::SetNodeDatas(const ModelNode& modelNode, NodeData* parent)
{

	NodeData nodeData;

	nodeData.localMatrix = modelNode.localMatrix;
	nodeData.meshNum = modelNode.meshNum;
	nodeData.name = modelNode.name;
	nodeData.parent = parent;
	if(nodeData.meshNum != -1){
		nodeDatas_.push_back(nodeData);
	}

	size_t parentIndex = nodeDatas_.size() - 1;

	for (uint32_t childIndex = 0; childIndex < modelNode.children.size(); ++childIndex) {
		// 再帰的に読んで階層構造を作る
		if (parentIndex == -1) {
			SetNodeDatas(modelNode.children[childIndex], nullptr);
		}
		else {
			SetNodeDatas(modelNode.children[childIndex], &nodeDatas_[parentIndex]);
		}

	}

}

void WorldTransform::Finalize()
{
	if (nodeDatas_.size() > 0) {
		DescriptorHerpManager::DescriptorHeapsMakeNull(indexDescriptorHeap_);
	}

}

Vector3 WorldTransform::GetWorldPosition()
{

	Vector3 position;

	position.x = worldMatrix_.m[3][0];
	position.y = worldMatrix_.m[3][1];
	position.z = worldMatrix_.m[3][2];

	return position;

}

std::vector<std::string> WorldTransform::GetNodeNames()
{

	std::vector<std::string> result;

	for (uint32_t i = 0; i < nodeDatas_.size(); ++i) {
		result.push_back(nodeDatas_[i].name);
	}


	return result;
}

void WorldTransform::SetNodeLocalMatrix(const std::vector<Matrix4x4> matrix)
{

	assert(matrix.size() == nodeDatas_.size());

	for (uint32_t i = 0; i < matrix.size(); ++i) {
		nodeDatas_[i].localMatrix = matrix[i];
	}

}
