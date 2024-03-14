#include "RenderTargetTexture.h"
#include "DSVDescriptorHerpManager.h"
#include "RTVDescriptorHerpManager.h"
#include "SRVDescriptorHerpManager.h"

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
		IID_PPV_ARGS(&resource_)); //作成するResourceポインタへのポインタ
	assert(SUCCEEDED(hr));

	//RTVの設定
	D3D12_RENDER_TARGET_VIEW_DESC rtvDesc{};
	rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;//出力結果をSRGBに変換して書き込む
	rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;//2dのテクスチャとして書き込む
	//RTVを作成するDescriptorHeapの場所を決める
	rtvHandle_ = RTVDescriptorHerpManager::GetCPUDescriptorHandle();
	rtvIndexDescriptorHeap_ = RTVDescriptorHerpManager::GetNextIndexDescriptorHeap();
	RTVDescriptorHerpManager::NextIndexDescriptorHeapChange();
	//RTVの生成
	device->CreateRenderTargetView(resource_.Get(), &rtvDesc, rtvHandle_);

	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;//2Dテクスチャ
	srvDesc.Texture2D.MipLevels = 1;

	//SRVを作成するDescriptorHeapの場所を決める
	cpuHandleSRV = SRVDescriptorHerpManager::GetCPUDescriptorHandle();
	gpuHandleSRV = SRVDescriptorHerpManager::GetGPUDescriptorHandle();
	indexDescriptorHeapSRV = SRVDescriptorHerpManager::GetNextIndexDescriptorHeap();
	SRVDescriptorHerpManager::NextIndexDescriptorHeapChange();
	//SRVの生成
	device->CreateShaderResourceView(resource_.Get(), &srvDesc, cpuHandleSRV);

}

void RenderTargetTexture::PreDraw(ID3D12GraphicsCommandList* commandList)
{

	assert(commandList);
	commandList_ = commandList;

	//TransitionBarrierの設定
	D3D12_RESOURCE_BARRIER barrier{};
	//今回のバリアはTransition
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	//Noneにしておく
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	//バリアを張る対象のリソース。現在のバックバッファに対して行う
	barrier.Transition.pResource = resource_.Get();
	//遷移前（現在）のResouceState
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
	//遷移後のResoureState
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
	//TransitionBarrierを張る
	commandList_->ResourceBarrier(1, &barrier);

	//描画先のDSVとRTVを設定する
	commandList_->OMSetRenderTargets(1, &rtvHandle_, false, &dsvHandle_);

	// 全画面クリア
	ClearRenderTarget();

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

	//TransitionBarrierの設定
	D3D12_RESOURCE_BARRIER barrier{};
	//今回のバリアはTransition
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	//Noneにしておく
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	//バリアを張る対象のリソース。現在のバックバッファに対して行う
	barrier.Transition.pResource = resource_.Get();

	//画面に描く処理はすべて終わり、画面に映すので、状態を遷移
	//今回はRenderTargetからPresentにする
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
	//TransitionBarrierを張る
	commandList_->ResourceBarrier(1, &barrier);

	commandList_ = nullptr;

}

void RenderTargetTexture::ClearRenderTarget()
{

	//指定した色で画面全体をクリアする
	float clearColor[] = { 0.1f, 0.25f, 0.5f, 1.0f };//青っぽい色。RGBAの順
	commandList_->ClearRenderTargetView(rtvHandle_, clearColor, 0, nullptr);

}

void RenderTargetTexture::ClearDepthBuffer()
{

	//指定した深度で画面全体をクリアする
	commandList_->ClearDepthStencilView(dsvHandle_, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);

}

void RenderTargetTexture::SetGraphicsRootDescriptorTable(ID3D12GraphicsCommandList* cmdList, uint32_t rootParameterIndex)
{

	assert(commandList_ == nullptr);

	commandList_ = cmdList;

	commandList_->SetGraphicsRootDescriptorTable(rootParameterIndex, gpuHandleSRV);

	// コマンドリストを解除
	commandList_ = nullptr;

}
