#include "SpotLight.h"
#include "../../base/TextureManager.h"
#include "../../base/WinApp.h"
#include <cassert>

#include <fstream>
#include <sstream>
#include <format>
#include "../../base/BufferResource.h"

using namespace DirectX;
using namespace Microsoft::WRL;

// デバイス
ID3D12Device* SpotLight::sDevice = nullptr;
// コマンドリスト
ID3D12GraphicsCommandList* SpotLight::sCommandList = nullptr;

void SpotLight::StaticInitialize(ID3D12Device* device)
{

	assert(device);

	sDevice = device;

}

SpotLight* SpotLight::Create()
{

	SpotLight* spotLight = new SpotLight();
	assert(spotLight);

	//初期化
	spotLight->Initialize();

	return spotLight;

}

void SpotLight::Initialize()
{

	//平行光源リソースを作る
	spotLightBuff_ = BufferResource::CreateBufferResource(sDevice, (sizeof(SpotLightData) + 0xff) & ~0xff);

	//書き込むためのアドレスを取得
	spotLightBuff_->Map(0, nullptr, reinterpret_cast<void**>(&spotLightMap));

	//デフォルト値
	spotLightMap->color = { 1.0f,1.0f,1.0f,1.0f };
	spotLightMap->position = { 0.0f, -1.0f, 0.0f };
	spotLightMap->intencity = 1.0f;
	spotLightMap->direction = { 0.0f, -1.0f, 0.0f};
	spotLightMap->distance = 10.0f;
	spotLightMap->decay = 10.0f;
	spotLightMap->cosAngle = 1.0f;
	spotLightMap->cosFalloffStart = 1.0f;

}

void SpotLight::Update(const SpotLightData& spotLightData)
{

	spotLightMap->color = spotLightData.color;
	spotLightMap->position = spotLightData.position;
	spotLightMap->direction = Vector3Calc::Normalize(spotLightData.direction);
	spotLightMap->distance = spotLightData.distance;
	spotLightMap->decay = spotLightData.decay;
	spotLightMap->cosAngle = spotLightData.cosAngle;
	spotLightMap->cosFalloffStart = spotLightData.cosFalloffStart;

}

void SpotLight::Draw(ID3D12GraphicsCommandList* cmdList, uint32_t rootParameterIndex)
{

	assert(sCommandList == nullptr);

	sCommandList = cmdList;

	//光源
	sCommandList->SetGraphicsRootConstantBufferView(rootParameterIndex, spotLightBuff_->GetGPUVirtualAddress());

	// コマンドリストを解除
	sCommandList = nullptr;

}
