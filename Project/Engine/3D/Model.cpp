#include "Model.h"
#include "../base/TextureManager.h"
#include "../base/WinApp.h"
#include <cassert>

#include <fstream>
#include <sstream>
#include <format>
#include "../base/BufferResource.h"
#include "ModelLoader.h"
#include "FogManager.h"
using namespace DirectX;
using namespace Microsoft::WRL;

// デバイス
ID3D12Device* Model::sDevice = nullptr;
// ディスクリプタサイズ
UINT Model::sDescriptorHandleIncrementSize;
// コマンドリスト
ID3D12GraphicsCommandList* Model::sCommandList = nullptr;
// ルートシグネチャ
ID3D12RootSignature* Model::sRootSignature[PipelineStateName::kPipelineStateNameOfCount];
// パイプラインステートオブジェクト
ID3D12PipelineState* Model::sPipelineState[PipelineStateName::kPipelineStateNameOfCount];
// ポイントライトマネージャ
PointLightManager* Model::pointLightManager_ = nullptr;
//	スポットライトマネージャ
SpotLightManager* Model::spotLightManager_ = nullptr;
//	平行光源
DirectionalLight* Model::directionalLight_ = nullptr;

/// <summary>
/// 静的初期化
/// </summary>
/// <param name="device">デバイス</param>
void Model::StaticInitialize(ID3D12Device* device,
	const std::array<ID3D12RootSignature*, PipelineStateName::kPipelineStateNameOfCount>& rootSignature,
	const std::array<ID3D12PipelineState*, PipelineStateName::kPipelineStateNameOfCount>& pipelineState) {

	assert(device);

	sDevice = device;

	// グラフィックパイプライン生成
	for (uint32_t i = 0u; i < PipelineStateName::kPipelineStateNameOfCount; i++) {
		sRootSignature[i] = rootSignature[i];
		sPipelineState[i] = pipelineState[i];
	}

}

void Model::PreDraw(
	ID3D12GraphicsCommandList* cmdList,
	PointLightManager* pointLightManager, 
	SpotLightManager* spotLightManager,
	DirectionalLight* directionalLight) {

	assert(sCommandList == nullptr);

	sCommandList = cmdList;

	//RootSignatureを設定。
	sCommandList->SetPipelineState(sPipelineState[PipelineStateName::kPipelineStateNameModel]);//PS0を設定
	sCommandList->SetGraphicsRootSignature(sRootSignature[PipelineStateName::kPipelineStateNameModel]);

	// SRV
	ID3D12DescriptorHeap* ppHeaps[] = { SRVDescriptorHerpManager::descriptorHeap_.Get() };
	sCommandList->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);

	//形状を設定。PS0に設定しているものとは別。
	sCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	pointLightManager_ = pointLightManager;
	spotLightManager_ = spotLightManager;
	directionalLight_ = directionalLight;

}

void Model::PreParticleDraw(ID3D12GraphicsCommandList* cmdList, const Matrix4x4& viewProjectionMatrix)
{

	assert(sCommandList == nullptr);

	sCommandList = cmdList;

	//RootSignatureを設定。
	sCommandList->SetPipelineState(sPipelineState[PipelineStateName::kPipelineStateNameParticle]);//PS0を設定
	sCommandList->SetGraphicsRootSignature(sRootSignature[PipelineStateName::kPipelineStateNameParticle]);

	// SRV
	ID3D12DescriptorHeap* ppHeaps[] = { SRVDescriptorHerpManager::descriptorHeap_.Get() };
	sCommandList->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);

	//形状を設定。PS0に設定しているものとは別。
	sCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	ParticleManager* particleManager = ParticleManager::GetInstance();
	particleManager->Map(viewProjectionMatrix);

}

/// <summary>
/// 静的前処理
/// </summary>
/// <param name="cmdList">描画コマンドリスト</param>
void Model::PreDrawOutLine(ID3D12GraphicsCommandList* cmdList) {

	assert(sCommandList == nullptr);

	sCommandList = cmdList;

	assert(sPipelineState[PipelineStateName::kPipelineStateNameOutLine]);

	//RootSignatureを設定。
	sCommandList->SetPipelineState(sPipelineState[PipelineStateName::kPipelineStateNameOutLine]);//PS0を設定
	sCommandList->SetGraphicsRootSignature(sRootSignature[PipelineStateName::kPipelineStateNameOutLine]);

	// SRV
	ID3D12DescriptorHeap* ppHeaps[] = { SRVDescriptorHerpManager::descriptorHeap_.Get() };
	sCommandList->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);

	//形状を設定。PS0に設定しているものとは別。
	sCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

}

void Model::PreManyModelsDraw(
	ID3D12GraphicsCommandList* cmdList, 
	PointLightManager* pointLightManager, 
	SpotLightManager* spotLightManager,
	DirectionalLight* directionalLight)
{

	assert(sCommandList == nullptr);

	sCommandList = cmdList;

	//RootSignatureを設定。
	sCommandList->SetPipelineState(sPipelineState[PipelineStateName::kPipelineStateNameManyModels]);//PS0を設定
	sCommandList->SetGraphicsRootSignature(sRootSignature[PipelineStateName::kPipelineStateNameManyModels]);

	// SRV
	ID3D12DescriptorHeap* ppHeaps[] = { SRVDescriptorHerpManager::descriptorHeap_.Get() };
	sCommandList->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);

	//形状を設定。PS0に設定しているものとは別。
	sCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	pointLightManager_ = pointLightManager;
	spotLightManager_ = spotLightManager;
	directionalLight_ = directionalLight;

}

/// <summary>
/// 描画後処理
/// </summary>
void Model::PostDraw() {
	// コマンドリストを解除
	sCommandList = nullptr;

	pointLightManager_ = nullptr;
	spotLightManager_ = nullptr;
	directionalLight_ = nullptr;

}

/// <summary>
/// 3Dモデル生成
/// </summary>
/// <returns></returns>
Model* Model::Create(const std::string& directoryPath, const std::string& filename, DirectXCommon* dxCommon, ITextureHandleManager* textureHandleManager) {

	// 3Dオブジェクトのインスタンスを生成
	Model* object3d = new Model();
	assert(object3d);

	// 初期化
	object3d->Initialize(directoryPath, filename, dxCommon, textureHandleManager);

	return object3d;

}

/// <summary>
/// 初期化
/// </summary>
void Model::Initialize(const std::string& directoryPath, const std::string& filename, DirectXCommon* dxCommon, ITextureHandleManager* textureHandleManager) {

	assert(sDevice);

	// モデル読み込み
	modelData_ = ModelLoader::LoadModelFile(directoryPath, filename);

	//メッシュ生成
	mesh_ = std::make_unique<Mesh>();
	mesh_->CreateMesh(sDevice,modelData_.vertices,modelData_.vertexInfluences);

	for (size_t i = 0; i < modelData_.material.textureFilePaths.size(); ++i) {
		// テクスチャハンドル
		textureHandles_.push_back(TextureManager::Load(modelData_.material.textureFilePaths[i], dxCommon, textureHandleManager));
		// リソース
		resourceDescs_.push_back(TextureManager::GetInstance()->GetResourceDesc(textureHandles_[i]));
	}

	// マテリアル
	defaultMaterial_.reset(Material::Create());

}

/// <summary>
/// 更新
/// </summary>
void Model::Update() {

}

/// <summary>
/// 描画
/// </summary>
void Model::Draw(WorldTransform& worldTransform, BaseCamera& camera) {

	// nullptrチェック
	assert(sDevice);
	assert(sCommandList);

	worldTransform.Map();

	D3D12_VERTEX_BUFFER_VIEW vbvs[2] = {
		*mesh_->GetVbView(),
		*mesh_->GetInfluenceView()
	};

	sCommandList->IASetVertexBuffers(0, 2, vbvs); //VBVを設定

	//マテリアルCBufferの場所を設定
	sCommandList->SetGraphicsRootConstantBufferView(0, defaultMaterial_->GetMaterialBuff()->GetGPUVirtualAddress());

	// 平行光源
	if (directionalLight_) {
		directionalLight_->Draw(sCommandList, 1);
	}

	// カメラCBufferの場所を設定
	sCommandList->SetGraphicsRootConstantBufferView(2, camera.GetWorldPositionBuff()->GetGPUVirtualAddress());
	
	// ワールドトランスフォーム
	sCommandList->SetGraphicsRootConstantBufferView(3, worldTransform.GetTransformationMatrixBuff()->GetGPUVirtualAddress());
	// ビュープロジェクション
	sCommandList->SetGraphicsRootConstantBufferView(4, camera.GetViewProjectionMatriBuff()->GetGPUVirtualAddress());

	// ローカル行列
	//worldTransform.SetGraphicsRootDescriptorTable(sCommandList, 5);

	//テクスチャ
	for (size_t i = 0; i < modelData_.material.textureFilePaths.size(); ++i) {
		TextureManager::GetInstance()->SetGraphicsRootDescriptorTable(sCommandList, 6 + static_cast<UINT>(i), textureHandles_[i]);
	}

	// ポイントライト
	if (pointLightManager_) {
		pointLightManager_->Draw(sCommandList, 10);
	}
	// スポットライト
	if (spotLightManager_) {
		spotLightManager_->Draw(sCommandList, 11);
	}

	// 霧
	sCommandList->SetGraphicsRootConstantBufferView(12, FogManager::GetInstance()->GetFogDataBuff()->GetGPUVirtualAddress());

	//描画
	sCommandList->DrawInstanced(UINT(modelData_.vertices.size()), 1, 0, 0);

}

/// <summary>
/// 描画
/// </summary>
void Model::Draw(WorldTransform& worldTransform, BaseCamera& camera, Material* material, LocalMatrixManager* localMatrixManager) {

	// nullptrチェック
	assert(sDevice);
	assert(sCommandList);

	worldTransform.Map();

	D3D12_VERTEX_BUFFER_VIEW vbvs[2] = {
		*mesh_->GetVbView(),
		*mesh_->GetInfluenceView()
	};

	sCommandList->IASetVertexBuffers(0, 2, vbvs); //VBVを設定

	//マテリアルCBufferの場所を設定
	sCommandList->SetGraphicsRootConstantBufferView(0, material->GetMaterialBuff()->GetGPUVirtualAddress());

	// 平行光源
	if (directionalLight_) {
		directionalLight_->Draw(sCommandList, 1);
	}

	// カメラCBufferの場所を設定
	sCommandList->SetGraphicsRootConstantBufferView(2, camera.GetWorldPositionBuff()->GetGPUVirtualAddress());

	// ワールドトランスフォーム
	sCommandList->SetGraphicsRootConstantBufferView(3, worldTransform.GetTransformationMatrixBuff()->GetGPUVirtualAddress());
	// ビュープロジェクション
	sCommandList->SetGraphicsRootConstantBufferView(4, camera.GetViewProjectionMatriBuff()->GetGPUVirtualAddress());

	// ローカル行列
	localMatrixManager->SetGraphicsRootDescriptorTable(sCommandList, 5);

	//テクスチャ
	for (size_t i = 0; i < modelData_.material.textureFilePaths.size(); ++i) {
		TextureManager::GetInstance()->SetGraphicsRootDescriptorTable(sCommandList, 6 + static_cast<UINT>(i), textureHandles_[i]);
	}

	// ポイントライト
	if (pointLightManager_) {
		pointLightManager_->Draw(sCommandList, 10);
	}
	// スポットライト
	if (spotLightManager_) {
		spotLightManager_->Draw(sCommandList, 11);
	}

	// 霧
	sCommandList->SetGraphicsRootConstantBufferView(12, FogManager::GetInstance()->GetFogDataBuff()->GetGPUVirtualAddress());

	//描画
	sCommandList->DrawInstanced(UINT(modelData_.vertices.size()), 1, 0, 0);

}

/// <summary>
/// 描画
/// </summary>
void Model::Draw(
	WorldTransform& worldTransform,
	BaseCamera& camera, 
	Material* material,
	uint32_t textureHandle) {

	// nullptrチェック
	assert(sDevice);
	assert(sCommandList);

	worldTransform.Map();

	D3D12_VERTEX_BUFFER_VIEW vbvs[2] = {
		*mesh_->GetVbView(),
		*mesh_->GetInfluenceView()
	};

	sCommandList->IASetVertexBuffers(0, 2, vbvs); //VBVを設定

	//マテリアルCBufferの場所を設定
	sCommandList->SetGraphicsRootConstantBufferView(0, material->GetMaterialBuff()->GetGPUVirtualAddress());

	// 平行光源
	if (directionalLight_) {
		directionalLight_->Draw(sCommandList, 1);
	}

	// カメラCBufferの場所を設定
	sCommandList->SetGraphicsRootConstantBufferView(2, camera.GetWorldPositionBuff()->GetGPUVirtualAddress());

	// ワールドトランスフォーム
	sCommandList->SetGraphicsRootConstantBufferView(3, worldTransform.GetTransformationMatrixBuff()->GetGPUVirtualAddress());
	// ビュープロジェクション
	sCommandList->SetGraphicsRootConstantBufferView(4, camera.GetViewProjectionMatriBuff()->GetGPUVirtualAddress());

	// ローカル行列
	//worldTransform.SetGraphicsRootDescriptorTable(sCommandList, 5);

	//テクスチャ
	TextureManager::GetInstance()->SetGraphicsRootDescriptorTable(sCommandList, 6, textureHandle);

	// ポイントライト
	if (pointLightManager_) {
		pointLightManager_->Draw(sCommandList, 10);
	}
	// スポットライト
	if (spotLightManager_) {
		spotLightManager_->Draw(sCommandList, 11);
	}

	// 霧
	sCommandList->SetGraphicsRootConstantBufferView(12, FogManager::GetInstance()->GetFogDataBuff()->GetGPUVirtualAddress());

	//描画
	sCommandList->DrawInstanced(UINT(modelData_.vertices.size()), 1, 0, 0);

}


void Model::Draw(
	const D3D12_GPU_DESCRIPTOR_HANDLE& localMatrixesHandle, 
	const D3D12_GPU_DESCRIPTOR_HANDLE& transformationMatrixesHandle, 
	BaseCamera& camera,
	uint32_t numInstance)
{

	// nullptrチェック
	assert(sDevice);
	assert(sCommandList);

	//RootSignatureを設定。
	sCommandList->SetPipelineState(sPipelineState[PipelineStateName::kPipelineStateNameManyModels]);//PS0を設定
	sCommandList->SetGraphicsRootSignature(sRootSignature[PipelineStateName::kPipelineStateNameManyModels]);

	//形状を設定。PS0に設定しているものとは別。
	sCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	sCommandList->IASetVertexBuffers(0, 1, mesh_->GetVbView()); //VBVを設定

	//マテリアルCBufferの場所を設定
	sCommandList->SetGraphicsRootConstantBufferView(0, defaultMaterial_->GetMaterialBuff()->GetGPUVirtualAddress());

	// カメラCBufferの場所を設定
	sCommandList->SetGraphicsRootConstantBufferView(7, camera.GetWorldPositionBuff()->GetGPUVirtualAddress());

	// ビュープロジェクション
	sCommandList->SetGraphicsRootConstantBufferView(11, camera.GetViewProjectionMatriBuff()->GetGPUVirtualAddress());

	//SRVのDescriptorTableの先頭を設定。2はrootParamenter[2]である
	for (size_t i = 0; i < modelData_.material.textureFilePaths.size(); ++i) {
		TextureManager::GetInstance()->SetGraphicsRootDescriptorTable(sCommandList, 2 + static_cast<UINT>(i), textureHandles_[i]);
	}

	// ポイントライト
	if (pointLightManager_) {
		pointLightManager_->Draw(sCommandList, 8);
	}
	// スポットライト
	if (spotLightManager_) {
		spotLightManager_->Draw(sCommandList, 9);
	}
	// 平行光源
	if (directionalLight_) {
		directionalLight_->Draw(sCommandList, 6);
	}

	// ワールドトランスフォーム
	sCommandList->SetGraphicsRootDescriptorTable(10, transformationMatrixesHandle);
	// ローカル
	sCommandList->SetGraphicsRootDescriptorTable(1, localMatrixesHandle);

	// 霧
	sCommandList->SetGraphicsRootConstantBufferView(12, FogManager::GetInstance()->GetFogDataBuff()->GetGPUVirtualAddress());

	//描画
	sCommandList->DrawInstanced(UINT(modelData_.vertices.size()), numInstance, 0, 0);

}

void Model::Draw(
	const D3D12_GPU_DESCRIPTOR_HANDLE& localMatrixesHandle, 
	const D3D12_GPU_DESCRIPTOR_HANDLE& transformationMatrixesHandle, 
	BaseCamera& camera,
	uint32_t numInstance,
	Material* material)
{

	// nullptrチェック
	assert(sDevice);
	assert(sCommandList);

	//RootSignatureを設定。
	sCommandList->SetPipelineState(sPipelineState[PipelineStateName::kPipelineStateNameManyModels]);//PS0を設定
	sCommandList->SetGraphicsRootSignature(sRootSignature[PipelineStateName::kPipelineStateNameManyModels]);

	//形状を設定。PS0に設定しているものとは別。
	sCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	sCommandList->IASetVertexBuffers(0, 1, mesh_->GetVbView()); //VBVを設定

	//マテリアルCBufferの場所を設定
	sCommandList->SetGraphicsRootConstantBufferView(0, material->GetMaterialBuff()->GetGPUVirtualAddress());

	// カメラCBufferの場所を設定
	sCommandList->SetGraphicsRootConstantBufferView(7, camera.GetWorldPositionBuff()->GetGPUVirtualAddress());

	// ビュープロジェクション
	sCommandList->SetGraphicsRootConstantBufferView(11, camera.GetViewProjectionMatriBuff()->GetGPUVirtualAddress());

	//SRVのDescriptorTableの先頭を設定。2はrootParamenter[2]である
	for (size_t i = 0; i < modelData_.material.textureFilePaths.size(); ++i) {
		TextureManager::GetInstance()->SetGraphicsRootDescriptorTable(sCommandList, 2 + static_cast<UINT>(i), textureHandles_[i]);
	}

	// ポイントライト
	if (pointLightManager_) {
		pointLightManager_->Draw(sCommandList, 8);
	}
	// スポットライト
	if (spotLightManager_) {
		spotLightManager_->Draw(sCommandList, 9);
	}
	// 平行光源
	if (directionalLight_) {
		directionalLight_->Draw(sCommandList, 6);
	}

	// ワールドトランスフォーム
	sCommandList->SetGraphicsRootDescriptorTable(10, transformationMatrixesHandle);
	// ローカル
	sCommandList->SetGraphicsRootDescriptorTable(1, localMatrixesHandle);

	// 霧
	sCommandList->SetGraphicsRootConstantBufferView(12, FogManager::GetInstance()->GetFogDataBuff()->GetGPUVirtualAddress());

	//描画
	sCommandList->DrawInstanced(UINT(modelData_.vertices.size()), numInstance, 0, 0);

}

void Model::Draw(
	const D3D12_GPU_DESCRIPTOR_HANDLE& localMatrixesHandle, 
	const D3D12_GPU_DESCRIPTOR_HANDLE& transformationMatrixesHandle, 
	BaseCamera& camera,
	uint32_t numInstance,
	Material* material,
	uint32_t textureHandle)
{

	// nullptrチェック
	assert(sDevice);
	assert(sCommandList);

	//RootSignatureを設定。
	sCommandList->SetPipelineState(sPipelineState[PipelineStateName::kPipelineStateNameManyModels]);//PS0を設定
	sCommandList->SetGraphicsRootSignature(sRootSignature[PipelineStateName::kPipelineStateNameManyModels]);

	//形状を設定。PS0に設定しているものとは別。
	sCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	sCommandList->IASetVertexBuffers(0, 1, mesh_->GetVbView()); //VBVを設定

	//マテリアルCBufferの場所を設定
	sCommandList->SetGraphicsRootConstantBufferView(0, material->GetMaterialBuff()->GetGPUVirtualAddress());

	// カメラCBufferの場所を設定
	sCommandList->SetGraphicsRootConstantBufferView(7, camera.GetWorldPositionBuff()->GetGPUVirtualAddress());

	// ビュープロジェクション
	sCommandList->SetGraphicsRootConstantBufferView(11, camera.GetViewProjectionMatriBuff()->GetGPUVirtualAddress());

	//SRVのDescriptorTableの先頭を設定。2はrootParamenter[2]である
	TextureManager::GetInstance()->SetGraphicsRootDescriptorTable(sCommandList, 2, textureHandle);

	// ポイントライト
	if (pointLightManager_) {
		pointLightManager_->Draw(sCommandList, 8);
	}
	// スポットライト
	if (spotLightManager_) {
		spotLightManager_->Draw(sCommandList, 9);
	}
	// 平行光源
	if (directionalLight_) {
		directionalLight_->Draw(sCommandList, 6);
	}

	// ワールドトランスフォーム
	sCommandList->SetGraphicsRootDescriptorTable(10, transformationMatrixesHandle);
	// ローカル
	sCommandList->SetGraphicsRootDescriptorTable(1, localMatrixesHandle);

	// 霧
	sCommandList->SetGraphicsRootConstantBufferView(12, FogManager::GetInstance()->GetFogDataBuff()->GetGPUVirtualAddress());

	//描画
	sCommandList->DrawInstanced(UINT(modelData_.vertices.size()), numInstance, 0, 0);

}

void Model::ParticleDraw()
{

	// nullptrチェック
	assert(sDevice);
	assert(sCommandList);

	ParticleManager* particleManager = ParticleManager::GetInstance();

	sCommandList->IASetVertexBuffers(0, 1, mesh_->GetVbView()); //VBVを設定

	//マテリアルCBufferの場所を設定
	sCommandList->SetGraphicsRootConstantBufferView(0, defaultMaterial_->GetMaterialBuff()->GetGPUVirtualAddress());

	//マテリアルCBufferの場所を設定
	sCommandList->SetGraphicsRootConstantBufferView(4, particleManager->GetCurrentStartInstanceIdBuff()->GetGPUVirtualAddress());

	//SRVのDescriptorTableの先頭を設定。2はrootParamenter[2]である
	for (size_t i = 0; i < modelData_.material.textureFilePaths.size(); ++i) {
		TextureManager::GetInstance()->SetGraphicsRootDescriptorTable(sCommandList, 2, textureHandles_[i]);
	}

	sCommandList->SetGraphicsRootDescriptorTable(1, particleManager->GetInstancingSrvHandleGPU());

	//描画
	sCommandList->DrawInstanced(UINT(modelData_.vertices.size()), particleManager->GetCurrentInstanceIndex(), 0, 0);

}

void Model::OutLineDraw(WorldTransform& worldTransform, BaseCamera& camera, OutLineData& outLineData) {
	// nullptrチェック
	assert(sDevice);
	assert(sCommandList);
	assert(0);

	worldTransform.Map();
	sCommandList->IASetVertexBuffers(0, 1, mesh_->GetVbView()); //VBVを設定

	//wvp用のCBufferの場所を設定
	//sCommandList->SetGraphicsRootConstantBufferView(1, worldTransform.transformationMatrixBuff_->GetGPUVirtualAddress());

	//マテリアルCBufferの場所を設定
	sCommandList->SetGraphicsRootConstantBufferView(0, outLineData.forPSResource_->GetGPUVirtualAddress());

	//マテリアルCBufferの場所を設定
	sCommandList->SetGraphicsRootConstantBufferView(2, outLineData.forVSResource_->GetGPUVirtualAddress());

	
	//描画
	sCommandList->DrawInstanced(UINT(modelData_.vertices.size()), 1, 0, 0);

}




/// <summary>
/// テクスチャハンドルの設定
/// </summary>
/// <param name="textureHandle"></param>
void Model::SetTextureHandle(uint32_t textureHandle, uint32_t index) {

	textureHandles_[index] = textureHandle;
	resourceDescs_[index] = TextureManager::GetInstance()->GetResourceDesc(textureHandles_[index]);

}


