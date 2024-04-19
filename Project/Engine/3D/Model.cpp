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
// デフォルトマテリアル
std::unique_ptr<Material> Model::sDefaultMaterial = nullptr;

/// <summary>
/// 静的初期化
/// </summary>
/// <param name="device">デバイス</param>
void Model::StaticInitialize(ID3D12Device* device) {

	assert(device);

	// デバイス
	sDevice = device;

	// デフォルトマテリアル
	sDefaultMaterial.reset(Material::Create());

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

}

//void Model::Draw(
//	const D3D12_GPU_DESCRIPTOR_HANDLE& localMatrixesHandle, 
//	const D3D12_GPU_DESCRIPTOR_HANDLE& transformationMatrixesHandle, 
//	BaseCamera& camera,
//	uint32_t numInstance)
//{
//
//	// nullptrチェック
//	assert(sDevice);
//	assert(sCommandList);
//
//	//RootSignatureを設定。
//	sCommandList->SetPipelineState(sPipelineState[PipelineStateName::kPipelineStateNameManyModels]);//PS0を設定
//	sCommandList->SetGraphicsRootSignature(sRootSignature[PipelineStateName::kPipelineStateNameManyModels]);
//
//	//形状を設定。PS0に設定しているものとは別。
//	sCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
//
//	sCommandList->IASetVertexBuffers(0, 1, mesh_->GetVbView()); //VBVを設定
//
//	//マテリアルCBufferの場所を設定
//	sCommandList->SetGraphicsRootConstantBufferView(0, defaultMaterial_->GetMaterialBuff()->GetGPUVirtualAddress());
//
//	// カメラCBufferの場所を設定
//	sCommandList->SetGraphicsRootConstantBufferView(7, camera.GetWorldPositionBuff()->GetGPUVirtualAddress());
//
//	// ビュープロジェクション
//	sCommandList->SetGraphicsRootConstantBufferView(11, camera.GetViewProjectionMatriBuff()->GetGPUVirtualAddress());
//
//	//SRVのDescriptorTableの先頭を設定。2はrootParamenter[2]である
//	for (size_t i = 0; i < modelData_.material.textureFilePaths.size(); ++i) {
//		TextureManager::GetInstance()->SetGraphicsRootDescriptorTable(sCommandList, 2 + static_cast<UINT>(i), textureHandles_[i]);
//	}
//
//	// ポイントライト
//	if (pointLightManager_) {
//		pointLightManager_->Draw(sCommandList, 8);
//	}
//	// スポットライト
//	if (spotLightManager_) {
//		spotLightManager_->Draw(sCommandList, 9);
//	}
//	// 平行光源
//	if (directionalLight_) {
//		directionalLight_->Draw(sCommandList, 6);
//	}
//
//	// ワールドトランスフォーム
//	sCommandList->SetGraphicsRootDescriptorTable(10, transformationMatrixesHandle);
//	// ローカル
//	sCommandList->SetGraphicsRootDescriptorTable(1, localMatrixesHandle);
//
//	// 霧
//	sCommandList->SetGraphicsRootConstantBufferView(12, FogManager::GetInstance()->GetFogDataBuff()->GetGPUVirtualAddress());
//
//	//描画
//	sCommandList->DrawInstanced(UINT(modelData_.vertices.size()), numInstance, 0, 0);
//
//}

//void Model::Draw(
//	const D3D12_GPU_DESCRIPTOR_HANDLE& localMatrixesHandle, 
//	const D3D12_GPU_DESCRIPTOR_HANDLE& transformationMatrixesHandle, 
//	BaseCamera& camera,
//	uint32_t numInstance,
//	Material* material)
//{
//
//	// nullptrチェック
//	assert(sDevice);
//	assert(sCommandList);
//
//	//RootSignatureを設定。
//	sCommandList->SetPipelineState(sPipelineState[PipelineStateName::kPipelineStateNameManyModels]);//PS0を設定
//	sCommandList->SetGraphicsRootSignature(sRootSignature[PipelineStateName::kPipelineStateNameManyModels]);
//
//	//形状を設定。PS0に設定しているものとは別。
//	sCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
//
//	sCommandList->IASetVertexBuffers(0, 1, mesh_->GetVbView()); //VBVを設定
//
//	//マテリアルCBufferの場所を設定
//	sCommandList->SetGraphicsRootConstantBufferView(0, material->GetMaterialBuff()->GetGPUVirtualAddress());
//
//	// カメラCBufferの場所を設定
//	sCommandList->SetGraphicsRootConstantBufferView(7, camera.GetWorldPositionBuff()->GetGPUVirtualAddress());
//
//	// ビュープロジェクション
//	sCommandList->SetGraphicsRootConstantBufferView(11, camera.GetViewProjectionMatriBuff()->GetGPUVirtualAddress());
//
//	//SRVのDescriptorTableの先頭を設定。2はrootParamenter[2]である
//	for (size_t i = 0; i < modelData_.material.textureFilePaths.size(); ++i) {
//		TextureManager::GetInstance()->SetGraphicsRootDescriptorTable(sCommandList, 2 + static_cast<UINT>(i), textureHandles_[i]);
//	}
//
//	// ポイントライト
//	if (pointLightManager_) {
//		pointLightManager_->Draw(sCommandList, 8);
//	}
//	// スポットライト
//	if (spotLightManager_) {
//		spotLightManager_->Draw(sCommandList, 9);
//	}
//	// 平行光源
//	if (directionalLight_) {
//		directionalLight_->Draw(sCommandList, 6);
//	}
//
//	// ワールドトランスフォーム
//	sCommandList->SetGraphicsRootDescriptorTable(10, transformationMatrixesHandle);
//	// ローカル
//	sCommandList->SetGraphicsRootDescriptorTable(1, localMatrixesHandle);
//
//	// 霧
//	sCommandList->SetGraphicsRootConstantBufferView(12, FogManager::GetInstance()->GetFogDataBuff()->GetGPUVirtualAddress());
//
//	//描画
//	sCommandList->DrawInstanced(UINT(modelData_.vertices.size()), numInstance, 0, 0);
//
//}
//
//void Model::Draw(
//	const D3D12_GPU_DESCRIPTOR_HANDLE& localMatrixesHandle, 
//	const D3D12_GPU_DESCRIPTOR_HANDLE& transformationMatrixesHandle, 
//	BaseCamera& camera,
//	uint32_t numInstance,
//	Material* material,
//	uint32_t textureHandle)
//{
//
//	// nullptrチェック
//	assert(sDevice);
//	assert(sCommandList);
//
//	//RootSignatureを設定。
//	sCommandList->SetPipelineState(sPipelineState[PipelineStateName::kPipelineStateNameManyModels]);//PS0を設定
//	sCommandList->SetGraphicsRootSignature(sRootSignature[PipelineStateName::kPipelineStateNameManyModels]);
//
//	//形状を設定。PS0に設定しているものとは別。
//	sCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
//
//	sCommandList->IASetVertexBuffers(0, 1, mesh_->GetVbView()); //VBVを設定
//
//	//マテリアルCBufferの場所を設定
//	sCommandList->SetGraphicsRootConstantBufferView(0, material->GetMaterialBuff()->GetGPUVirtualAddress());
//
//	// カメラCBufferの場所を設定
//	sCommandList->SetGraphicsRootConstantBufferView(7, camera.GetWorldPositionBuff()->GetGPUVirtualAddress());
//
//	// ビュープロジェクション
//	sCommandList->SetGraphicsRootConstantBufferView(11, camera.GetViewProjectionMatriBuff()->GetGPUVirtualAddress());
//
//	//SRVのDescriptorTableの先頭を設定。2はrootParamenter[2]である
//	TextureManager::GetInstance()->SetGraphicsRootDescriptorTable(sCommandList, 2, textureHandle);
//
//	// ポイントライト
//	if (pointLightManager_) {
//		pointLightManager_->Draw(sCommandList, 8);
//	}
//	// スポットライト
//	if (spotLightManager_) {
//		spotLightManager_->Draw(sCommandList, 9);
//	}
//	// 平行光源
//	if (directionalLight_) {
//		directionalLight_->Draw(sCommandList, 6);
//	}
//
//	// ワールドトランスフォーム
//	sCommandList->SetGraphicsRootDescriptorTable(10, transformationMatrixesHandle);
//	// ローカル
//	sCommandList->SetGraphicsRootDescriptorTable(1, localMatrixesHandle);
//
//	// 霧
//	sCommandList->SetGraphicsRootConstantBufferView(12, FogManager::GetInstance()->GetFogDataBuff()->GetGPUVirtualAddress());
//
//	//描画
//	sCommandList->DrawInstanced(UINT(modelData_.vertices.size()), numInstance, 0, 0);
//
//}
//

/// <summary>
/// テクスチャハンドルの設定
/// </summary>
/// <param name="textureHandle"></param>
void Model::SetTextureHandle(uint32_t textureHandle, uint32_t index) {

	textureHandles_[index] = textureHandle;
	resourceDescs_[index] = TextureManager::GetInstance()->GetResourceDesc(textureHandles_[index]);

}


