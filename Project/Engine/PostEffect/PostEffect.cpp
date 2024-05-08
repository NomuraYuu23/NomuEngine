#include "PostEffect.h"
#include "../base/BufferResource.h"
#include "../base/Log.h"
#include "../base/CompileShader.h"
#include <fstream>
#include <cmath>
#include "../2D/ImguiManager.h"

PostEffect* PostEffect::GetInstance()
{
	static PostEffect instance;
	return &instance;
}

void PostEffect::Initialize()
{

	// デバイス取得
	device_ = DirectXCommon::GetInstance()->GetDevice();

	commandList_ = nullptr;

	// 定数バッファ作成
	computeParametersBuff_ = BufferResource::CreateBufferResource(device_, ((sizeof(ComputeParameters) + 0xff) & ~0xff));
	computeParametersBuff_->Map(0, nullptr, reinterpret_cast<void**>(&computeParametersMap_));

	// 定数バッファに渡す値の設定
	computeParametersMap_->threadIdOffsetX = 0; // スレッドのオフセットX
	computeParametersMap_->threadIdTotalX = kTextureWidth; // スレッドの総数X
	computeParametersMap_->threadIdOffsetY = 0; // スレッドのオフセットY
	computeParametersMap_->threadIdTotalY = kTextureHeight; // スレッドの総数Y
	computeParametersMap_->threadIdOffsetZ = 0; // スレッドのオフセットZ
	computeParametersMap_->threadIdTotalZ = 1; // スレッドの総数Z

	computeParametersMap_->clearColor = { 0.0f, 1.0f, 0.0f, 1.0f }; // クリアするときの色
	computeParametersMap_->threshold = 0.8f; // しきい値

	computeParametersMap_->kernelSize = 7; // カーネルサイズ
	computeParametersMap_->sigma = 1.0f; // 標準偏差

	computeParametersMap_->time = 0.0f; // 時間

	computeParametersMap_->rShift = { 0.0f,0.0f }; // Rずらし
	computeParametersMap_->gShift = { 0.0f,0.0f }; // Gずらし
	computeParametersMap_->bShift = { 0.0f,0.0f }; // Bずらし

	computeParametersMap_->distortion = 0.0f; // 歪み

	computeParametersMap_->vignetteSize = 16.0f; // ビネットの大きさ
	computeParametersMap_->vignetteChange = 0.8f; // ビネットの変化

	computeParametersMap_->horzGlitchPase = 0.1f; //水平
	computeParametersMap_->vertGlitchPase = 0.1f; //垂直
	computeParametersMap_->glitchStepValue = 0.1f; // グリッチのステップ値

	computeParametersMap_->radialBlurSamples = 8; // ブラーのサンプル回数
	computeParametersMap_->radialBlurCenter = { 0.5f,0.5f }; // 中心座標
	computeParametersMap_->radialBlurStrength = 0.0f; // ブラーの広がる強さ
	computeParametersMap_->radialBlurMask = 0.0f; // 放射状ブラーが適用されないサイズ

	computeParametersMap_->flareColor = { 0.7f, 0.7f, 0.7f, 0.7f }; // フレアの色
	computeParametersMap_->flareSize = { 0.3f, 0.3f }; // フレアの大きさ
	computeParametersMap_->flarePosition = { 0.0f, 0.0f }; // フレアの位置

	computeParametersMap_->paraColor = { 0.2f, 0.2f, 0.2f, 0.7f };  // パラの色
	computeParametersMap_->paraSize = { 0.3f, 0.3f };// パラの大きさ
	computeParametersMap_->paraPosition = { 1280.0f, 720.0f }; // パラの位置

	computeParametersMap_->magnificationER = 2.0f; // 拡大縮小倍率

	// ルートシグネチャ
	CreateRootSignature();

	//シェーダー
	CompileShader();

	// パイプライン
	CreatePipline();

	// 編集する画像初期化
	for (uint32_t i = 0; i < kNumEditTexture; ++i) {
		editTextures_[i] = std::make_unique<TextureUAV>();
		editTextures_[i]->Initialize(
			device_,
			kTextureWidth,
			kTextureHeight);
	}
	for (uint32_t i = 0; i < 3; ++i) {
		internalEditTextures_[i] = std::make_unique<TextureUAV>();
		internalEditTextures_[i]->Initialize(
			device_,
			kTextureWidth,
			kTextureHeight);
	}

}

void PostEffect::ImGuiDraw()
{

	ImGui::Begin("PostEffect");
	ImGui::Text("time %6.2f", computeParametersMap_->time);
	ImGui::DragFloat("threshold", &computeParametersMap_->threshold, 0.01f, 0.0f, 1.0f);
	ImGui::DragInt("kernelSize", &computeParametersMap_->kernelSize, 2, 3, 55);
	ImGui::DragFloat("sigma", &computeParametersMap_->sigma, 0.01f, 0.0f);
	ImGui::DragFloat2("rShift", &computeParametersMap_->rShift.x, 0.01f);
	ImGui::DragFloat2("gShift", &computeParametersMap_->gShift.x, 0.01f);
	ImGui::DragFloat2("bShift", &computeParametersMap_->bShift.x, 0.01f);
	ImGui::DragFloat("distortion", &computeParametersMap_->distortion, 0.01f);
	ImGui::DragFloat("vignetteSize", &computeParametersMap_->vignetteSize, 0.01f);
	ImGui::DragFloat("vignetteChange", &computeParametersMap_->vignetteChange, 0.01f);
	ImGui::DragFloat("horzGlitchPase", &computeParametersMap_->horzGlitchPase, 0.01f);
	ImGui::DragFloat("vertGlitchPase", &computeParametersMap_->vertGlitchPase, 0.01f);
	ImGui::DragFloat("glitchStepValue", &computeParametersMap_->glitchStepValue, 0.01f);
	ImGui::DragInt("radialBlurSamples", &computeParametersMap_->radialBlurSamples, 2, 2, 32);
	ImGui::DragFloat2("radialBlurCenter", &computeParametersMap_->radialBlurCenter.x, 0.01f);
	ImGui::DragFloat("radialBlurStrength", &computeParametersMap_->radialBlurStrength, 0.01f);
	ImGui::DragFloat("radialBlurMask", &computeParametersMap_->radialBlurMask, 0.01f);
	ImGui::ColorEdit4("flareColor", &computeParametersMap_->flareColor.x);
	ImGui::DragFloat2("flareSize", &computeParametersMap_->flareSize.x, 0.01f, 0.0f);
	ImGui::DragFloat2("flarePosition", &computeParametersMap_->flarePosition.x, 0.01f);
	ImGui::ColorEdit4("paraColor", &computeParametersMap_->paraColor.x);
	ImGui::DragFloat2("paraSize", &computeParametersMap_->paraSize.x, 0.01f, 0.0f);
	ImGui::DragFloat2("paraPosition", &computeParametersMap_->paraPosition.x, 0.01f);
	ImGui::DragFloat("magnificationER", &computeParametersMap_->magnificationER, 0.01f, 1.0f);
	ImGui::End();

}

void PostEffect::CopyCommand(
	ID3D12GraphicsCommandList* commandList, 
	uint32_t editTextureIndex,
	const CD3DX12_GPU_DESCRIPTOR_HANDLE& copyGPUHandle)
{

	// インデックスが超えているとエラー
	assert(editTextureIndex < kNumEditTexture);

	// コマンドリスト
	commandList_ = commandList;

	// コマンドリストがヌルならエラー
	assert(commandList_);

	// ルートシグネチャ
	commandList_->SetComputeRootSignature(rootSignature_.Get());
	// パイプライン
	commandList_->SetPipelineState(pipelineStates_[kPipelineIndexCopyCS].Get());

	// ディスパッチ数
	uint32_t x = (kTextureWidth + kNumThreadX - 1) / kNumThreadX;
	uint32_t y = (kTextureHeight + kNumThreadY - 1) / kNumThreadY;
	uint32_t z = 1;

	// バッファを送る
	// 定数パラメータ
	commandList_->SetComputeRootConstantBufferView(0, computeParametersBuff_->GetGPUVirtualAddress());
	// コピーする画像
	commandList_->SetComputeRootDescriptorTable(1, copyGPUHandle);
	// 編集する画像セット
	editTextures_[editTextureIndex]->SetRootDescriptorTable(commandList_, 3);

	// 実行
	commandList_->Dispatch(x, y, z);

	// コマンドリスト
	commandList_ = nullptr;

}

void PostEffect::ClearCommand(
	ID3D12GraphicsCommandList* commandList,
	uint32_t editTextureIndex)
{

	// インデックスが超えているとエラー
	assert(editTextureIndex < kNumEditTexture);

	// コマンドリスト
	commandList_ = commandList;

	// コマンドリストがヌルならエラー
	assert(commandList_);

	// ルートシグネチャ
	commandList_->SetComputeRootSignature(rootSignature_.Get());
	// パイプライン
	commandList_->SetPipelineState(pipelineStates_[kPipelineIndexClesrCS].Get());

	// ディスパッチ数
	uint32_t x = (kTextureWidth + kNumThreadX - 1) / kNumThreadX;
	uint32_t y = (kTextureHeight + kNumThreadY - 1) / kNumThreadY;
	uint32_t z = 1;

	// バッファを送る
	// 定数パラメータ
	commandList_->SetComputeRootConstantBufferView(0, computeParametersBuff_->GetGPUVirtualAddress());
	// 編集する画像セット
	editTextures_[editTextureIndex]->SetRootDescriptorTable(commandList_, 3);

	// 実行
	commandList_->Dispatch(x, y, z);

	// コマンドリスト
	commandList_ = nullptr;

}

void PostEffect::BinaryThresholdCommand(
	ID3D12GraphicsCommandList* commandList,
	uint32_t editTextureIndex,
	const CD3DX12_GPU_DESCRIPTOR_HANDLE& binaryThresholdGPUHandle)
{

	// インデックスが超えているとエラー
	assert(editTextureIndex < kNumEditTexture);

	// コマンドリスト
	commandList_ = commandList;

	// コマンドリストがヌルならエラー
	assert(commandList_);

	// ルートシグネチャ
	commandList_->SetComputeRootSignature(rootSignature_.Get());
	// パイプライン
	commandList_->SetPipelineState(pipelineStates_[kPipelineIndexBinaryThresholdCS].Get());

	// ディスパッチ数
	uint32_t x = (kTextureWidth + kNumThreadX - 1) / kNumThreadX;
	uint32_t y = (kTextureHeight + kNumThreadY - 1) / kNumThreadY;
	uint32_t z = 1;

	// バッファを送る

	// 定数パラメータ
	commandList_->SetComputeRootConstantBufferView(0, computeParametersBuff_->GetGPUVirtualAddress());
	// 二値化する画像をセット
	commandList_->SetComputeRootDescriptorTable(1, binaryThresholdGPUHandle);
	// 編集する画像セット
	editTextures_[editTextureIndex]->SetRootDescriptorTable(commandList_, 3);

	// 実行
	commandList_->Dispatch(x, y, z);

	// コマンドリスト
	commandList_ = nullptr;

}

void PostEffect::GaussianBlurCommand(
	ID3D12GraphicsCommandList* commandList,
	uint32_t editTextureIndex,
	const CD3DX12_GPU_DESCRIPTOR_HANDLE& gaussianBluGPUHandle)
{

	// インデックスが超えているとエラー
	assert(editTextureIndex < kNumEditTexture);

	// コマンドリスト
	commandList_ = commandList;

	// コマンドリストがヌルならエラー
	assert(commandList_);

	// ルートシグネチャ
	commandList_->SetComputeRootSignature(rootSignature_.Get());
	
	// ディスパッチ数
	uint32_t x = (kTextureWidth + kNumThreadX - 1) / kNumThreadX;
	uint32_t y = (kTextureHeight + kNumThreadY - 1) / kNumThreadY;
	uint32_t z = 1;

	// パイプライン
	commandList_->SetPipelineState(pipelineStates_[kPipelineIndexGaussianBlurHorizontal].Get());
	// バッファを送る
	// 定数パラメータ
	commandList_->SetComputeRootConstantBufferView(0, computeParametersBuff_->GetGPUVirtualAddress());
	// ガウスブラーを掛ける画像をセット
	commandList_->SetComputeRootDescriptorTable(1, gaussianBluGPUHandle);
	// 編集する画像セット
	internalEditTextures_[0]->SetRootDescriptorTable(commandList_, 3);

	// 実行
	commandList_->Dispatch(x, y, z);

	// パイプライン
	commandList_->SetPipelineState(pipelineStates_[kPipelineIndexGaussianBlurVertical].Get());
	// バッファを送る
	// ガウスブラーを掛ける画像をセット
	internalEditTextures_[0]->SetRootDescriptorTable(commandList_, 1);
	// 編集する画像セット
	editTextures_[editTextureIndex]->SetRootDescriptorTable(commandList_, 3);

	// 実行
	commandList_->Dispatch(x, y, z);

	// コマンドリスト
	commandList_ = nullptr;

}

void PostEffect::BloomCommand(
	ID3D12GraphicsCommandList* commandList, 
	uint32_t editTextureIndex, 
	const CD3DX12_GPU_DESCRIPTOR_HANDLE& bloomGPUHandle)
{

	// インデックスが超えているとエラー
	assert(editTextureIndex < kNumEditTexture);

	// コマンドリスト
	commandList_ = commandList;

	// コマンドリストがヌルならエラー
	assert(commandList_);

	// ルートシグネチャ
	commandList_->SetComputeRootSignature(rootSignature_.Get());

	// ディスパッチ数
	uint32_t x = (kTextureWidth + kNumThreadX - 1) / kNumThreadX;
	uint32_t y = (kTextureHeight + kNumThreadY - 1) / kNumThreadY;
	uint32_t z = 1;

	// パイプライン
	commandList_->SetPipelineState(pipelineStates_[kPipelineIndexBrightnessThreshold].Get());

	// バッファを送る

	// 定数パラメータ
	commandList_->SetComputeRootConstantBufferView(0, computeParametersBuff_->GetGPUVirtualAddress());
	// 明度分けする画像をセット
	commandList_->SetComputeRootDescriptorTable(1, bloomGPUHandle);
	// 編集する画像セット
	internalEditTextures_[0]->SetRootDescriptorTable(commandList_, 3);

	// 実行
	commandList_->Dispatch(x, y, z);

	// パイプライン
	commandList_->SetPipelineState(pipelineStates_[kPipelineIndexGaussianBlurHorizontal].Get());
	// バッファを送る
	// ガウスブラーを掛ける画像をセット
	internalEditTextures_[0]->SetRootDescriptorTable(commandList_, 1);
	// 編集する画像セット
	internalEditTextures_[1]->SetRootDescriptorTable(commandList_, 3);

	// 実行
	commandList_->Dispatch(x, y, z);

	// パイプライン
	commandList_->SetPipelineState(pipelineStates_[kPipelineIndexGaussianBlurVertical].Get());
	// バッファを送る
	// ガウスブラーを掛ける画像をセット
	internalEditTextures_[1]->SetRootDescriptorTable(commandList_, 1);
	// 編集する画像セット
	internalEditTextures_[2]->SetRootDescriptorTable(commandList_, 3);

	// 実行
	commandList_->Dispatch(x, y, z);

	// パイプライン
	commandList_->SetPipelineState(pipelineStates_[kPipelineIndexBlurAdd].Get());
	// バッファを送る
	// 加算する画像をセット
	commandList_->SetComputeRootDescriptorTable(1, bloomGPUHandle);
	// 加算する画像をセット
	internalEditTextures_[2]->SetRootDescriptorTable(commandList_, 2);
	// 編集する画像セット
	editTextures_[editTextureIndex]->SetRootDescriptorTable(commandList_, 3);

	// 実行
	commandList_->Dispatch(x, y, z);
	
	// コマンドリスト
	commandList_ = nullptr;

}

void PostEffect::OverwriteCommand(
	ID3D12GraphicsCommandList* commandList, 
	uint32_t editTextureIndex, 
	const CD3DX12_GPU_DESCRIPTOR_HANDLE& addGPUHandle0, 
	const CD3DX12_GPU_DESCRIPTOR_HANDLE& addGPUHandle1)
{

	// インデックスが超えているとエラー
	assert(editTextureIndex < kNumEditTexture);

	// コマンドリスト
	commandList_ = commandList;

	// コマンドリストがヌルならエラー
	assert(commandList_);

	// ルートシグネチャ
	commandList_->SetComputeRootSignature(rootSignature_.Get());

	// ディスパッチ数
	uint32_t x = (kTextureWidth + kNumThreadX - 1) / kNumThreadX;
	uint32_t y = (kTextureHeight + kNumThreadY - 1) / kNumThreadY;
	uint32_t z = 1;

	// パイプライン
	commandList_->SetPipelineState(pipelineStates_[kPipelineIndexOverwrite].Get());
	// バッファを送る
	// 定数パラメータ
	commandList_->SetComputeRootConstantBufferView(0, computeParametersBuff_->GetGPUVirtualAddress());
	// 加算する画像をセット
	commandList_->SetComputeRootDescriptorTable(1, addGPUHandle0);
	// 加算する画像をセット
	commandList_->SetComputeRootDescriptorTable(2, addGPUHandle1);
	// 編集する画像セット
	editTextures_[editTextureIndex]->SetRootDescriptorTable(commandList_, 3);

	// 実行
	commandList_->Dispatch(x, y, z);

	// コマンドリスト
	commandList_ = nullptr;

}

void PostEffect::RTTCorrectionCommand(
	ID3D12GraphicsCommandList* commandList, 
	uint32_t editTextureIndex, 
	const CD3DX12_GPU_DESCRIPTOR_HANDLE& textureGPUHandle)
{

	// インデックスが超えているとエラー
	assert(editTextureIndex < kNumEditTexture);

	// コマンドリスト
	commandList_ = commandList;

	// コマンドリストがヌルならエラー
	assert(commandList_);

	// ルートシグネチャ
	commandList_->SetComputeRootSignature(rootSignature_.Get());

	// ディスパッチ数
	uint32_t x = (kTextureWidth + kNumThreadX - 1) / kNumThreadX;
	uint32_t y = (kTextureHeight + kNumThreadY - 1) / kNumThreadY;
	uint32_t z = 1;

	// パイプライン
	commandList_->SetPipelineState(pipelineStates_[kPipelineIndexRTTCorrection].Get());
	// バッファを送る
	// 定数パラメータ
	commandList_->SetComputeRootConstantBufferView(0, computeParametersBuff_->GetGPUVirtualAddress());
	// 修正する画像をセット
	commandList_->SetComputeRootDescriptorTable(1, textureGPUHandle);
	// 編集する画像セット
	editTextures_[editTextureIndex]->SetRootDescriptorTable(commandList_, 3);

	// 実行
	commandList_->Dispatch(x, y, z);

	// コマンドリスト
	commandList_ = nullptr;

}

void PostEffect::MotionBlurCommand(
	ID3D12GraphicsCommandList* commandList, 
	uint32_t editTextureIndex, 
	const CD3DX12_GPU_DESCRIPTOR_HANDLE& motionBlurGPUHandle, 
	ID3D12Resource* velocityBuff)
{

	// インデックスが超えているとエラー
	assert(editTextureIndex < kNumEditTexture);

	// コマンドリスト
	commandList_ = commandList;

	// コマンドリストがヌルならエラー
	assert(commandList_);

	// ルートシグネチャ
	commandList_->SetComputeRootSignature(rootSignature_.Get());
	
	// ディスパッチ数
	uint32_t x = (kTextureWidth + kNumThreadX - 1) / kNumThreadX;
	uint32_t y = (kTextureHeight + kNumThreadY - 1) / kNumThreadY;
	uint32_t z = 1;

	// パイプライン
	commandList_->SetPipelineState(pipelineStates_[kPipelineIndexMotionBlur].Get());
	// バッファを送る
	// 定数パラメータ
	commandList_->SetComputeRootConstantBufferView(0, computeParametersBuff_->GetGPUVirtualAddress());
	// 修正する画像をセット
	commandList_->SetComputeRootDescriptorTable(1, motionBlurGPUHandle);
	// 編集する画像セット
	internalEditTextures_[0]->SetRootDescriptorTable(commandList_, 3);
	// 速度パラメータ
	commandList_->SetComputeRootConstantBufferView(4, velocityBuff->GetGPUVirtualAddress());

	// 実行
	commandList_->Dispatch(x, y, z);

	// パイプライン
	commandList_->SetPipelineState(pipelineStates_[kPipelineIndexBlurAdd].Get());
	// バッファを送る
	// 定数パラメータ
	commandList_->SetComputeRootConstantBufferView(0, computeParametersBuff_->GetGPUVirtualAddress());
	// 加算する画像をセット
	commandList_->SetComputeRootDescriptorTable(1, motionBlurGPUHandle);
	// 加算する画像をセット
	internalEditTextures_[0]->SetRootDescriptorTable(commandList_, 2);
	// 編集する画像セット
	editTextures_[editTextureIndex]->SetRootDescriptorTable(commandList_, 3);

	// 実行
	commandList_->Dispatch(x, y, z);

	// コマンドリスト
	commandList_ = nullptr;

}

void PostEffect::WhiteNoizeCommand(
	ID3D12GraphicsCommandList* commandList, 
	uint32_t editTextureIndex, 
	const CD3DX12_GPU_DESCRIPTOR_HANDLE& whiteNoizeGPUHandle)
{

	// インデックスが超えているとエラー
	assert(editTextureIndex < kNumEditTexture);

	// コマンドリスト
	commandList_ = commandList;

	// コマンドリストがヌルならエラー
	assert(commandList_);

	// ルートシグネチャ
	commandList_->SetComputeRootSignature(rootSignature_.Get());

	// ディスパッチ数
	uint32_t x = (kTextureWidth + kNumThreadX - 1) / kNumThreadX;
	uint32_t y = (kTextureHeight + kNumThreadY - 1) / kNumThreadY;
	uint32_t z = 1;

	// パイプライン
	commandList_->SetPipelineState(pipelineStates_[kPipelineIndexWhiteNoise].Get());
	// バッファを送る
	// 定数パラメータ
	commandList_->SetComputeRootConstantBufferView(0, computeParametersBuff_->GetGPUVirtualAddress());
	// ノイズを掛ける画像をセット
	commandList_->SetComputeRootDescriptorTable(1, whiteNoizeGPUHandle);
	// 編集する画像セット
	editTextures_[editTextureIndex]->SetRootDescriptorTable(commandList_, 3);

	// 実行
	commandList_->Dispatch(x, y, z);

	// コマンドリスト
	commandList_ = nullptr;

}

void PostEffect::ScanLineCommand(
	ID3D12GraphicsCommandList* commandList, 
	uint32_t editTextureIndex, 
	const CD3DX12_GPU_DESCRIPTOR_HANDLE& scanLineGPUHandle)
{

	// インデックスが超えているとエラー
	assert(editTextureIndex < kNumEditTexture);

	// コマンドリスト
	commandList_ = commandList;

	// コマンドリストがヌルならエラー
	assert(commandList_);

	// ルートシグネチャ
	commandList_->SetComputeRootSignature(rootSignature_.Get());

	// ディスパッチ数
	uint32_t x = (kTextureWidth + kNumThreadX - 1) / kNumThreadX;
	uint32_t y = (kTextureHeight + kNumThreadY - 1) / kNumThreadY;
	uint32_t z = 1;

	// パイプライン
	commandList_->SetPipelineState(pipelineStates_[kPipelineIndexScanLine].Get());
	// バッファを送る
	// 定数パラメータ
	commandList_->SetComputeRootConstantBufferView(0, computeParametersBuff_->GetGPUVirtualAddress());
	// 走査線を掛ける画像をセット
	commandList_->SetComputeRootDescriptorTable(1, scanLineGPUHandle);
	// 編集する画像セット
	editTextures_[editTextureIndex]->SetRootDescriptorTable(commandList_, 3);

	// 実行
	commandList_->Dispatch(x, y, z);

	// コマンドリスト
	commandList_ = nullptr;

}

void PostEffect::RGBShiftCommand(
	ID3D12GraphicsCommandList* commandList, 
	uint32_t editTextureIndex, 
	const CD3DX12_GPU_DESCRIPTOR_HANDLE& rgbShiftGPUHandle)
{

	// インデックスが超えているとエラー
	assert(editTextureIndex < kNumEditTexture);

	// コマンドリスト
	commandList_ = commandList;

	// コマンドリストがヌルならエラー
	assert(commandList_);

	// ルートシグネチャ
	commandList_->SetComputeRootSignature(rootSignature_.Get());

	// ディスパッチ数
	uint32_t x = (kTextureWidth + kNumThreadX - 1) / kNumThreadX;
	uint32_t y = (kTextureHeight + kNumThreadY - 1) / kNumThreadY;
	uint32_t z = 1;

	// パイプライン
	commandList_->SetPipelineState(pipelineStates_[kPipelineIndexRGBShift].Get());
	// バッファを送る
	// 定数パラメータ
	commandList_->SetComputeRootConstantBufferView(0, computeParametersBuff_->GetGPUVirtualAddress());
	// RGBずらしを掛ける画像をセット
	commandList_->SetComputeRootDescriptorTable(1, rgbShiftGPUHandle);
	// 編集する画像セット
	editTextures_[editTextureIndex]->SetRootDescriptorTable(commandList_, 3);

	// 実行
	commandList_->Dispatch(x, y, z);

	// コマンドリスト
	commandList_ = nullptr;

}

void PostEffect::BarrelCurvedCommand(
	ID3D12GraphicsCommandList* commandList, 
	uint32_t editTextureIndex, 
	const CD3DX12_GPU_DESCRIPTOR_HANDLE& barrelCurvedGPUHandle)
{

	// インデックスが超えているとエラー
	assert(editTextureIndex < kNumEditTexture);

	// コマンドリスト
	commandList_ = commandList;

	// コマンドリストがヌルならエラー
	assert(commandList_);

	// ルートシグネチャ
	commandList_->SetComputeRootSignature(rootSignature_.Get());

	// ディスパッチ数
	uint32_t x = (kTextureWidth + kNumThreadX - 1) / kNumThreadX;
	uint32_t y = (kTextureHeight + kNumThreadY - 1) / kNumThreadY;
	uint32_t z = 1;

	// パイプライン
	commandList_->SetPipelineState(pipelineStates_[kPipelineIndexBarrelCurved].Get());
	// バッファを送る
	// 定数パラメータ
	commandList_->SetComputeRootConstantBufferView(0, computeParametersBuff_->GetGPUVirtualAddress());
	// 樽状湾曲を掛ける画像をセット
	commandList_->SetComputeRootDescriptorTable(1, barrelCurvedGPUHandle);
	// 編集する画像セット
	editTextures_[editTextureIndex]->SetRootDescriptorTable(commandList_, 3);

	// 実行
	commandList_->Dispatch(x, y, z);

	// コマンドリスト
	commandList_ = nullptr;

}

void PostEffect::VignetteCommand(
	ID3D12GraphicsCommandList* commandList, 
	uint32_t editTextureIndex, 
	const CD3DX12_GPU_DESCRIPTOR_HANDLE& vignetteGPUHandle)
{

	// インデックスが超えているとエラー
	assert(editTextureIndex < kNumEditTexture);

	// コマンドリスト
	commandList_ = commandList;

	// コマンドリストがヌルならエラー
	assert(commandList_);

	// ルートシグネチャ
	commandList_->SetComputeRootSignature(rootSignature_.Get());

	// ディスパッチ数
	uint32_t x = (kTextureWidth + kNumThreadX - 1) / kNumThreadX;
	uint32_t y = (kTextureHeight + kNumThreadY - 1) / kNumThreadY;
	uint32_t z = 1;

	// パイプライン
	commandList_->SetPipelineState(pipelineStates_[kPipelineIndexVignette].Get());
	// バッファを送る
	// 定数パラメータ
	commandList_->SetComputeRootConstantBufferView(0, computeParametersBuff_->GetGPUVirtualAddress());
	// ビネットを掛ける画像をセット
	commandList_->SetComputeRootDescriptorTable(1, vignetteGPUHandle);
	// 編集する画像セット
	editTextures_[editTextureIndex]->SetRootDescriptorTable(commandList_, 3);

	// 実行
	commandList_->Dispatch(x, y, z);

	// コマンドリスト
	commandList_ = nullptr;

}

void PostEffect::GlitchCommand(
	ID3D12GraphicsCommandList* commandList, 
	uint32_t editTextureIndex, 
	const CD3DX12_GPU_DESCRIPTOR_HANDLE& glitchGPUHandle)
{

	// インデックスが超えているとエラー
	assert(editTextureIndex < kNumEditTexture);

	// コマンドリスト
	commandList_ = commandList;

	// コマンドリストがヌルならエラー
	assert(commandList_);

	// ルートシグネチャ
	commandList_->SetComputeRootSignature(rootSignature_.Get());

	// ディスパッチ数
	uint32_t x = (kTextureWidth + kNumThreadX - 1) / kNumThreadX;
	uint32_t y = (kTextureHeight + kNumThreadY - 1) / kNumThreadY;
	uint32_t z = 1;

	// パイプライン
	commandList_->SetPipelineState(pipelineStates_[kPipelineIndexGlitch].Get());
	// バッファを送る
	// 定数パラメータ
	commandList_->SetComputeRootConstantBufferView(0, computeParametersBuff_->GetGPUVirtualAddress());
	// グリッチを掛ける画像をセット
	commandList_->SetComputeRootDescriptorTable(1, glitchGPUHandle);
	// 編集する画像セット
	editTextures_[editTextureIndex]->SetRootDescriptorTable(commandList_, 3);

	// 実行
	commandList_->Dispatch(x, y, z);

	// コマンドリスト
	commandList_ = nullptr;

}

void PostEffect::RadialBlurCommand(
	ID3D12GraphicsCommandList* commandList, 
	uint32_t editTextureIndex, 
	const CD3DX12_GPU_DESCRIPTOR_HANDLE& radialBlurGPUHandle)
{

	// インデックスが超えているとエラー
	assert(editTextureIndex < kNumEditTexture);

	// コマンドリスト
	commandList_ = commandList;

	// コマンドリストがヌルならエラー
	assert(commandList_);

	// ルートシグネチャ
	commandList_->SetComputeRootSignature(rootSignature_.Get());

	// ディスパッチ数
	uint32_t x = (kTextureWidth + kNumThreadX - 1) / kNumThreadX;
	uint32_t y = (kTextureHeight + kNumThreadY - 1) / kNumThreadY;
	uint32_t z = 1;

	// パイプライン
	commandList_->SetPipelineState(pipelineStates_[kPipelineIndexRadialBlur].Get());
	// バッファを送る
	// 定数パラメータ
	commandList_->SetComputeRootConstantBufferView(0, computeParametersBuff_->GetGPUVirtualAddress());
	// 放射状ブラーを掛ける画像をセット
	commandList_->SetComputeRootDescriptorTable(1, radialBlurGPUHandle);
	// 編集する画像セット
	editTextures_[editTextureIndex]->SetRootDescriptorTable(commandList_, 3);

	// 実行
	commandList_->Dispatch(x, y, z);

	// コマンドリスト
	commandList_ = nullptr;

}

void PostEffect::ShockWaveCommand(
	ID3D12GraphicsCommandList* commandList, 
	uint32_t editTextureIndex, 
	const CD3DX12_GPU_DESCRIPTOR_HANDLE& shockWaveGPUHandle,
	ID3D12Resource* shockWaveBuff)
{

	// インデックスが超えているとエラー
	assert(editTextureIndex < kNumEditTexture);

	// コマンドリスト
	commandList_ = commandList;

	// コマンドリストがヌルならエラー
	assert(commandList_);

	// ルートシグネチャ
	commandList_->SetComputeRootSignature(rootSignature_.Get());

	// ディスパッチ数
	uint32_t x = (kTextureWidth + kNumThreadX - 1) / kNumThreadX;
	uint32_t y = (kTextureHeight + kNumThreadY - 1) / kNumThreadY;
	uint32_t z = 1;

	// パイプライン
	commandList_->SetPipelineState(pipelineStates_[kPipelineIndexShockWave].Get());
	// バッファを送る
	// 定数パラメータ
	commandList_->SetComputeRootConstantBufferView(0, computeParametersBuff_->GetGPUVirtualAddress());
	// 衝撃波を掛ける画像をセット
	commandList_->SetComputeRootDescriptorTable(1, shockWaveGPUHandle);
	// 編集する画像セット
	editTextures_[editTextureIndex]->SetRootDescriptorTable(commandList_, 3);
	// 衝撃波パラメータ
	commandList_->SetComputeRootConstantBufferView(5, shockWaveBuff->GetGPUVirtualAddress());

	// 実行
	commandList_->Dispatch(x, y, z);

	// コマンドリスト
	commandList_ = nullptr;

}

void PostEffect::FlareParaCommand(
	ID3D12GraphicsCommandList* commandList, 
	uint32_t editTextureIndex, 
	const CD3DX12_GPU_DESCRIPTOR_HANDLE& flareParaGPUHandle)
{

	// インデックスが超えているとエラー
	assert(editTextureIndex < kNumEditTexture);

	// コマンドリスト
	commandList_ = commandList;

	// コマンドリストがヌルならエラー
	assert(commandList_);

	// ルートシグネチャ
	commandList_->SetComputeRootSignature(rootSignature_.Get());

	// ディスパッチ数
	uint32_t x = (kTextureWidth + kNumThreadX - 1) / kNumThreadX;
	uint32_t y = (kTextureHeight + kNumThreadY - 1) / kNumThreadY;
	uint32_t z = 1;

	// パイプライン
	commandList_->SetPipelineState(pipelineStates_[kPipelineIndexFlarePara].Get());
	// バッファを送る
	// 定数パラメータ
	commandList_->SetComputeRootConstantBufferView(0, computeParametersBuff_->GetGPUVirtualAddress());
	// 衝撃波を掛ける画像をセット
	commandList_->SetComputeRootDescriptorTable(1, flareParaGPUHandle);
	// 編集する画像セット
	editTextures_[editTextureIndex]->SetRootDescriptorTable(commandList_, 3);

	// 実行
	commandList_->Dispatch(x, y, z);

	// コマンドリスト
	commandList_ = nullptr;

}

void PostEffect::ReductionCommand(
	ID3D12GraphicsCommandList* commandList, 
	uint32_t editTextureIndex, 
	const CD3DX12_GPU_DESCRIPTOR_HANDLE& reductionGPUHandle)
{

	// インデックスが超えているとエラー
	assert(editTextureIndex < kNumEditTexture);

	// コマンドリスト
	commandList_ = commandList;

	// コマンドリストがヌルならエラー
	assert(commandList_);

	// ルートシグネチャ
	commandList_->SetComputeRootSignature(rootSignature_.Get());

	// ディスパッチ数
	uint32_t x = (kTextureWidth + kNumThreadX - 1) / kNumThreadX;
	uint32_t y = (kTextureHeight + kNumThreadY - 1) / kNumThreadY;
	uint32_t z = 1;

	// パイプライン
	commandList_->SetPipelineState(pipelineStates_[kPipelineIndexReduction].Get());
	// バッファを送る
	// 定数パラメータ
	commandList_->SetComputeRootConstantBufferView(0, computeParametersBuff_->GetGPUVirtualAddress());
	// 衝撃波を掛ける画像をセット
	commandList_->SetComputeRootDescriptorTable(1, reductionGPUHandle);
	// 編集する画像セット
	editTextures_[editTextureIndex]->SetRootDescriptorTable(commandList_, 3);

	// 実行
	commandList_->Dispatch(x, y, z);

	// コマンドリスト
	commandList_ = nullptr;

}

void PostEffect::ExpansionCommand(
	ID3D12GraphicsCommandList* commandList, 
	uint32_t editTextureIndex, 
	const CD3DX12_GPU_DESCRIPTOR_HANDLE& expansionGPUHandle)
{

	// インデックスが超えているとエラー
	assert(editTextureIndex < kNumEditTexture);

	// コマンドリスト
	commandList_ = commandList;

	// コマンドリストがヌルならエラー
	assert(commandList_);

	// ルートシグネチャ
	commandList_->SetComputeRootSignature(rootSignature_.Get());

	// ディスパッチ数
	uint32_t x = (kTextureWidth + kNumThreadX - 1) / kNumThreadX;
	uint32_t y = (kTextureHeight + kNumThreadY - 1) / kNumThreadY;
	uint32_t z = 1;

	// パイプライン
	commandList_->SetPipelineState(pipelineStates_[kPipelineIndexExpansion].Get());
	// バッファを送る
	// 定数パラメータ
	commandList_->SetComputeRootConstantBufferView(0, computeParametersBuff_->GetGPUVirtualAddress());
	// 衝撃波を掛ける画像をセット
	commandList_->SetComputeRootDescriptorTable(1, expansionGPUHandle);
	// 編集する画像セット
	editTextures_[editTextureIndex]->SetRootDescriptorTable(commandList_, 3);

	// 実行
	commandList_->Dispatch(x, y, z);

	// コマンドリスト
	commandList_ = nullptr;

}

void PostEffect::GrayScaleCommand(
	ID3D12GraphicsCommandList* commandList, 
	uint32_t editTextureIndex, 
	const CD3DX12_GPU_DESCRIPTOR_HANDLE& grayScaleGPUHandle)
{

	// インデックスが超えているとエラー
	assert(editTextureIndex < kNumEditTexture);

	// コマンドリスト
	commandList_ = commandList;

	// コマンドリストがヌルならエラー
	assert(commandList_);

	// ルートシグネチャ
	commandList_->SetComputeRootSignature(rootSignature_.Get());

	// ディスパッチ数
	uint32_t x = (kTextureWidth + kNumThreadX - 1) / kNumThreadX;
	uint32_t y = (kTextureHeight + kNumThreadY - 1) / kNumThreadY;
	uint32_t z = 1;

	// パイプライン
	commandList_->SetPipelineState(pipelineStates_[kPipelineIndexGrayScale].Get());
	// バッファを送る
	// 定数パラメータ
	commandList_->SetComputeRootConstantBufferView(0, computeParametersBuff_->GetGPUVirtualAddress());
	// 衝撃波を掛ける画像をセット
	commandList_->SetComputeRootDescriptorTable(1, grayScaleGPUHandle);
	// 編集する画像セット
	editTextures_[editTextureIndex]->SetRootDescriptorTable(commandList_, 3);

	// 実行
	commandList_->Dispatch(x, y, z);

	// コマンドリスト
	commandList_ = nullptr;

}

void PostEffect::SepiaCommand(
	ID3D12GraphicsCommandList* commandList, 
	uint32_t editTextureIndex, 
	const CD3DX12_GPU_DESCRIPTOR_HANDLE& sepiaGPUHandle)
{

	// インデックスが超えているとエラー
	assert(editTextureIndex < kNumEditTexture);

	// コマンドリスト
	commandList_ = commandList;

	// コマンドリストがヌルならエラー
	assert(commandList_);

	// ルートシグネチャ
	commandList_->SetComputeRootSignature(rootSignature_.Get());

	// ディスパッチ数
	uint32_t x = (kTextureWidth + kNumThreadX - 1) / kNumThreadX;
	uint32_t y = (kTextureHeight + kNumThreadY - 1) / kNumThreadY;
	uint32_t z = 1;

	// パイプライン
	commandList_->SetPipelineState(pipelineStates_[kPipelineIndexSepia].Get());
	// バッファを送る
	// 定数パラメータ
	commandList_->SetComputeRootConstantBufferView(0, computeParametersBuff_->GetGPUVirtualAddress());
	// 衝撃波を掛ける画像をセット
	commandList_->SetComputeRootDescriptorTable(1, sepiaGPUHandle);
	// 編集する画像セット
	editTextures_[editTextureIndex]->SetRootDescriptorTable(commandList_, 3);

	// 実行
	commandList_->Dispatch(x, y, z);

	// コマンドリスト
	commandList_ = nullptr;

}

void PostEffect::GlitchRGBShiftCommand(
	ID3D12GraphicsCommandList* commandList, 
	uint32_t editTextureIndex, 
	const CD3DX12_GPU_DESCRIPTOR_HANDLE& glitchRGBShiftGPUHandle)
{

	// インデックスが超えているとエラー
	assert(editTextureIndex < kNumEditTexture);

	// コマンドリスト
	commandList_ = commandList;

	// コマンドリストがヌルならエラー
	assert(commandList_);

	// ルートシグネチャ
	commandList_->SetComputeRootSignature(rootSignature_.Get());

	// ディスパッチ数
	uint32_t x = (kTextureWidth + kNumThreadX - 1) / kNumThreadX;
	uint32_t y = (kTextureHeight + kNumThreadY - 1) / kNumThreadY;
	uint32_t z = 1;

	// パイプライン
	commandList_->SetPipelineState(pipelineStates_[kPipelineIndexGlitchRGBShift].Get());
	// バッファを送る
	// 定数パラメータ
	commandList_->SetComputeRootConstantBufferView(0, computeParametersBuff_->GetGPUVirtualAddress());
	// 衝撃波を掛ける画像をセット
	commandList_->SetComputeRootDescriptorTable(1, glitchRGBShiftGPUHandle);
	// 編集する画像セット
	editTextures_[editTextureIndex]->SetRootDescriptorTable(commandList_, 3);

	// 実行
	commandList_->Dispatch(x, y, z);

	// コマンドリスト
	commandList_ = nullptr;


}

void PostEffect::CreateRootSignature()
{

	HRESULT hr;

	D3D12_ROOT_SIGNATURE_DESC descriptionRootsignature{};
	descriptionRootsignature.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	// ディスクリプタレンジ

	// ソース0
	D3D12_DESCRIPTOR_RANGE descriptorRangeSouce0[1] = {};
	descriptorRangeSouce0[0].BaseShaderRegister = 0;//0から始まる
	descriptorRangeSouce0[0].NumDescriptors = 1;//数は一つ
	descriptorRangeSouce0[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;//SAVを使う
	descriptorRangeSouce0[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;//Offsetを自動計算

	// ソース1
	D3D12_DESCRIPTOR_RANGE descriptorRangeSouce1[1] = {};
	descriptorRangeSouce1[0].BaseShaderRegister = 1;//0から始まる
	descriptorRangeSouce1[0].NumDescriptors = 1;//数は一つ
	descriptorRangeSouce1[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;//SAVを使う
	descriptorRangeSouce1[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;//Offsetを自動計算

	// 編集するテクスチャ情報
	D3D12_DESCRIPTOR_RANGE descriptorRangeEditTextureInformation[1] = {};
	descriptorRangeEditTextureInformation[0].BaseShaderRegister = 0;//0から始まる
	descriptorRangeEditTextureInformation[0].NumDescriptors = 1;//数は一つ
	descriptorRangeEditTextureInformation[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_UAV;//UAVを使う
	descriptorRangeEditTextureInformation[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;//Offsetを自動計算

	// ルートパラメータ
	D3D12_ROOT_PARAMETER rootParameters[6] = {};
	// 定数バッファ
	rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;   //CBVを使う
	rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL; //ALLで使う
	rootParameters[0].Descriptor.ShaderRegister = 0;                  //レジスタ番号0とバインド

	// 元画像テクスチャ0
	rootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;//DescriptorTableを使う
	rootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;//全てで使う
	rootParameters[1].DescriptorTable.pDescriptorRanges = descriptorRangeSouce0;//Tableの中身の配列を指定
	rootParameters[1].DescriptorTable.NumDescriptorRanges = _countof(descriptorRangeSouce0);//Tableで利用する数

	// 元画像テクスチャ1
	rootParameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;//DescriptorTableを使う
	rootParameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;//全てで使う
	rootParameters[2].DescriptorTable.pDescriptorRanges = descriptorRangeSouce1;//Tableの中身の配列を指定
	rootParameters[2].DescriptorTable.NumDescriptorRanges = _countof(descriptorRangeSouce1);//Tableで利用する数

	// 行先画像テクスチャ
	rootParameters[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;//DescriptorTableを使う
	rootParameters[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;//全てで使う
	rootParameters[3].DescriptorTable.pDescriptorRanges = descriptorRangeEditTextureInformation;//Tableの中身の配列を指定
	rootParameters[3].DescriptorTable.NumDescriptorRanges = _countof(descriptorRangeEditTextureInformation);//Tableで利用する数

	// 速度バッファ
	rootParameters[4].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;   //CBVを使う
	rootParameters[4].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL; //ALLで使う
	rootParameters[4].Descriptor.ShaderRegister = 1;                  //レジスタ番号1とバインド

	// 衝撃波バッファ
	rootParameters[5].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;   //CBVを使う
	rootParameters[5].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL; //ALLで使う
	rootParameters[5].Descriptor.ShaderRegister = 2;                  //レジスタ番号1とバインド

	descriptionRootsignature.pParameters = rootParameters; //ルートパラメータ配列へのポインタ
	descriptionRootsignature.NumParameters = _countof(rootParameters); //配列の長さ

	// サンプラー
	D3D12_STATIC_SAMPLER_DESC samplerDesc[1] = {};
	samplerDesc[0].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	samplerDesc[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	samplerDesc[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	samplerDesc[0].MipLODBias = 0.0f;
	samplerDesc[0].MaxAnisotropy = 0;
	samplerDesc[0].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
	samplerDesc[0].BorderColor = D3D12_STATIC_BORDER_COLOR_OPAQUE_BLACK;
	samplerDesc[0].MinLOD = 0.0f;
	samplerDesc[0].MaxLOD = 3.402823466e+38f;
	samplerDesc[0].RegisterSpace = 0;
	samplerDesc[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	descriptionRootsignature.pStaticSamplers = samplerDesc;
	descriptionRootsignature.NumStaticSamplers = _countof(samplerDesc);

	//シリアライズしてバイナリにする
	ID3DBlob* signatureBlob = nullptr;
	ID3DBlob* errorBlob = nullptr;
	hr = D3D12SerializeRootSignature(&descriptionRootsignature,
		D3D_ROOT_SIGNATURE_VERSION_1, &signatureBlob, &errorBlob);
	if (FAILED(hr)) {
		Log::Message(reinterpret_cast<char*>(errorBlob->GetBufferPointer()));
		assert(false);
	}

	hr = device_->CreateRootSignature(0, signatureBlob->GetBufferPointer(),
		signatureBlob->GetBufferSize(), IID_PPV_ARGS(&rootSignature_));
	assert(SUCCEEDED(hr));


}

void PostEffect::CreateHeaderHLSL()
{
	
	// ファイルを開く
	std::ofstream file("Resources/shaders/PostEffect.hlsli");

	// ファイルがないのでエラー
	assert(file);

	// スレッド数
	file << "#define" << " " << "THREAD_X" << " " << kNumThreadX << "\n";
	file << "#define" << " " << "THREAD_Y" << " " << kNumThreadY << "\n";
	file << "#define" << " " << "THREAD_Z" << " " << kNumThreadZ << "\n";
	file << "#define" << " " << "PI" << " " << 3.14159265 << "\n";

	// ファイルを閉じる
	file.close();

}

void PostEffect::CompileShader()
{

	for (uint32_t i = 0; i < kPipelineIndexOfCount; ++i) {

		// ヘッダを作成
		CreateHeaderHLSL();

		// コンパイル
		shaders_[i] = CompileShader::Compile(
			shaderNames_[i].first,
			L"cs_6_0",
			shaderNames_[i].second);

	}

}

void PostEffect::CreatePipline()
{

	// パイプライン作成
	for (uint32_t i = 0; i < kPipelineIndexOfCount; ++i) {

		D3D12_COMPUTE_PIPELINE_STATE_DESC desc{};
		desc.CS.pShaderBytecode = shaders_[i]->GetBufferPointer();
		desc.CS.BytecodeLength = shaders_[i]->GetBufferSize();
		desc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;
		desc.NodeMask = 0;
		desc.pRootSignature = rootSignature_.Get();

		HRESULT hr = device_->CreateComputePipelineState(&desc, IID_PPV_ARGS(&pipelineStates_[i]));
		assert(SUCCEEDED(hr));
	}

}
