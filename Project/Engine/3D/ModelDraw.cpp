#include "ModelDraw.h"
#include "../base/SRVDescriptorHerpManager.h"
#include "../base/TextureManager.h"

using namespace DirectX;
using namespace Microsoft::WRL;

// コマンドリスト
ID3D12GraphicsCommandList* ModelDraw::sCommandList = nullptr;
// ルートシグネチャ
ID3D12RootSignature* ModelDraw::sRootSignature[PipelineStateIndex::kPipelineStateIndexOfCount];
// パイプラインステートオブジェクト
ID3D12PipelineState* ModelDraw::sPipelineState[PipelineStateIndex::kPipelineStateIndexOfCount];
//	平行光源
DirectionalLight* ModelDraw::sDirectionalLight_ = nullptr;
// ポイントライトマネージャ
PointLightManager* ModelDraw::sPointLightManager_ = nullptr;
//	スポットライトマネージャ
SpotLightManager* ModelDraw::sSpotLightManager_ = nullptr;
// 霧マネージャー
FogManager* ModelDraw::sFogManager_ = nullptr;

void ModelDraw::Initialize(
	const std::array<ID3D12RootSignature*, PipelineStateIndex::kPipelineStateIndexOfCount>& rootSignature,
	const std::array<ID3D12PipelineState*, PipelineStateIndex::kPipelineStateIndexOfCount>& pipelineState)
{

	// グラフィックパイプライン生成
	for (uint32_t i = 0u; i < PipelineStateIndex::kPipelineStateIndexOfCount; i++) {
		sRootSignature[i] = rootSignature[i];
		sPipelineState[i] = pipelineState[i];
	}

}

void ModelDraw::PreDraw(const PreDrawDesc& desc)
{

	assert(sCommandList == nullptr);

	sCommandList = desc.commandList;

	//RootSignatureを設定。
	sCommandList->SetPipelineState(sPipelineState[desc.pipelineStateIndex]);//PS0を設定
	sCommandList->SetGraphicsRootSignature(sRootSignature[desc.pipelineStateIndex]);

	// SRV
	ID3D12DescriptorHeap* ppHeaps[] = { SRVDescriptorHerpManager::descriptorHeap_.Get() };
	sCommandList->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);

	//形状を設定。PS0に設定しているものとは別。
	sCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	sDirectionalLight_ = desc.directionalLight;
	sPointLightManager_ = desc.pointLightManager;
	sSpotLightManager_ = desc.spotLightManager;
	sFogManager_ = desc.fogManager;

}

void ModelDraw::PostDraw()
{

	// コマンドリストを解除
	sCommandList = nullptr;

	sDirectionalLight_ = nullptr;
	sPointLightManager_ = nullptr;
	sSpotLightManager_ = nullptr;
	sFogManager_ = nullptr;

}

void ModelDraw::AnimObjectDraw(AnimObjectDesc& desc)
{

	// nullptrチェック
	assert(sCommandList);

	// ワールドトランスフォームマップ処理
	desc.worldTransform->Map(desc.camera->GetViewProjectionMatrix());

	//VBVを設定 (インフルエンスと合体)
	D3D12_VERTEX_BUFFER_VIEW vbvs[2] = {
		*(desc.model->GetMesh())->GetVbView(),
		*(desc.model->GetMesh())->GetInfluenceView()
	};
	sCommandList->IASetVertexBuffers(0, 2, vbvs);

	//マテリアルCBufferの場所を設定
	if (desc.material) {
		sCommandList->SetGraphicsRootConstantBufferView(0, desc.material->GetMaterialBuff()->GetGPUVirtualAddress());
	}
	else {
		sCommandList->SetGraphicsRootConstantBufferView(0, Model::GetDefaultMaterial()->GetMaterialBuff()->GetGPUVirtualAddress());
	}

	// 平行光源
	if (sDirectionalLight_) {
		sDirectionalLight_->Draw(sCommandList, 1);
	}

	// カメラCBufferの場所を設定
	sCommandList->SetGraphicsRootConstantBufferView(2, desc.camera->GetWorldPositionBuff()->GetGPUVirtualAddress());

	// ワールドトランスフォーム
	sCommandList->SetGraphicsRootConstantBufferView(3, desc.worldTransform->GetTransformationMatrixBuff()->GetGPUVirtualAddress());

	// ローカル行列
	desc.localMatrixManager->SetGraphicsRootDescriptorTable(sCommandList, 4);

	//テクスチャ
	if (desc.textureHandles.empty()) {
		for (size_t i = 0; i < desc.model->GetModelData().material.textureFilePaths.size(); ++i) {
			TextureManager::GetInstance()->SetGraphicsRootDescriptorTable(sCommandList, 5 + static_cast<UINT>(i), desc.model->GetTextureHandles()[i]);
		}
	}
	else {
		for (size_t i = 0; i < desc.textureHandles.size(); ++i) {
			TextureManager::GetInstance()->SetGraphicsRootDescriptorTable(sCommandList, 5 + static_cast<UINT>(i), desc.textureHandles[i]);
		}
	}

	// ポイントライト
	if (sPointLightManager_) {
		sPointLightManager_->Draw(sCommandList, 9);
	}
	// スポットライト
	if (sSpotLightManager_) {
		sSpotLightManager_->Draw(sCommandList, 10);
	}

	// 霧
	sCommandList->SetGraphicsRootConstantBufferView(11, sFogManager_->GetFogDataBuff()->GetGPUVirtualAddress());

	//描画
	sCommandList->DrawInstanced(UINT(desc.model->GetModelData().vertices.size()), 1, 0, 0);

}

void ModelDraw::ParticleDraw(ParticleDesc& desc)
{
	// nullptrチェック
	assert(sCommandList);

	ParticleManager* particleManager = ParticleManager::GetInstance();

	sCommandList->IASetVertexBuffers(0, 1, desc.model->GetMesh()->GetVbView()); //VBVを設定

	//マテリアルを設定
	sCommandList->SetGraphicsRootConstantBufferView(0, Model::GetDefaultMaterial()->GetMaterialBuff()->GetGPUVirtualAddress());

	// 開始位置を設定
	sCommandList->SetGraphicsRootConstantBufferView(3, particleManager->GetCurrentStartInstanceIdBuff()->GetGPUVirtualAddress());

	//SRVのDescriptorTableの先頭を設定。2はrootParamenter[2]である
	for (size_t i = 0; i < desc.model->GetModelData().material.textureFilePaths.size(); ++i) {
		TextureManager::GetInstance()->SetGraphicsRootDescriptorTable(sCommandList, 2, desc.model->GetTextureHandles()[i]);
	}

	sCommandList->SetGraphicsRootDescriptorTable(1, particleManager->GetInstancingSrvHandleGPU());

	//描画
	sCommandList->DrawInstanced(UINT(desc.model->GetModelData().vertices.size()), particleManager->GetCurrentInstanceIndex(), 0, 0);

}

void ModelDraw::ManyAnimObjectsDraw(ManyAnimObjectsDesc& desc)
{

	// nullptrチェック
	assert(sCommandList);

	//VBVを設定 (インフルエンスと合体)
	D3D12_VERTEX_BUFFER_VIEW vbvs[2] = {
		*(desc.model->GetMesh())->GetVbView(),
		*(desc.model->GetMesh())->GetInfluenceView()
	};
	sCommandList->IASetVertexBuffers(0, 2, vbvs);

	//マテリアルCBufferの場所を設定
	if (desc.material) {
		sCommandList->SetGraphicsRootConstantBufferView(0, desc.material->GetMaterialBuff()->GetGPUVirtualAddress());
	}
	else {
		sCommandList->SetGraphicsRootConstantBufferView(0, Model::GetDefaultMaterial()->GetMaterialBuff()->GetGPUVirtualAddress());
	}

	// カメラCBufferの場所を設定
	sCommandList->SetGraphicsRootConstantBufferView(7, desc.camera->GetWorldPositionBuff()->GetGPUVirtualAddress());


	//SRVのDescriptorTableの先頭を設定。2はrootParamenter[2]である
	for (size_t i = 0; i < desc.model->GetModelData().material.textureFilePaths.size(); ++i) {
		TextureManager::GetInstance()->SetGraphicsRootDescriptorTable(sCommandList, 2 + static_cast<UINT>(i), desc.model->GetTextureHandles()[i]);
	}

	// ポイントライト
	if (sPointLightManager_) {
		sPointLightManager_->Draw(sCommandList, 8);
	}
	// スポットライト
	if (sSpotLightManager_) {
		sSpotLightManager_->Draw(sCommandList, 9);
	}
	// 平行光源
	if (sDirectionalLight_) {
		sDirectionalLight_->Draw(sCommandList, 6);
	}

	// ワールドトランスフォーム
	sCommandList->SetGraphicsRootDescriptorTable(10, *desc.transformationMatrixesHandle);
	// ローカル
	sCommandList->SetGraphicsRootDescriptorTable(1, *desc.localMatrixesHandle);
	// 霧
	sCommandList->SetGraphicsRootConstantBufferView(11, sFogManager_->GetFogDataBuff()->GetGPUVirtualAddress());

	//描画
	sCommandList->DrawInstanced(UINT(desc.model->GetModelData().vertices.size()), desc.numInstance, 0, 0);

}
