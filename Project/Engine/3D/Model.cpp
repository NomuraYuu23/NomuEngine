#include "Model.h"
#include "../base/TextureManager.h"
#include "../base/WinApp.h"
#include <cassert>

#include <fstream>
#include <sstream>
#include <format>
#include "../base/BufferResource.h"
#include "ModelLoader.h"
using namespace DirectX;
using namespace Microsoft::WRL;

// デバイス
ID3D12Device* Model::sDevice = nullptr;
// ディスクリプタサイズ
UINT Model::sDescriptorHandleIncrementSize;
// コマンドリスト
ID3D12GraphicsCommandList* Model::sCommandList = nullptr;
// ルートシグネチャ
ID3D12RootSignature* Model::sRootSignature[GraphicsPipelineState::PipelineStateName::kPipelineStateNameOfCount];
// パイプラインステートオブジェクト
ID3D12PipelineState* Model::sPipelineState[GraphicsPipelineState::PipelineStateName::kPipelineStateNameOfCount];
//計算
Matrix4x4Calc* Model::matrix4x4Calc = nullptr;
// ポイントライトマネージャ
PointLightManager* Model::pointLightManager_ = nullptr;
//	スポットライトマネージャ
SpotLightManager* Model::spotLightManager_ = nullptr;

/// <summary>
/// 静的初期化
/// </summary>
/// <param name="device">デバイス</param>
void Model::StaticInitialize(ID3D12Device* device,
	const std::array<ID3D12RootSignature*, GraphicsPipelineState::PipelineStateName::kPipelineStateNameOfCount>& rootSignature,
	const std::array<ID3D12PipelineState*, GraphicsPipelineState::PipelineStateName::kPipelineStateNameOfCount>& pipelineState) {

	assert(device);

	sDevice = device;

	matrix4x4Calc = Matrix4x4Calc::GetInstance();

	// グラフィックパイプライン生成
	for (uint32_t i = 0u; i < GraphicsPipelineState::PipelineStateName::kPipelineStateNameOfCount; i++) {
		sRootSignature[i] = rootSignature[i];
		sPipelineState[i] = pipelineState[i];
	}

}

/// <summary>
/// 静的前処理
/// </summary>
/// <param name="cmdList">描画コマンドリスト</param>
void Model::PreDraw(ID3D12GraphicsCommandList* cmdList, PointLightManager* pointLightManager, SpotLightManager* spotLightManager) {

	assert(sCommandList == nullptr);

	sCommandList = cmdList;

	//RootSignatureを設定。
	sCommandList->SetPipelineState(sPipelineState[GraphicsPipelineState::PipelineStateName::kPipelineStateNameModel]);//PS0を設定
	sCommandList->SetGraphicsRootSignature(sRootSignature[GraphicsPipelineState::PipelineStateName::kPipelineStateNameModel]);

	//形状を設定。PS0に設定しているものとは別。
	sCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	pointLightManager_ = pointLightManager;
	spotLightManager_ = spotLightManager;

}

void Model::PreParticleDraw(ID3D12GraphicsCommandList* cmdList, const Matrix4x4& viewProjectionMatrix)
{

	assert(sCommandList == nullptr);

	sCommandList = cmdList;

	//RootSignatureを設定。
	sCommandList->SetPipelineState(sPipelineState[GraphicsPipelineState::PipelineStateName::kPipelineStateNameParticle]);//PS0を設定
	sCommandList->SetGraphicsRootSignature(sRootSignature[GraphicsPipelineState::PipelineStateName::kPipelineStateNameParticle]);

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

	assert(sPipelineState[GraphicsPipelineState::PipelineStateName::kPipelineStateNameOutLine]);

	//RootSignatureを設定。
	sCommandList->SetPipelineState(sPipelineState[GraphicsPipelineState::PipelineStateName::kPipelineStateNameOutLine]);//PS0を設定
	sCommandList->SetGraphicsRootSignature(sRootSignature[GraphicsPipelineState::PipelineStateName::kPipelineStateNameOutLine]);

	//形状を設定。PS0に設定しているものとは別。
	sCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

}

/// <summary>
/// 描画後処理
/// </summary>
void Model::PostDraw() {
	// コマンドリストを解除
	sCommandList = nullptr;

	pointLightManager_ = nullptr;
	spotLightManager_ = nullptr;

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
	modelData_ = ModelLoader::LoadObjFile(directoryPath, filename);

	//メッシュ生成
	mesh_ = std::make_unique<Mesh>();
	mesh_->CreateMesh(sDevice,modelData_.vertices);

	// テクスチャハンドル
	textureHandle_ = TextureManager::Load(modelData_.material.textureFilePath, dxCommon, textureHandleManager);

	// リソース
	resourceDesc_ = TextureManager::GetInstance()->GetResourceDesc(textureHandle_);

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

	worldTransform.Map(camera.GetViewProjectionMatrix());

	sCommandList->IASetVertexBuffers(0, 1, mesh_->GetVbView()); //VBVを設定

	//wvp用のCBufferの場所を設定
	sCommandList->SetGraphicsRootConstantBufferView(1, worldTransform.transformationMatrixBuff_->GetGPUVirtualAddress());

	//マテリアルCBufferの場所を設定
	sCommandList->SetGraphicsRootConstantBufferView(0, defaultMaterial_->GetMaterialBuff()->GetGPUVirtualAddress());

	// カメラCBufferの場所を設定
	sCommandList->SetGraphicsRootConstantBufferView(4, camera.GetWorldPositionBuff()->GetGPUVirtualAddress());

	//SRVのDescriptorTableの先頭を設定。2はrootParamenter[2]である
	TextureManager::GetInstance()->SetGraphicsRootDescriptorTable(sCommandList, 2, textureHandle_);

	// ポイントライト
	if (pointLightManager_) {
		pointLightManager_->Draw(sCommandList, 5);
	}
	// スポットライト
	if (spotLightManager_) {
		spotLightManager_->Draw(sCommandList, 6);
	}

	//描画
	sCommandList->DrawInstanced(UINT(modelData_.vertices.size()), 1, 0, 0);

}

/// <summary>
/// 描画
/// </summary>
void Model::Draw(WorldTransform& worldTransform, BaseCamera& camera, Material* material) {

	// nullptrチェック
	assert(sDevice);
	assert(sCommandList);

	worldTransform.Map(camera.GetViewProjectionMatrix());

	sCommandList->IASetVertexBuffers(0, 1, mesh_->GetVbView()); //VBVを設定

	//wvp用のCBufferの場所を設定
	sCommandList->SetGraphicsRootConstantBufferView(1, worldTransform.transformationMatrixBuff_->GetGPUVirtualAddress());

	//マテリアルCBufferの場所を設定
	sCommandList->SetGraphicsRootConstantBufferView(0, material->GetMaterialBuff()->GetGPUVirtualAddress());

	// カメラCBufferの場所を設定
	sCommandList->SetGraphicsRootConstantBufferView(4, camera.GetWorldPositionBuff()->GetGPUVirtualAddress());

	//SRVのDescriptorTableの先頭を設定。2はrootParamenter[2]である
	TextureManager::GetInstance()->SetGraphicsRootDescriptorTable(sCommandList, 2, textureHandle_);

	// ポイントライト
	if (pointLightManager_) {
		pointLightManager_->Draw(sCommandList, 5);
	}
	// スポットライト
	if (spotLightManager_) {
		spotLightManager_->Draw(sCommandList, 6);
	}

	//描画
	sCommandList->DrawInstanced(UINT(modelData_.vertices.size()), 1, 0, 0);

}

/// <summary>
/// 描画
/// </summary>
void Model::Draw(WorldTransform& worldTransform, BaseCamera& camera, Material* material,uint32_t textureHandle) {

	// nullptrチェック
	assert(sDevice);
	assert(sCommandList);

	worldTransform.Map(camera.GetViewProjectionMatrix());

	sCommandList->IASetVertexBuffers(0, 1, mesh_->GetVbView()); //VBVを設定

	//wvp用のCBufferの場所を設定
	sCommandList->SetGraphicsRootConstantBufferView(1, worldTransform.transformationMatrixBuff_->GetGPUVirtualAddress());

	//マテリアルCBufferの場所を設定
	sCommandList->SetGraphicsRootConstantBufferView(0, material->GetMaterialBuff()->GetGPUVirtualAddress());

	// カメラCBufferの場所を設定
	sCommandList->SetGraphicsRootConstantBufferView(4, camera.GetWorldPositionBuff()->GetGPUVirtualAddress());

	//SRVのDescriptorTableの先頭を設定。2はrootParamenter[2]である
	TextureManager::GetInstance()->SetGraphicsRootDescriptorTable(sCommandList, 2, textureHandle);

	// ポイントライト
	if (pointLightManager_) {
		pointLightManager_->Draw(sCommandList, 5);
	}
	// スポットライト
	if (spotLightManager_) {
		spotLightManager_->Draw(sCommandList, 6);
	}

	//描画
	sCommandList->DrawInstanced(UINT(modelData_.vertices.size()), 1, 0, 0);

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
	TextureManager::GetInstance()->SetGraphicsRootDescriptorTable(sCommandList, 2, textureHandle_);

	sCommandList->SetGraphicsRootDescriptorTable(1, particleManager->GetInstancingSrvHandleGPU());

	//描画
	sCommandList->DrawInstanced(UINT(modelData_.vertices.size()), particleManager->GetCurrentInstanceIndex(), 0, 0);

}

void Model::OutLineDraw(WorldTransform& worldTransform, BaseCamera& camera, OutLineData& outLineData) {
	// nullptrチェック
	assert(sDevice);
	assert(sCommandList);

	worldTransform.Map(camera.GetViewProjectionMatrix());
	sCommandList->IASetVertexBuffers(0, 1, mesh_->GetVbView()); //VBVを設定

	//wvp用のCBufferの場所を設定
	sCommandList->SetGraphicsRootConstantBufferView(1, worldTransform.transformationMatrixBuff_->GetGPUVirtualAddress());

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
void Model::SetTextureHandle(uint32_t textureHandle) {

	textureHandle_ = textureHandle;
	resourceDesc_ = TextureManager::GetInstance()->GetResourceDesc(textureHandle_);

}


