#include "SwapChain.h"
#include <cassert>
#include "RTVDescriptorHerpManager.h"
#include "DSVDescriptorHerpManager.h"
#include "../Math/Matrix4x4.h"
#include "BufferResource.h"

SwapChain* SwapChain::GetInstance()
{
	static SwapChain instance;
	return &instance;
}

void SwapChain::Initialize(
	int32_t backBufferWidth,
	int32_t backBufferHeight,
	DXGIDevice* dxgiDevice,
	ID3D12CommandQueue* commandQueue,
	WinApp* winApp)
{

	backBufferWidth_ = backBufferWidth;
	backBufferHeight_ = backBufferHeight;

	//スワップチェーンを生成する
	swapChain_ = nullptr;
	DXGI_SWAP_CHAIN_DESC1 swapChainDesc{};
	swapChainDesc.Width = backBufferWidth_;//画面の幅。ウィンドウのクライアント領域を同じものにしておく
	swapChainDesc.Height = backBufferHeight_;//画面の高さ。ウィンドウのクライアント領域を同じものにしておく
	swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;//色の形式
	swapChainDesc.SampleDesc.Count = 1;//マルチサンプルしない
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;//描画のターゲットとして利用する
	swapChainDesc.BufferCount = kSwapChainNum_;//ダブルバッファ
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;//モニタにうつしたら、中身を破棄
	//コマンドキュー、ウィンドウハンドル、設定を渡して生成する
	HRESULT hr = dxgiDevice->GetFactory()->CreateSwapChainForHwnd(commandQueue, winApp->GetHwnd(), &swapChainDesc, nullptr, nullptr, reinterpret_cast<IDXGISwapChain1**>(swapChain_.GetAddressOf()));
	assert(SUCCEEDED(hr));

	// RTVリソース
	for (uint32_t i = 0; i < kSwapChainNum_; i++)
	{
		
		//SwapChainからResourceを引っ張ってくる
		rtvResources_[i] = { nullptr };
		hr = swapChain_->GetBuffer(i, IID_PPV_ARGS(&rtvResources_[i]));
		//うまく取得できなければ起動できない
		assert(SUCCEEDED(hr));

		//RTVの設定
		D3D12_RENDER_TARGET_VIEW_DESC rtvDesc{};
		rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;//出力結果をSRGBに変換して書き込む
		rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;//2dのテクスチャとして書き込む

		//RTVを作成するDescriptorHeapの場所を決める
		rtvHandles_[i] = RTVDescriptorHerpManager::GetCPUDescriptorHandle();
		rtvIndexDescriptorHeaps_[i] = RTVDescriptorHerpManager::GetNextIndexDescriptorHeap();
		RTVDescriptorHerpManager::NextIndexDescriptorHeapChange();
		//SRVの生成
		dxgiDevice->GetDevice()->CreateRenderTargetView(rtvResources_[i].Get(), &rtvDesc, rtvHandles_[i]);

	}

	// DSVリソース
	//DepthStencilTextureをウィンドウのサイズで作成
	D3D12_RESOURCE_DESC resourceDesc{};
	resourceDesc.Width = backBufferWidth_;
	resourceDesc.Height = backBufferHeight_;
	resourceDesc.MipLevels = 1;
	resourceDesc.DepthOrArraySize = 1;
	resourceDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	resourceDesc.SampleDesc.Count = 1;
	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	resourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

	//利用するHeapの設定
	D3D12_HEAP_PROPERTIES heapProperties{};
	heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;

	//深度値のクリア設定
	D3D12_CLEAR_VALUE depthClearValue{};
	depthClearValue.DepthStencil.Depth = 1.0f;
	depthClearValue.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;

	//Resoureの生成
	hr = dxgiDevice->GetDevice()->CreateCommittedResource(
		&heapProperties,
		D3D12_HEAP_FLAG_NONE,
		&resourceDesc,
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
	dxgiDevice->GetDevice()->CreateDepthStencilView(dsvResource_.Get(), &dsvDesc, dsvHandle_);

	DrawInitialize(dxgiDevice->GetDevice());

}

void SwapChain::PreDraw(ID3D12GraphicsCommandList* commandList)
{

	assert(commandList);
	commandList_ = commandList;

	//これから書き込むバックバッファのインデックスを取得
	UINT backBufferIndex = swapChain_->GetCurrentBackBufferIndex();

	//TransitionBarrierの設定
	D3D12_RESOURCE_BARRIER barrier{};
	//今回のバリアはTransition
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	//Noneにしておく
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	//バリアを張る対象のリソース。現在のバックバッファに対して行う
	barrier.Transition.pResource = rtvResources_[backBufferIndex].Get();
	//遷移前（現在）のResouceState
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
	//遷移後のResoureState
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
	//TransitionBarrierを張る
	commandList_->ResourceBarrier(1, &barrier);

	//描画先のDSVとRTVを設定する
	commandList_->OMSetRenderTargets(1, &rtvHandles_[backBufferIndex], false, &dsvHandle_);

	// 全画面クリア
	//指定した色で画面全体をクリアする
	float clearColor[] = { 0.1f, 0.25f, 0.5f, 1.0f };//青っぽい色。RGBAの順
	commandList_->ClearRenderTargetView(rtvHandles_[backBufferIndex], clearColor, 0, nullptr);

	// 深度値クリア
	//指定した深度で画面全体をクリアする
	commandList_->ClearDepthStencilView(dsvHandle_, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);

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

void SwapChain::PostDraw()
{

	//これから書き込むバックバッファのインデックスを取得
	UINT backBufferIndex = swapChain_->GetCurrentBackBufferIndex();

	//TransitionBarrierの設定
	D3D12_RESOURCE_BARRIER barrier{};
	//今回のバリアはTransition
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	//Noneにしておく
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	//バリアを張る対象のリソース。現在のバックバッファに対して行う
	barrier.Transition.pResource = rtvResources_[backBufferIndex].Get();

	//画面に描く処理はすべて終わり、画面に映すので、状態を遷移
	//今回はRenderTargetからPresentにする
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
	//TransitionBarrierを張る
	commandList_->ResourceBarrier(1, &barrier);
	
	commandList_ = nullptr;

}

void SwapChain::Draw(ID3D12GraphicsCommandList* commandList,
	RenderTargetTexture* renderTargetTexture)
{

	PreDraw(commandList);

	//RootSignatureを設定。
	commandList_->SetPipelineState(postPipelineState_);//PS0を設定
	commandList_->SetGraphicsRootSignature(postRootSignature_);
	//形状を設定。PS0に設定しているものとは別。
	commandList_->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// 頂点バッファの設定
	commandList_->IASetVertexBuffers(0, 1, &vbView_);
	//IBVを設定
	commandList_->IASetIndexBuffer(&ibView_);

	//ID3D12DescriptorHeap* ppHeaps[] = { DescriptorHerpManager::descriptorHeap_.Get() };
	//commandList_->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);

	//シェーダーリソースビューをセット
	renderTargetTexture->SetGraphicsRootDescriptorTable(commandList_, 0, 0);

	//描画
	commandList_->DrawIndexedInstanced(6, 1, 0, 0, 0);

	PostDraw();

}

void SwapChain::DrawInitialize(
	ID3D12Device* device)
{

	postRootSignature_ = GraphicsPipelineState::sRootSignature[GraphicsPipelineState::kPipelineStateNamePostEffect].Get();
	postPipelineState_ = GraphicsPipelineState::sPipelineState[GraphicsPipelineState::kPipelineStateNamePostEffect].Get();

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
