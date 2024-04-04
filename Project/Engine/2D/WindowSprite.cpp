#include "WindowSprite.h"
#include "../base/WinApp.h"
#include "../base/BufferResource.h"
#include "../base/GraphicsPipelineState/GraphicsPipelineState.h"
#include "../base/SRVDescriptorHerpManager.h"

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

	//Sprite用の頂点リソースを作る
	vertBuff_ = BufferResource::CreateBufferResource(device_, sizeof(SpriteVertex) * kVertNum);

	//リソースの先頭のアドレスから使う
	vbView_.BufferLocation = vertBuff_->GetGPUVirtualAddress();
	//使用するリソースのサイズは頂点6つ分のサイズ
	vbView_.SizeInBytes = sizeof(SpriteVertex) * kVertNum;
	//1頂点あたりのサイズ
	vbView_.StrideInBytes = sizeof(SpriteVertex);

	//書き込むためのアドレスを取得
	vertBuff_->Map(0, nullptr, reinterpret_cast<void**>(&vertMap));

	//インデックスリソースを作る
	indexBuff_ = BufferResource::CreateBufferResource(device_, sizeof(uint32_t) * kVertNum);

	//インデックスバッファビュー
	//リソースの先頭のアドレスから使う
	ibView_.BufferLocation = indexBuff_->GetGPUVirtualAddress();
	//使用するリソースのサイズはインデックス6つ分のサイズ
	ibView_.SizeInBytes = sizeof(uint32_t) * kVertNum;
	//インデックスはuint32_tとする
	ibView_.Format = DXGI_FORMAT_R32_UINT;

	//インデックスリソースにデータを書き込む
	indexBuff_->Map(0, nullptr, reinterpret_cast<void**>(&indexMap));

	// 頂点データ
	float left = -1.0f;
	float right = 1.0f;
	float top = 1.0f;
	float bottom = -1.0f;

	//一枚目の三角形
	vertMap[0].position = { left, bottom, 0.0f, 1.0f };//左下
	vertMap[0].texcoord = { 0.0f, 1.0f };
	vertMap[0].normal = { 0.0f, 0.0f, -1.0f };
	vertMap[1].position = { left, top, 0.0f, 1.0f };//左上
	vertMap[1].texcoord = { 0.0f, 0.0f };
	vertMap[1].normal = { 0.0f, 0.0f, -1.0f };
	vertMap[2].position = { right, bottom, 0.0f, 1.0f };//右下
	vertMap[2].texcoord = { 1.0f, 1.0f };
	vertMap[2].normal = { 0.0f, 0.0f, -1.0f };
	//ニ枚目の三角形
	vertMap[3].position = { right, top, 0.0f, 1.0f };//右上
	vertMap[3].texcoord = { 1.0f, 0.0f };
	vertMap[3].normal = { 0.0f, 0.0f, -1.0f };
	vertMap[4].position = { left, top, 0.0f, 1.0f };//左上
	vertMap[4].texcoord = { 0.0f, 0.0f };
	vertMap[4].normal = { 0.0f, 0.0f, -1.0f };
	vertMap[5].position = { right, bottom, 0.0f, 1.0f };//右下
	vertMap[5].texcoord = { 1.0f, 1.0f };
	vertMap[5].normal = { 0.0f, 0.0f, -1.0f };

	//インデックスリソースにデータを書き込む
	indexMap[0] = 0;
	indexMap[1] = 1;
	indexMap[2] = 2;
	indexMap[3] = 1;
	indexMap[4] = 3;
	indexMap[5] = 2;


}

void WindowSprite::Draw(const CD3DX12_GPU_DESCRIPTOR_HANDLE& srvGPUHandle)
{

	//RootSignatureを設定。
	commandList_->SetPipelineState(
		GraphicsPipelineState::sPipelineState[GraphicsPipelineState::kPipelineStateNameSwapChain].Get());//PS0を設定
	commandList_->SetGraphicsRootSignature(
		GraphicsPipelineState::sRootSignature[GraphicsPipelineState::kPipelineStateNameSwapChain].Get());

	// SRV
	ID3D12DescriptorHeap* ppHeaps[] = { SRVDescriptorHerpManager::descriptorHeap_.Get() };
	commandList_->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);

	//形状を設定。PS0に設定しているものとは別。
	commandList_->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// 頂点バッファの設定
	commandList_->IASetVertexBuffers(0, 1, &vbView_);
	//IBVを設定
	commandList_->IASetIndexBuffer(&ibView_);

	// シェーダーリソースビューをセット
	commandList_->SetGraphicsRootDescriptorTable(0, srvGPUHandle);

	//描画
	commandList_->DrawIndexedInstanced(kVertNum, 1, 0, 0, 0);

}
