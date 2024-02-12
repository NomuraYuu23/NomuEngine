#include "Material.h"
#include <cassert>

#include <fstream>
#include <sstream>
#include <format>
#include "../base/BufferResource.h"

using namespace DirectX;
using namespace Microsoft::WRL;

// デバイス
ID3D12Device* Material::sDevice = nullptr;

/// <summary>
/// 静的初期化
/// </summary>
/// <param name="device">デバイス</param>
void Material::StaticInitialize(ID3D12Device* device) {

	assert(device);

	sDevice = device;

}

/// <summary>
/// マテリアル生成
/// </summary>
/// <returns></returns>
Material* Material::Create() {

	// 3Dオブジェクトのインスタンスを生成
	Material* material = new Material();
	assert(material);

	// 初期化
	material->Initialize();

	return material;

}

/// <summary>
/// 初期化
/// </summary>
void Material::Initialize() {

	uvTransform_ = {
	{1.0f,1.0f,1.0f},
	{0.0f,0.0f,0.0f},
	{0.0f,0.0f,0.0f},
	};
	color_ = { 1.0f, 1.0f, 1.0f, 1.0f };
	enableLighting_ = None;
	shininess_ = 100.0f;

	//Sprite用のマテリアルリソースを作る
	materialBuff_ = BufferResource::CreateBufferResource(sDevice, (sizeof(MaterialData) + 0xff) & ~0xff);
	//書き込むためのアドレスを取得
	materialBuff_->Map(0, nullptr, reinterpret_cast<void**>(&materialMap_));
	//白を書き込んでみる
	materialMap_->color = color_;
	//SpriteはLightingしないのでfalseを設定する
	materialMap_->enableLighting = enableLighting_;	//UVTransfome初期化
	materialMap_->uvTransform = Matrix4x4Calc::GetInstance()->MakeAffineMatrix(uvTransform_.scale,uvTransform_.rotate, uvTransform_.translate);
	materialMap_->shininess = shininess_;

}

/// <summary>
/// 更新
/// </summary>
void Material::Update(const TransformStructure& uvTransform, const Vector4& color, int enableLighting, float shininess) {

	uvTransform_ = uvTransform;
	color_ = color;
	enableLighting_ = enableLighting;
	shininess_ = shininess;

	//UVTransfome用
	materialMap_->uvTransform = Matrix4x4Calc::GetInstance()->MakeAffineMatrix(uvTransform_.scale, uvTransform_.rotate, uvTransform_.translate);

	materialMap_->color = color_;

	materialMap_->enableLighting = enableLighting_;

	materialMap_->shininess = shininess_;

}

void Material::SetUvTransform(const TransformStructure& uvTransform)
{
	
	uvTransform_ = uvTransform;
	materialMap_->uvTransform = Matrix4x4Calc::GetInstance()->MakeAffineMatrix(uvTransform_.scale, uvTransform_.rotate, uvTransform_.translate);

}

void Material::SetColor(const Vector4& color)
{

	color_ = color;
	materialMap_->color = color_;

}

void Material::SetEnableLighting(const int32_t enableLighting)
{

	enableLighting_ = enableLighting;
	materialMap_->enableLighting = enableLighting_;

}

void Material::SetShininess(const float shininess)
{

	shininess_ = shininess;
	materialMap_->shininess = shininess_;

}
