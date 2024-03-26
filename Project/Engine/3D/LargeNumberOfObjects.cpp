#include "LargeNumberOfObjects.h"
#include "../base/SRVDescriptorHerpManager.h"

LargeNumberOfObjects::~LargeNumberOfObjects()
{

	objects_.remove_if([](OneOfManyObjects* object) {
		delete object;
		return true;
		});

}

void LargeNumberOfObjects::Initialize(Model* model)
{

	// モデル
	model_ = model;
	
	// マテリアル
	material_.reset(Material::Create());

	// ノードデータ
	SetNodeDatas(model_->GetRootNode(), -1);
	uint32_t nodeCount = static_cast<uint32_t>(nodeDatas_.size());
	assert(nodeCount);

	// ローカル行列
	localMatrixesBuff_ = BufferResource::CreateBufferResource(DirectXCommon::GetInstance()->GetDevice(), ((sizeof(LocalMatrix) + 0xff) & ~0xff) * nodeCount);
	localMatrixesBuff_->Map(0, nullptr, reinterpret_cast<void**>(&localMatrixesMap_));

	for (uint32_t i = 0; i < nodeCount; ++i) {
		localMatrixesMap_[nodeDatas_[i].meshNum].matrix = Matrix4x4::MakeIdentity4x4();
	}

	D3D12_SHADER_RESOURCE_VIEW_DESC localMatrixesSrvDesc{};
	localMatrixesSrvDesc.Format = DXGI_FORMAT_UNKNOWN;
	localMatrixesSrvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	localMatrixesSrvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
	localMatrixesSrvDesc.Buffer.FirstElement = 0;
	localMatrixesSrvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
	localMatrixesSrvDesc.Buffer.NumElements = static_cast<UINT>(nodeDatas_.size());
	localMatrixesSrvDesc.Buffer.StructureByteStride = sizeof(LocalMatrix);

	localMatrixesHandleCPU_ = SRVDescriptorHerpManager::GetCPUDescriptorHandle();
	localMatrixesHandleGPU_ = SRVDescriptorHerpManager::GetGPUDescriptorHandle();
	localMatrixesIndexDescriptorHeap_ = SRVDescriptorHerpManager::GetNextIndexDescriptorHeap();
	SRVDescriptorHerpManager::NextIndexDescriptorHeapChange();
	DirectXCommon::GetInstance()->GetDevice()->CreateShaderResourceView(localMatrixesBuff_.Get(), &localMatrixesSrvDesc, localMatrixesHandleCPU_);

	// トランスフォームマトリックス
	transformationMatrixesBuff_ = BufferResource::CreateBufferResource(DirectXCommon::GetInstance()->GetDevice(), ((sizeof(TransformationMatrix) + 0xff) & ~0xff) * kNumInstanceMax_);
	transformationMatrixesBuff_->Map(0, nullptr, reinterpret_cast<void**>(&transformationMatrixesMap_));

	for (uint32_t i = 0; i < kNumInstanceMax_; ++i) {
		transformationMatrixesMap_[i].World = Matrix4x4::MakeIdentity4x4();
		transformationMatrixesMap_[i].WorldInverseTranspose = Matrix4x4::MakeIdentity4x4();
	}

	D3D12_SHADER_RESOURCE_VIEW_DESC transformationMatrixesSrvDesc{};
	transformationMatrixesSrvDesc.Format = DXGI_FORMAT_UNKNOWN;
	transformationMatrixesSrvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	transformationMatrixesSrvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
	transformationMatrixesSrvDesc.Buffer.FirstElement = 0;
	transformationMatrixesSrvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
	transformationMatrixesSrvDesc.Buffer.NumElements = kNumInstanceMax_;
	transformationMatrixesSrvDesc.Buffer.StructureByteStride = sizeof(TransformationMatrix);

	transformationMatrixesHandleCPU_ = SRVDescriptorHerpManager::GetCPUDescriptorHandle();
	transformationMatrixesHandleGPU_ = SRVDescriptorHerpManager::GetGPUDescriptorHandle();
	transformationMatrixesIndexDescriptorHeap_ = SRVDescriptorHerpManager::GetNextIndexDescriptorHeap();
	SRVDescriptorHerpManager::NextIndexDescriptorHeapChange();
	DirectXCommon::GetInstance()->GetDevice()->CreateShaderResourceView(transformationMatrixesBuff_.Get(), &transformationMatrixesSrvDesc, transformationMatrixesHandleCPU_);

}

void LargeNumberOfObjects::Update()
{

	std::list<OneOfManyObjects*>::iterator itr = objects_.begin();
	for (; itr != objects_.end(); ++itr) {
		OneOfManyObjects* obj = *itr;
		obj->Update();
	}

}

void LargeNumberOfObjects::Map()
{

	// ノードデータ
	for (uint32_t i = 0; i < nodeDatas_.size(); ++i) {

		if (nodeDatas_[i].parentIndex >= 0) {
			nodeDatas_[i].matrix = Matrix4x4::Multiply(
				nodeDatas_[i].localMatrix,
				nodeDatas_[nodeDatas_[i].parentIndex].matrix);
		}
		else {
			nodeDatas_[i].matrix = nodeDatas_[i].localMatrix;
		}

		localMatrixesMap_[i].matrix = nodeDatas_[i].offsetMatrix * nodeDatas_[i].matrix;

	}

	std::list<OneOfManyObjects*>::iterator itr = objects_.begin();
	uint32_t i = 0;
	for (; itr != objects_.end(); ++itr) {
		OneOfManyObjects* obj = *itr;
		transformationMatrixesMap_[i].World = obj->worldMatrix_;
		transformationMatrixesMap_[i].WorldInverseTranspose = Matrix4x4::Transpose(obj->worldMatrix_);
		i++;
	}
	numInstance_ = i;

}

void LargeNumberOfObjects::Draw(BaseCamera& camera)
{

}

void LargeNumberOfObjects::SetNodeDatas(const ModelNode& modelNode, int32_t parentIndex)
{

	NodeData nodeData;

	nodeData.localMatrix = modelNode.localMatrix;
	nodeData.meshNum = modelNode.meshNum;
	nodeData.name = modelNode.name;
	nodeData.parentIndex = parentIndex;
	nodeData.offsetMatrix = modelNode.offsetMatrix;
	nodeDatas_.push_back(std::move(nodeData));

	int32_t newParentIndex = static_cast<int32_t>(nodeDatas_.size()) - 1;

	for (uint32_t childIndex = 0; childIndex < modelNode.children.size(); ++childIndex) {
		SetNodeDatas(modelNode.children[childIndex], newParentIndex);
	}

}

void LargeNumberOfObjects::DeadDelete()
{

	objects_.remove_if([](OneOfManyObjects* object) {
		if (object->IsDead()) {
			delete object;
			return true;
		}
		return false;
		});

}

void LargeNumberOfObjects::AddObject(OneOfManyObjects* object)
{

	objects_.push_back(object);

}
