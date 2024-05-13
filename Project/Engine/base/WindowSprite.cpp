#include "WindowSprite.h"
#include "WinApp.h"
#include "BufferResource.h"
#include "GraphicsPipelineState/GraphicsPipelineState.h"
#include "SRVDescriptorHerpManager.h"

using namespace Microsoft::WRL;

WindowSprite* WindowSprite::GetInstance()
{
	static WindowSprite instance;
	return &instance;
}

void WindowSprite::Initialize(
	ID3D12Device* device,
	ID3D12GraphicsCommandList* commandList)
{

	device_ = device;
	commandList_ = commandList;

}

void WindowSprite::DrawSRV(const CD3DX12_GPU_DESCRIPTOR_HANDLE& srvGPUHandle)
{

	//RootSignatureを設定。
	commandList_->SetPipelineState(
		GraphicsPipelineState::sPipelineState[GraphicsPipelineState::kPipelineStateIndexWindowSpriteSRV].Get());//PS0を設定
	commandList_->SetGraphicsRootSignature(
		GraphicsPipelineState::sRootSignature[GraphicsPipelineState::kPipelineStateIndexWindowSpriteSRV].Get());

	// SRV
	ID3D12DescriptorHeap* ppHeaps[] = { SRVDescriptorHerpManager::descriptorHeap_.Get() };
	commandList_->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);

	//形状を設定。PS0に設定しているものとは別。
	commandList_->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// シェーダーリソースビューをセット
	commandList_->SetGraphicsRootDescriptorTable(0, srvGPUHandle);

	//描画
	commandList_->DrawInstanced(kVertNum, 1, 0, 0);

}

void WindowSprite::DrawUAV(const CD3DX12_GPU_DESCRIPTOR_HANDLE& uavGPUHandle)
{

	//RootSignatureを設定。
	commandList_->SetPipelineState(
		GraphicsPipelineState::sPipelineState[GraphicsPipelineState::kPipelineStateIndexWindowSpriteUAV].Get());//PS0を設定
	commandList_->SetGraphicsRootSignature(
		GraphicsPipelineState::sRootSignature[GraphicsPipelineState::kPipelineStateIndexWindowSpriteUAV].Get());

	// SRV
	ID3D12DescriptorHeap* ppHeaps[] = { SRVDescriptorHerpManager::descriptorHeap_.Get() };
	commandList_->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);

	//形状を設定。PS0に設定しているものとは別。
	commandList_->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// シェーダーリソースビューをセット
	commandList_->SetGraphicsRootDescriptorTable(0, uavGPUHandle);

	//描画
	commandList_->DrawInstanced(kVertNum, 1, 0, 0);

}
