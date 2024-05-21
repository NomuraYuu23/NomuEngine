#include "SkyBox.h"
#include "../base/DirectXCommon.h"
#include "../base/BufferResource.h"

const size_t SkyBox::kVertNum = 8;
const size_t SkyBox::kIndexNum = 36;

void SkyBox::Initialize(uint32_t textureHandle)
{

	ID3D12Device* device = DirectXCommon::GetInstance()->GetDevice();

	// 頂点バッファ
	vertBuff_ = BufferResource::CreateBufferResource(
		device, ((sizeof(SkyBox::VertexData) + 0xff) & ~0xff) * kVertNum);
	//リソースの先頭のアドレスから使う
	vbView_.BufferLocation = vertBuff_->GetGPUVirtualAddress();
	//使用するリソースのサイズ
	vbView_.SizeInBytes = UINT(sizeof(SkyBox::VertexData) * kVertNum);
	//1頂点あたりのサイズ
	vbView_.StrideInBytes = sizeof(SkyBox::VertexData);
	//書き込むためのアドレスを取得
	vertBuff_->Map(0, nullptr, reinterpret_cast<void**>(&vertMap_));

	VertMapping();

	//インデックスリソースを作る
	indexBuff_ = BufferResource::CreateBufferResource(
		device, ((sizeof(uint32_t) + 0xff) & ~0xff) * kIndexNum);
	//リソースの先頭のアドレスから使う
	ibView_.BufferLocation = indexBuff_->GetGPUVirtualAddress();
	//使用するリソースのサイズ
	ibView_.SizeInBytes = sizeof(uint32_t) * kIndexNum;
	//インデックスはuint32_tとする
	ibView_.Format = DXGI_FORMAT_R32_UINT;
	//インデックスリソースにデータを書き込む
	indexBuff_->Map(0, nullptr, reinterpret_cast<void**>(&indexMap_));

	IndexMapping();

	// トランスフォームバッファ
	transformationBuff_ = BufferResource::CreateBufferResource(
		device, ((sizeof(TransformationMatrix) + 0xff) & ~0xff));
	// トランスフォーム行列マップ
	transformationBuff_->Map(0, nullptr, reinterpret_cast<void**>(&transformationMatrixMap_));
	transformationMatrixMap_->World = Matrix4x4::MakeIdentity4x4();
	transformationMatrixMap_->WorldInverseTranspose = Matrix4x4::MakeIdentity4x4();
	transformationMatrixMap_->WVP = Matrix4x4::MakeIdentity4x4();

	// マテリアル
	material_.reset(Material::Create());

	// テクスチャハンドル
	textureHandle_ = textureHandle;

}

void SkyBox::Draw(ID3D12GraphicsCommandList* commandList, BaseCamera* camera)
{
}

void SkyBox::VertMapping()
{

#pragma region 右
	vertMap_[0].position = { 1.0f,1.0f,1.0f,1.0f };
	vertMap_[1].position = { 1.0f,1.0f,-1.0f,1.0f };
	vertMap_[2].position = { 1.0f,-1.0f,1.0f,1.0f };
	vertMap_[3].position = { 1.0f,-1.0f,-1.0f,1.0f };
#pragma endregion

#pragma region 左
	vertMap_[4].position = { -1.0f,1.0f,-1.0f,1.0f };
	vertMap_[5].position = { -1.0f,1.0f,1.0f,1.0f };
	vertMap_[6].position = { -1.0f,-1.0f,-1.0f,1.0f };
	vertMap_[7].position = { -1.0f,-1.0f,1.0f,1.0f };
#pragma endregion

}

void SkyBox::IndexMapping()
{

#pragma region 右
	indexMap_[0] = 0;
	indexMap_[1] = 1;
	indexMap_[2] = 2;
	indexMap_[3] = 2;
	indexMap_[4] = 1;
	indexMap_[5] = 3;
#pragma endregion

#pragma region 左
	indexMap_[0] = 4;
	indexMap_[1] = 5;
	indexMap_[2] = 6;
	indexMap_[3] = 6;
	indexMap_[4] = 5;
	indexMap_[5] = 7;
#pragma endregion

}
