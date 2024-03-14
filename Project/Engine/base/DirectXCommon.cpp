#include "DirectXCommon.h"
#include "SafeDelete.h"
#include <algorithm>
#include <cassert>
#include <thread>
#include <timeapi.h>
#include <vector>
#include "SRVDescriptorHerpManager.h"
#include "RTVDescriptorHerpManager.h"
#include "DSVDescriptorHerpManager.h"
#include "BufferResource.h"
#include "../Math/Matrix4x4.h"
#include "Log.h"

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "Winmm.lib")

using namespace Microsoft::WRL;

DirectXCommon* DirectXCommon::GetInstance() {
	static DirectXCommon instance;
	return &instance;
}

// 初期化
void DirectXCommon::Initialize(
	WinApp* winApp, int32_t backBufferWidth,
	int32_t backBufferHeight) {

	// nullptrチェック
	assert(winApp);
	assert(4 <= backBufferWidth && backBufferWidth <= 4096);
	assert(4 <= backBufferHeight && backBufferHeight <= 4096);

	winApp_ = winApp;
	backBufferWidth_ = backBufferWidth;
	backBufferHeight_ = backBufferHeight;

	// FPS固定初期化
	InitializeFixFPS();

	// DXGIデバイス初期化
	dxgiDevice_ = DXGIDevice::GetInstance();
	dxgiDevice_->Initialize();

	// ディスクリプタヒープ初期化
	RTVDescriptorHerpManager::Initialize(dxgiDevice_->GetDevice());
	SRVDescriptorHerpManager::Initialize(dxgiDevice_->GetDevice());
	DSVDescriptorHerpManager::Initialize(dxgiDevice_->GetDevice());

	// コマンド関連初期化
	DxCommand::StaticInitialize(dxgiDevice_->GetDevice());
	command_ = std::make_unique<DxCommand>();
	command_->Initialize();
	loadCommand_ = std::make_unique<DxCommand>();
	loadCommand_->Initialize();

	// スワップチェーンの生成
	swapChain_ = SwapChain::GetInstance();
	swapChain_->Initialize(
		backBufferWidth_,
		backBufferHeight_,
		dxgiDevice_,
		DxCommand::GetCommandQueue(),
		winApp_);

	renderTargetTexture_ = std::make_unique<RenderTargetTexture>();
	renderTargetTexture_->Initialize(
		dxgiDevice_->GetDevice(), 
		backBufferWidth_, 
		backBufferHeight_);

	// フェンス生成
	CreateFence();

}

// 描画前処理
void DirectXCommon::PreDraw() {

	renderTargetTexture_->PreDraw(command_->GetCommadList());

}
// 描画後処理
void DirectXCommon::PostDraw() {

	renderTargetTexture_->PostDraw();

	//////////////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////////////////////////

	swapChain_->PreDraw(command_->GetCommadList());

	//ポストエフェクト
	PostEffect();

	swapChain_->PostDraw();

	//////////////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////////////////////////


	//コマンドリストの内容を確定させる。すべてのコマンドを積んでからCloseすること
	HRESULT hr = command_->GetCommadList()->Close();
	assert(SUCCEEDED(hr));

	//GPUにコマンドリストの実行を行わせる
	ID3D12CommandList* commandLists[] = { command_->GetCommadList() };
	DxCommand::GetCommandQueue()->ExecuteCommandLists(1, commandLists);
	//GPUとOSに画面の交換を行うように通知する
	swapChain_->Present();

	//Fenceの値を更新
	fenceVal_++;
	//GPUがここまでたどり着いたときに、Fenceの値を指定した値に代入するようにSignalを送る
	DxCommand::GetCommandQueue()->Signal(fence_.Get(), fenceVal_);

	//Fenceの値が指定したSignal値にたどり着いているが確認する
	//GetCompletedValueの初期値はFence作成時に渡した初期値
	if (fence_->GetCompletedValue() < fenceVal_) {
		//FrenceのSignalを持つためのイベントを作成する
		HANDLE fenceEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
		assert(fenceEvent != nullptr);
		//指定したSignalにたどりついていないので、たどりつくまで待つようにイベントを設定する
		fence_->SetEventOnCompletion(fenceVal_, fenceEvent);
		//イベントを待つ
		WaitForSingleObject(fenceEvent, INFINITE);
	}

	// FPS固定
	UpdateFixFPS();

	//次のフレーム用のコマンドリストを準備
	hr = command_->GetCommandAllocator()->Reset();
	assert(SUCCEEDED(hr));
	hr = command_->GetCommadList()->Reset(command_->GetCommandAllocator(), nullptr);
	assert(SUCCEEDED(hr));

}

// 深度バッファのクリア
void DirectXCommon::ClearDepthBuffer() {

	renderTargetTexture_->ClearDepthBuffer();

}

// フェンス生成
void DirectXCommon::CreateFence() {

	//初期化0でFenceを作る
	HRESULT hr = dxgiDevice_->GetDevice()->CreateFence(fenceVal_, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence_));
	assert(SUCCEEDED(hr));

	//FrenceのSignalを持つためのイベントを作成する
	HANDLE fenceEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	assert(fenceEvent != nullptr);

}

void DirectXCommon::InitializeFixFPS()
{

	// 現在時間を記録する
	reference_ = std::chrono::steady_clock::now();

}

void DirectXCommon::UpdateFixFPS()
{

	// 1/60秒ぴったりの時間
	const std::chrono::microseconds kMinTime(uint64_t(1000000.0f / 60.0f));
	// 1/60秒よりわずか
	const std::chrono::microseconds kMinCheckTime(uint64_t(1000000.0f / 65.0f));

	// 現在時間を取得する
	std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();
	// 前回記録からの経過時間を取得する
	std::chrono::microseconds elapsed =
		std::chrono::duration_cast<std::chrono::microseconds>(now - reference_);

	// 1/60秒(よりわずかに短い時間)経ってない場合
	if (elapsed < kMinCheckTime) {
		// 1/60秒経過するまで微小なスリーブを繰り返す
		while (std::chrono::steady_clock::now() - reference_ < kMinTime) {
			// 1マイクロ秒スリーブ
			std::this_thread::sleep_for(std::chrono::microseconds(1));
		}
	}
	// 現在の時間を記録する
	reference_ = std::chrono::steady_clock::now();

}

void DirectXCommon::PostEffectInitialize(ID3D12RootSignature* postRootSignature,
	ID3D12PipelineState* postPipelineState)
{

	postRootSignature_ = postRootSignature;
	postPipelineState_ = postPipelineState;

	//Sprite用の頂点リソースを作る
	vertBuff_ = BufferResource::CreateBufferResource(dxgiDevice_->GetDevice(), sizeof(SpriteVertex) * 6);

	//リソースの先頭のアドレスから使う
	vbView_.BufferLocation = vertBuff_->GetGPUVirtualAddress();
	//使用するリソースのサイズは頂点6つ分のサイズ
	vbView_.SizeInBytes = sizeof(SpriteVertex) * 6;
	//1頂点あたりのサイズ
	vbView_.StrideInBytes = sizeof(SpriteVertex);

	//書き込むためのアドレスを取得
	vertBuff_->Map(0, nullptr, reinterpret_cast<void**>(&vertMap));

	//インデックスリソースを作る
	indexBuff_ = BufferResource::CreateBufferResource(dxgiDevice_->GetDevice(), sizeof(uint32_t) * 6);

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

void DirectXCommon::PostEffect()
{

	//RootSignatureを設定。
	command_->GetCommadList()->SetPipelineState(postPipelineState_);//PS0を設定
	command_->GetCommadList()->SetGraphicsRootSignature(postRootSignature_);
	//形状を設定。PS0に設定しているものとは別。
	command_->GetCommadList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// 頂点バッファの設定
	command_->GetCommadList()->IASetVertexBuffers(0, 1, &vbView_);
	//IBVを設定
	command_->GetCommadList()->IASetIndexBuffer(&ibView_);

	//ID3D12DescriptorHeap* ppHeaps[] = { DescriptorHerpManager::descriptorHeap_.Get() };
	//commandList_->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);

	//シェーダーリソースビューをセット
	renderTargetTexture_->SetGraphicsRootDescriptorTable(command_->GetCommadList(), 0);

	//描画
	command_->GetCommadList()->DrawIndexedInstanced(6, 1, 0, 0, 0);

}
