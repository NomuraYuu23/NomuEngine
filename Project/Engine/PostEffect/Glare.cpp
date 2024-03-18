#include "Glare.h"
#include "../base/WinApp.h"
#include "../base/BufferResource.h"

void Glare::Initialize(const std::array<uint32_t, kImageForGlareIndexOfCount>& imageForGlareHandles)
{

	device_ = DirectXCommon::GetInstance()->GetDevice();

	renderTargetTexture_ = std::make_unique<RenderTargetTexture>();
	renderTargetTexture_->Initialize(
		device_,
		WinApp::kWindowWidth,
		WinApp::kWindowHeight);

	imageForGlareHandles_ = imageForGlareHandles;

	// 定数バッファ作成
	computeParametersBuff_ = BufferResource::CreateBufferResource(
		device_,
		(sizeof(ComputeParameters) + 0xff) & ~0xff);
	computeParametersBuff_->Map(0, nullptr, reinterpret_cast<void**>(&computeParametersMap_));

	computeParametersMap_->lamdaR = static_cast<float>(633e-9); // ラムダR
	computeParametersMap_->lamdaG = static_cast<float>(532e-9); // ラムダG
	computeParametersMap_->lamdaB = static_cast<float>(466e-9); // ラムダB
	computeParametersMap_->glareIntensity = 0.8f; // グレア強度
	computeParametersMap_->threshold = 0.8f; // しきい値

}

void Glare::Execution(
	const CD3DX12_GPU_DESCRIPTOR_HANDLE& imageWithGlareHandle,
	float glareIntensity,
	float threshold,
	ImageForGlareIndex imageForGlareIndex)
{
	
	// リスト
	ID3D12GraphicsCommandList* list = DirectXCommon::GetInstance()->GetCommadList();

	// 定数バッファを更新
	computeParametersMap_->glareIntensity = glareIntensity;
	computeParametersMap_->threshold = threshold;
	// 定数バッファを送る
	list->SetGraphicsRootConstantBufferView(0, computeParametersBuff_->GetGPUVirtualAddress());




}
