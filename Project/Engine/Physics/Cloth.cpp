#include "Cloth.h"
#include "../base/BufferResource.h"
#include "../base/DirectXCommon.h"
#include <cmath>
#include "../base/TextureManager.h"


// デバイス
ID3D12Device* Cloth::sDevice = nullptr;
// コマンドリスト
ID3D12GraphicsCommandList* Cloth::sCommandList = nullptr;
// ルートシグネチャ
ID3D12RootSignature* Cloth::sRootSignature = nullptr;
// パイプラインステートオブジェクト
ID3D12PipelineState* Cloth::sPipelineState = nullptr;

void Cloth::StaticInitialize(ID3D12Device* device, ID3D12RootSignature* rootSignature, ID3D12PipelineState* pipelineState)
{

	assert(device);
	assert(rootSignature);
	assert(pipelineState);

	sDevice = device;

	sRootSignature = rootSignature;
	sPipelineState = pipelineState;

}

void Cloth::PreManyModelsDraw(ID3D12GraphicsCommandList* cmdList)
{

	assert(sCommandList == nullptr);

	sCommandList = cmdList;

	//RootSignatureを設定。
	sCommandList->SetPipelineState(sPipelineState);//PS0を設定
	sCommandList->SetGraphicsRootSignature(sRootSignature);

	//形状を設定。PS0に設定しているものとは別。
	sCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void Cloth::PostDraw()
{
	// コマンドリストを解除
	sCommandList = nullptr;
}

void Cloth::Initialize(
	float scale, 
	uint32_t div,
	uint32_t textureHandle)
{

	// スケーリング
	scale_ = scale;

	// 質点分割数
	div_ = div;

	// 質点作成と初期化
	ClothPoint point;
	for (uint32_t y = 0; y < div_ + 1; ++y) {
		for (uint32_t x = 0; x < div_ + 1; ++x) {
			// x
			point.currentPos.x = x / div_ * 2.0f - 1.0f;
			point.currentPos.x *= scale_;
			// y
			point.currentPos.y = y / div_ * 2.0f;
			point.currentPos.y *= scale_;
			// z
			point.currentPos.z = 1.0f;
			point.currentPos.z *= scale_;

			// preにコピー
			point.previousPos = point.currentPos;

			// 重み（0.0fなら固定されている）
			if (y == 0) {
				point.weight = 0.0f;
			}
			else {
				point.weight = 1.0f;
			}

			// プッシュバック
			points_.push_back(point);

		}
	}

	// 制約の作成と初期化
	ClothConstraint clothConstraint;
	for (uint32_t y = 0; y < div_ + 1; ++y) {
		for (uint32_t x = 0; x < div_ + 1; ++x) {
	
			// 構成バネ 左
			clothConstraint = ClothConstraintGenerate(x, y, -1, 0, kClothConstraintIndexStructural);
			if (clothConstraint.type != kClothConstraintIndexOfCount) {
				clothConstraints_.push_back(clothConstraint);
			}
			// 構成バネ 上
			clothConstraint = ClothConstraintGenerate(x, y, 0, -1, kClothConstraintIndexStructural);
			if (clothConstraint.type != kClothConstraintIndexOfCount) {
				clothConstraints_.push_back(clothConstraint);
			}

			// せん断バネ 左上
			clothConstraint = ClothConstraintGenerate(x, y, -1, -1, kClothConstraintIndexShear);
			if (clothConstraint.type != kClothConstraintIndexOfCount) {
				clothConstraints_.push_back(clothConstraint);
			}

			// せん断バネ 右上
			clothConstraint = ClothConstraintGenerate(x, y, 1, -1, kClothConstraintIndexShear);
			if (clothConstraint.type != kClothConstraintIndexOfCount) {
				clothConstraints_.push_back(clothConstraint);
			}

			// 曲げバネ １つ飛ばし左
			clothConstraint = ClothConstraintGenerate(x, y, -2, 0, kClothConstraintIndexBending);
			if (clothConstraint.type != kClothConstraintIndexOfCount) {
				clothConstraints_.push_back(clothConstraint);
			}

			// 曲げバネ １つ飛ばし上
			clothConstraint = ClothConstraintGenerate(x, y, 0, -2, kClothConstraintIndexBending);
			if (clothConstraint.type != kClothConstraintIndexOfCount) {
				clothConstraints_.push_back(clothConstraint);
			}

		}
	}

	// 頂点データ初期化
	VertexesInitialize();

	// マテリアル初期化
	material_.reset(Material::Create());
	
	// テクスチャハンドル
	textureHandle_ = textureHandle;

}

void Cloth::Update(const ClothUpdateDesc& desc)
{

	// 質量
	pointMass_ = 1.0f;
	float g = desc.gravity * pointMass_;
	float w = desc.wind * pointMass_;
	float k = desc.basicStrength * pointMass_;

	// 積分フェーズ
	IntegralPhase(g, w, desc.acc,desc.step, desc.resistance);

	// 制約充足フェーズ
	ConstraintSatisfactionPhaseDesc constraintSatisfactionPhaseDesc;
	constraintSatisfactionPhaseDesc.step = desc.step;
	constraintSatisfactionPhaseDesc.basicStrength = desc.basicStrength;
	constraintSatisfactionPhaseDesc.structuralShrink = desc.bendingShrink;
	constraintSatisfactionPhaseDesc.structuralStretch = desc.structuralStretch;
	constraintSatisfactionPhaseDesc.shearShrink = desc.shearShrink;
	constraintSatisfactionPhaseDesc.shearStretch = desc.shearStretch;
	constraintSatisfactionPhaseDesc.bendingShrink = desc.bendingShrink;
	constraintSatisfactionPhaseDesc.bendingStretch = desc.bendingStretch;

	// 反復して安定させる
	for (uint32_t i = 0; i < desc.constraintSatisfactionPhaseNum; ++i) {
		ConstraintSatisfactionPhase(constraintSatisfactionPhaseDesc);
	}

	// 衝突判定フェーズ

	// 頂点更新
	VertexesUpdate();

}

void Cloth::Draw(BaseCamera& camera)
{

	// nullptrチェック
	assert(sDevice);
	assert(sCommandList);

	//worldTransform.Map();

	sCommandList->IASetVertexBuffers(0, 1, &vbView_); //VBVを設定

	//マテリアルCBufferの場所を設定
	sCommandList->SetGraphicsRootConstantBufferView(0, material_->GetMaterialBuff()->GetGPUVirtualAddress());

	// カメラCBufferの場所を設定
	//sCommandList->SetGraphicsRootConstantBufferView(7, camera.GetWorldPositionBuff()->GetGPUVirtualAddress());

	// ワールドトランスフォーム
	//sCommandList->SetGraphicsRootConstantBufferView(10, worldTransform.GetTransformationMatrixBuff()->GetGPUVirtualAddress());
	// ビュープロジェクション
	sCommandList->SetGraphicsRootConstantBufferView(1, camera.GetViewProjectionMatriBuff()->GetGPUVirtualAddress());

	//SRVのDescriptorTableの先頭を設定。2はrootParamenter[2]である
	TextureManager::GetInstance()->SetGraphicsRootDescriptorTable(sCommandList, 2, textureHandle_);

	// ポイントライト
	//if (pointLightManager_) {
	//	pointLightManager_->Draw(sCommandList, 8);
	//}
	//// スポットライト
	//if (spotLightManager_) {
	//	spotLightManager_->Draw(sCommandList, 9);
	//}

	//worldTransform.SetGraphicsRootDescriptorTable(sCommandList, 1);

	//描画
	sCommandList->DrawInstanced(UINT(points_.size()), 1, 0, 0);

}

Cloth::ClothConstraint Cloth::ClothConstraintGenerate(
	uint32_t x, 
	uint32_t y, 
	int32_t offsetX, 
	int32_t offsetY, 
	ClothConstraintIndex type)
{

	const int32_t targetX = x + offsetX;
	const int32_t targetY = y + offsetY;

	ClothConstraint clothConstraint{ {},{},0.0f ,kClothConstraintIndexOfCount };
	if (targetX >= 0 &&
		targetX < static_cast<int32_t>(div_) + 1 &&
		targetY >= 0 &&
		targetY < static_cast<int32_t>(div_) + 1) {

		clothConstraint.p1 = &points_[y * (div_ + 1) + x];
		clothConstraint.p2 = &points_[targetY * (div_ + 1) + targetY];
		clothConstraint.naturalLength = scale_ * 2.0f / div_ * std::sqrtf(static_cast<float>(offsetX * offsetX + offsetY * offsetY));
		clothConstraint.type = type;
	}

	return clothConstraint;

}

void Cloth::VertexesInitialize()
{

	// 質点の数分作る

	// バッファ
	vertBuff_ = BufferResource::CreateBufferResource(
		sDevice,
		((sizeof(VertexData) + 0xff) & ~0xff) * points_.size());

	//リソースの先頭のアドレスから使う
	vbView_.BufferLocation = vertBuff_->GetGPUVirtualAddress();
	//使用するリソースのサイズは頂点3つ分のサイズ
	vbView_.SizeInBytes = UINT(sizeof(VertexData) * points_.size());
	//1頂点あたりのサイズ
	vbView_.StrideInBytes = sizeof(VertexData);

	//書き込むためのアドレスを取得
	vertBuff_->Map(0, nullptr, reinterpret_cast<void**>(&vertMap_));
	//頂点データをリソースにコピー
	std::memcpy(vertMap_, points_.data(), sizeof(VertexData) * points_.size());

	for (uint32_t i = 0; i < points_.size(); ++i) {
		// 位置
		vertMap_[i].position = Vector4(
			points_[i].currentPos.x,
			points_[i].currentPos.y,
			points_[i].currentPos.z,
			1.0f);

		// 法線
		vertMap_[i].normal = { 0.0f, 0.0f, -1.0f };

		// テクスチャ座標
		vertMap_[i].texcoord.x = points_[i].currentPos.x / scale_;
		vertMap_[i].texcoord.y = points_[i].currentPos.y / scale_;
	}

}

void Cloth::VertexesUpdate()
{

	for (uint32_t i = 0; i < points_.size(); ++i) {
		// 位置
		vertMap_[i].position = Vector4(
			points_[i].currentPos.x,
			points_[i].currentPos.y, 
			points_[i].currentPos.z, 
			1.0f);

		// 法線
		//vertMap_[i].normal = { 0.0f, 0.0f, -1.0f };
	}

}

void Cloth::IntegralPhase(
	float gravity, 
	float wind, 
	float acc, 
	float step, 
	float resistance)
{

	// 重力と風力による変位(移動量)を計算しておく
	Vector3 f{};
	f.x = 0.0f; // 特に力は加えない
	f.y -= gravity; // 重力
	f.z += wind * (std::sinf(acc) * 0.5f + 0.5f); // 風力（適当になびかせる）

	// 力を変位に変換しておく
	f = f * (step * step * 0.5f / pointMass_);

	// 抵抗は速度に対して働く
	float r = 1.0f - resistance * step;

	for (uint32_t i = 0; i < points_.size(); ++i) {

		// 変位
		Vector3 dx;
		dx = points_[i].currentPos - points_[i].previousPos; // 速度
		points_[i].previousPos = points_[i].currentPos; // 前の位置を更新
		dx += f; // 力の変位を足しこむ
		dx *= r; // 抵抗

		// 位置更新
		dx *= points_[i].weight; // 固定点は動かさない
		points_[i].currentPos += dx;

	}

}

void Cloth::ConstraintSatisfactionPhase(const ConstraintSatisfactionPhaseDesc& desc)
{

	for (uint32_t i = 0; i < clothConstraints_.size(); ++i) {

		// 二つの質点がお互いに固定点であればスキップ(0除算)
		if (clothConstraints_[i].p1->weight + clothConstraints_[i].p2->weight == 0.0f) {
			continue;
		}

		// 伸び抵抗と縮み抵抗
		float shrink = 0.0f;
		float stretch = 0.0f;
		if (clothConstraints_[i].type == kClothConstraintIndexStructural) {
			shrink = desc.structuralShrink;
			stretch = desc.structuralStretch;
		}else if(clothConstraints_[i].type == kClothConstraintIndexShear){
			shrink = desc.shearShrink;
			stretch = desc.shearStretch;
		}
		else {
			shrink = desc.bendingShrink;
			stretch = desc.bendingStretch;
		}

		// バネの力(スカラー)
		float distance = Vector3::Length(clothConstraints_[i].p2->currentPos - clothConstraints_[i].p1->currentPos);
		float f = (distance - clothConstraints_[i].naturalLength) * desc.basicStrength;
		if (f >= 0.0f) {
			f *= shrink;
		}
		else {
			f *= stretch;
		}

		// 変位
		Vector3 dx;
		dx = clothConstraints_[i].p2->currentPos, clothConstraints_[i].p1->currentPos; // 方向ベクトルを作る
		dx = Vector3::Normalize(dx); // 正規化
		dx *= f; // バネの力を加える
		dx *= desc.step * desc.step * 0.5f / pointMass_; // 力を変位に変換

		// 位置更新（二つの質点を互いに移動させる）
		Vector3 dxP1;
		dxP1 = dx * clothConstraints_[i].p1->weight * (1.0f / (clothConstraints_[i].p1->weight + clothConstraints_[i].p2->weight));
		clothConstraints_[i].p1->currentPos += dxP1;

		Vector3 dxP2;
		dxP2 = dx * clothConstraints_[i].p2->weight * (1.0f / (clothConstraints_[i].p1->weight + clothConstraints_[i].p2->weight));
		clothConstraints_[i].p2->currentPos += dxP2;

	}

}
