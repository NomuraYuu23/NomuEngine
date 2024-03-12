#include "DrawLine.h"
#include "../base/WinApp.h"
#include <cassert>
#include "../base/BufferResource.h"

using namespace DirectX;
using namespace Microsoft::WRL;

// デバイス
ID3D12Device* DrawLine::sDevice = nullptr;
// コマンドリスト
ID3D12GraphicsCommandList* DrawLine::sCommandList = nullptr;
// ルートシグネチャ
ID3D12RootSignature* DrawLine::sRootSignature;
// パイプラインステートオブジェクト
ID3D12PipelineState* DrawLine::sPipelineState;

void DrawLine::StaticInitialize(ID3D12Device* device, ID3D12RootSignature* rootSignature, ID3D12PipelineState* pipelineState)
{

	assert(device);

	sDevice = device;

	sRootSignature = rootSignature;
	sPipelineState = pipelineState;

}

void DrawLine::PreDraw(ID3D12GraphicsCommandList* cmdList)
{

	assert(DrawLine::sCommandList == nullptr);

	sCommandList = cmdList;

	//RootSignatureを設定。
	sCommandList->SetPipelineState(sPipelineState);//PS0を設定
	sCommandList->SetGraphicsRootSignature(sRootSignature);
	//形状を設定。PS0に設定しているものとは別。
	sCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINELIST);

}

void DrawLine::PostDraw()
{
	//コマンドリストを解除
	DrawLine::sCommandList = nullptr;

}

DrawLine* DrawLine::Create()
{
	
	// Spriteのインスタンスを生成
	DrawLine* drawLine = new DrawLine();
	if (drawLine == nullptr) {
		return nullptr;
	}

	// 初期化
	if (!drawLine->Initialize()) {
		delete drawLine;
		assert(0);
		return nullptr;
	}

	return drawLine;

}

DrawLine::DrawLine()
{

}

bool DrawLine::Initialize()
{
	assert(sDevice);

	//Sprite用の頂点リソースを作る
	vertBuff_ = BufferResource::CreateBufferResource(sDevice, sizeof(ColorVertexData) * kVertNum);

	//リソースの先頭のアドレスから使う
	vbView_.BufferLocation = vertBuff_->GetGPUVirtualAddress();
	//使用するリソースのサイズは頂点6つ分のサイズ
	vbView_.SizeInBytes = sizeof(ColorVertexData) * kVertNum;
	//1頂点あたりのサイズ
	vbView_.StrideInBytes = sizeof(ColorVertexData);

	//書き込むためのアドレスを取得
	vertBuff_->Map(0, nullptr, reinterpret_cast<void**>(&vertMap));

	vertMap[0].color = { 1.0f, 1.0f, 1.0f, 1.0f };
	vertMap[0].positon = { 0.0f, 0.0f, 0.0f, 1.0f};
	vertMap[1].color = { 1.0f, 1.0f, 1.0f, 1.0f };
	vertMap[1].positon = { 0.0f, 0.0f, 0.0f, 1.0f };

	return true;

}

void DrawLine::Draw(
	const Vector3& position0,
	const Vector3& position1,
	const Vector4& color0,
	const Vector4& color1,
	BaseCamera& camera)
{

	vertMap[0].color = color0;
	vertMap[1].color = color1;
	vertMap[0].positon = { position0.x, position0.y, position0.z, 1.0f };
	vertMap[1].positon = { position1.x, position1.y, position1.z, 1.0f };
	
	// 頂点バッファの設定
	sCommandList->IASetVertexBuffers(0, 1, &vbView_);

	//VP CBufferの場所を設定
	sCommandList->SetGraphicsRootConstantBufferView(0, camera.GetViewProjectionMatriBuff()->GetGPUVirtualAddress());

	//描画
	sCommandList->DrawInstanced(kVertNum, 1, 0, 0);


}
