#include "RenderTargetTexture.h"
#include "DSVDescriptorHerpManager.h"
#include "RTVDescriptorHerpManager.h"
#include "SRVDescriptorHerpManager.h"
#include "../Math/Matrix4x4.h"
#include "BufferResource.h"

void RenderTargetTexture::Initialize(
	ID3D12Device* device,
	int32_t backBufferWidth,
	int32_t backBufferHeight)
{

	HRESULT hr;

	assert(device);

	backBufferWidth_ = backBufferWidth;
	backBufferHeight_ = backBufferHeight;

	//利用するHeapの設定
	D3D12_HEAP_PROPERTIES heapProperties{};
	heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT; //デフォルト

	// DSVリソース
	//DepthStencilTextureをウィンドウのサイズで作成
	D3D12_RESOURCE_DESC dsvResourceDesc{};
	dsvResourceDesc.Width = backBufferWidth_;
	dsvResourceDesc.Height = backBufferHeight_;
	dsvResourceDesc.MipLevels = 1;
	dsvResourceDesc.DepthOrArraySize = 1;
	dsvResourceDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	dsvResourceDesc.SampleDesc.Count = 1;
	dsvResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	dsvResourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

	//深度値のクリア設定
	D3D12_CLEAR_VALUE depthClearValue{};
	depthClearValue.DepthStencil.Depth = 1.0f;
	depthClearValue.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;

	//Resoureの生成
	dsvResource_ = nullptr;
	hr = device->CreateCommittedResource(
		&heapProperties,
		D3D12_HEAP_FLAG_NONE,
		&dsvResourceDesc,
		D3D12_RESOURCE_STATE_DEPTH_WRITE,
		&depthClearValue,
		IID_PPV_ARGS(&dsvResource_));
	assert(SUCCEEDED(hr));

	//DSVの設定
	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc{};
	dsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	//DSVを作成するDescriptorHeapの場所を決める
	dsvHandle_ = DSVDescriptorHerpManager::GetCPUDescriptorHandle();
	dsvIndexDescriptorHeap_ = DSVDescriptorHerpManager::GetNextIndexDescriptorHeap();
	DSVDescriptorHerpManager::NextIndexDescriptorHeapChange();
	//DSVの生成
	device->CreateDepthStencilView(dsvResource_.Get(), &dsvDesc, dsvHandle_);

	for (uint32_t i = 0; i < kResourceNum_; ++i) {

		// RTVリソース
		D3D12_RESOURCE_DESC rtvResouceDesc;
		rtvResouceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		rtvResouceDesc.Alignment = 0;
		rtvResouceDesc.Width = backBufferWidth_;
		rtvResouceDesc.Height = backBufferHeight_;
		rtvResouceDesc.DepthOrArraySize = 1;
		rtvResouceDesc.MipLevels = 0;
		rtvResouceDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
		rtvResouceDesc.SampleDesc.Count = 1;
		rtvResouceDesc.SampleDesc.Quality = 0;
		rtvResouceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;
		rtvResouceDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;

		//Resoureの生成
		hr = device->CreateCommittedResource(
			&heapProperties, //Heapの設定
			D3D12_HEAP_FLAG_NONE, //Heapの特殊な設定。特になし。
			&rtvResouceDesc, //Resourceの設定
			D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, //データ転送される設定
			nullptr, //Clear最適値。使わないのでnullptr
			IID_PPV_ARGS(&resources_[i])); //作成するResourceポインタへのポインタ
		assert(SUCCEEDED(hr));

		//RTVの設定
		D3D12_RENDER_TARGET_VIEW_DESC rtvDesc{};
		rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;//出力結果をSRGBに変換して書き込む
		rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;//2dのテクスチャとして書き込む
		//RTVを作成するDescriptorHeapの場所を決める
		rtvHandles_[i] = RTVDescriptorHerpManager::GetCPUDescriptorHandle();
		rtvIndexDescriptorHeaps_[i] = RTVDescriptorHerpManager::GetNextIndexDescriptorHeap();
		RTVDescriptorHerpManager::NextIndexDescriptorHeapChange();
		//RTVの生成
		device->CreateRenderTargetView(resources_[i].Get(), &rtvDesc, rtvHandles_[i]);

		D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
		srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
		srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;//2Dテクスチャ
		srvDesc.Texture2D.MipLevels = 1;

		//SRVを作成するDescriptorHeapの場所を決める
		srvCPUHandles_[i] = SRVDescriptorHerpManager::GetCPUDescriptorHandle();
		srvGPUHandles_[i] = SRVDescriptorHerpManager::GetGPUDescriptorHandle();
		srvIndexDescriptorHeaps_[i] = SRVDescriptorHerpManager::GetNextIndexDescriptorHeap();
		SRVDescriptorHerpManager::NextIndexDescriptorHeapChange();
		//SRVの生成
		device->CreateShaderResourceView(resources_[i].Get(), &srvDesc, srvCPUHandles_[i]);

		// レンダーターゲットか
		isRenderTarget_[i] = false;

	}

	TextureInitialize(device);

}

void RenderTargetTexture::PreDraw(ID3D12GraphicsCommandList* commandList)
{

	assert(commandList);
	commandList_ = commandList;

	for (uint32_t i = 0; i < kResourceNum_; ++i) {
		ChangeRenderTarget(i);
	}

	//描画先のDSVとRTVを設定する
	commandList_->OMSetRenderTargets(kResourceNum_, rtvHandles_, false, &dsvHandle_);

	// 全画面クリア
	for (uint32_t i = 0; i < kResourceNum_; ++i) {
		ClearRenderTarget(i);
	}

	// 深度値クリア
	ClearDepthBuffer();

	//ビューポート
	D3D12_VIEWPORT viewport{};
	//クライアント領域のサイズと一緒にして画面全体に表示
	viewport.Width = static_cast<float>(backBufferWidth_);
	viewport.Height = static_cast<float>(backBufferHeight_);
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	commandList_->RSSetViewports(1, &viewport);//Viewportを設定

	//シザー矩形
	D3D12_RECT scissorRect{};
	//基本的にビューポートと同じ矩形が構成されるようにする
	scissorRect.left = 0;
	scissorRect.right = backBufferWidth_;
	scissorRect.top = 0;
	scissorRect.bottom = backBufferHeight_;
	commandList_->RSSetScissorRects(1, &scissorRect);//Scirssorを設定


}

void RenderTargetTexture::PostDraw()
{

	for (uint32_t i = 0; i < kResourceNum_; ++i) {
		if (isRenderTarget_[i]) {
			ChangePixelShaderResource(i);
		}
	}

	commandList_ = nullptr;

}

void RenderTargetTexture::ClearRenderTarget(uint32_t resourceIndex)
{

	//指定した色で画面全体をクリアする
	float clearColor[] = { 0.1f, 0.25f, 0.5f, 1.0f };//青っぽい色。RGBAの順
	commandList_->ClearRenderTargetView(rtvHandles_[resourceIndex], clearColor, 0, nullptr);

}

void RenderTargetTexture::ClearDepthBuffer()
{

	//指定した深度で画面全体をクリアする
	commandList_->ClearDepthStencilView(dsvHandle_, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);

}

void RenderTargetTexture::SetGraphicsRootDescriptorTable(ID3D12GraphicsCommandList* cmdList, uint32_t rootParameterIndex, uint32_t resourceIndex)
{

	assert(commandList_ == nullptr);

	commandList_ = cmdList;

	commandList_->SetGraphicsRootDescriptorTable(rootParameterIndex, srvGPUHandles_[resourceIndex]);

	// コマンドリストを解除
	commandList_ = nullptr;

}

void RenderTargetTexture::ChangeRenderTarget(uint32_t resourceIndex)
{

	assert(commandList_);
	assert(!isRenderTarget_[resourceIndex]);

	//TransitionBarrierの設定
	D3D12_RESOURCE_BARRIER barrier{};
	//今回のバリアはTransition
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	//Noneにしておく
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	//バリアを張る対象のリソース。
	barrier.Transition.pResource = resources_[resourceIndex].Get();
	//遷移前（現在）のResouceState
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
	//遷移後のResoureState
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
	//TransitionBarrierを張る
	commandList_->ResourceBarrier(1, &barrier);

}

void RenderTargetTexture::ChangePixelShaderResource(uint32_t resourceIndex)
{

	assert(commandList_);
	assert(isRenderTarget_[resourceIndex]);

	//TransitionBarrierの設定
	D3D12_RESOURCE_BARRIER barrier{};
	//今回のバリアはTransition
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	//Noneにしておく
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	//バリアを張る対象のリソース。
	barrier.Transition.pResource = resources_[resourceIndex].Get();

	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
	//TransitionBarrierを張る
	commandList_->ResourceBarrier(1, &barrier);

}

void RenderTargetTexture::TextureInitialize(ID3D12Device* device)
{

	//Sprite用の頂点リソースを作る
	vertBuff_ = BufferResource::CreateBufferResource(device, sizeof(SpriteVertex) * 6);

	//リソースの先頭のアドレスから使う
	vbView_.BufferLocation = vertBuff_->GetGPUVirtualAddress();
	//使用するリソースのサイズは頂点6つ分のサイズ
	vbView_.SizeInBytes = sizeof(SpriteVertex) * 6;
	//1頂点あたりのサイズ
	vbView_.StrideInBytes = sizeof(SpriteVertex);

	//書き込むためのアドレスを取得
	vertBuff_->Map(0, nullptr, reinterpret_cast<void**>(&vertMap));

	//インデックスリソースを作る
	indexBuff_ = BufferResource::CreateBufferResource(device, sizeof(uint32_t) * 6);

	//インデックスバッファビュー
	//リソースの先頭のアドレスから使う
	ibView_.BufferLocation = indexBuff_->GetGPUVirtualAddress();
	//使用するリソースのサイズはインデックス6つ分のサイズ
	ibView_.SizeInBytes = sizeof(uint32_t) * 6;
	//インデックスはuint32_tとする
	ibView_.Format = DXGI_FORMAT_R32_UINT;

	//インデックスリソースにデータを書き込む
	indexBuff_->Map(0, nullptr, reinterpret_cast<void**>(&indexMap));

	// 頂点データ
	float left = 0.0f;
	float right = static_cast<float>(backBufferWidth_);
	float top = 0.0f;
	float bottom = static_cast<float>(backBufferHeight_);

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

	Matrix4x4 viewMatrixSprite = Matrix4x4::MakeIdentity4x4();
	Matrix4x4 projectionMatrixSprite = Matrix4x4::MakeOrthographicMatrix(0.0f, 0.0f, float(backBufferWidth_), float(backBufferHeight_), 0.0f, 100.0f);
	Matrix4x4 viewProjectionMatrixSprite = Matrix4x4::Multiply(viewMatrixSprite, projectionMatrixSprite);
	for (uint32_t i = 0; i < 6; ++i) {
		Vector3 pos = Vector3{ vertMap[i].position.x, vertMap[i].position.y, vertMap[i].position.z };
		pos = Matrix4x4::Transform(pos, viewProjectionMatrixSprite);
		vertMap[i].position.x = pos.x;
		vertMap[i].position.y = pos.y;
		vertMap[i].position.z = pos.z;
	}

	//インデックスリソースにデータを書き込む
	indexMap[0] = 0;
	indexMap[1] = 1;
	indexMap[2] = 2;
	indexMap[3] = 1;
	indexMap[4] = 3;
	indexMap[5] = 2;

}

void RenderTargetTexture::TextureDraw(
	GraphicsPipelineState::PipelineStateName pipelineStateName,
	uint32_t resourceIndex,
	ID3D12GraphicsCommandList* commandList)
{

	postRootSignature_ = GraphicsPipelineState::sRootSignature[pipelineStateName].Get();
	postPipelineState_ = GraphicsPipelineState::sPipelineState[pipelineStateName].Get();

	//RootSignatureを設定。
	commandList->SetPipelineState(postPipelineState_);//PS0を設定
	commandList->SetGraphicsRootSignature(postRootSignature_);
	//形状を設定。PS0に設定しているものとは別。
	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// 頂点バッファの設定
	commandList->IASetVertexBuffers(0, 1, &vbView_);
	//IBVを設定
	commandList->IASetIndexBuffer(&ibView_);

	//ID3D12DescriptorHeap* ppHeaps[] = { DescriptorHerpManager::descriptorHeap_.Get() };
	//commandList_->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);

	//シェーダーリソースビューをセット
	SetGraphicsRootDescriptorTable(commandList, 0, resourceIndex);

	//描画
	commandList->DrawIndexedInstanced(6, 1, 0, 0, 0);

}
