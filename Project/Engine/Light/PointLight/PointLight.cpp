#include "PointLight.h"
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
ID3D12Device* PointLight::sDevice = nullptr;
// コマンドリスト
ID3D12GraphicsCommandList* PointLight::sCommandList = nullptr;

void PointLight::StaticInitialize(ID3D12Device* device)
{

	assert(device);

	sDevice = device;

}

PointLight* PointLight::Create()
{

	PointLight* pointLight = new PointLight();
	assert(pointLight);

	//初期化
	pointLight->Initialize();

	return pointLight;

}

void PointLight::Initialize()
{

	//平行光源リソースを作る
	pointLightBuff_ = BufferResource::CreateBufferResource(sDevice, (sizeof(PointLightData) + 0xff) & ~0xff);

	//書き込むためのアドレスを取得
	pointLightBuff_->Map(0, nullptr, reinterpret_cast<void**>(&pointLightMap));

	//デフォルト値
	pointLightMap->color = { 1.0f,1.0f,1.0f,1.0f };
	pointLightMap->position = { 0.0f, -1.0f, 0.0f };
	pointLightMap->intencity = 1.0f;

}

void PointLight::Update(const PointLightData& directionalLightData)
{

	pointLightMap->color = directionalLightData.color;
	pointLightMap->position = directionalLightData.position;
	pointLightMap->intencity = directionalLightData.intencity;

}

void PointLight::Draw(ID3D12GraphicsCommandList* cmdList, uint32_t rootParameterIndex)
{

	assert(sCommandList == nullptr);

	sCommandList = cmdList;

	//光源
	sCommandList->SetGraphicsRootConstantBufferView(rootParameterIndex, pointLightBuff_->GetGPUVirtualAddress());

	// コマンドリストを解除
	sCommandList = nullptr;

}
