#include "Model.h"
#include "../base/TextureManager.h"
#include "../base/WinApp.h"
#include <cassert>

#include <fstream>
#include <sstream>
#include <format>
#include "../base/BufferResource.h"
using namespace DirectX;
using namespace Microsoft::WRL;

// デバイス
ID3D12Device* Model::sDevice = nullptr;
// ディスクリプタサイズ
UINT Model::sDescriptorHandleIncrementSize;
// コマンドリスト
ID3D12GraphicsCommandList* Model::sCommandList = nullptr;
// ルートシグネチャ
ID3D12RootSignature* Model::sRootSignature[GraphicsPipelineState::PipelineStateName::kCountOfPipelineStateName];
// パイプラインステートオブジェクト
ID3D12PipelineState* Model::sPipelineState[GraphicsPipelineState::PipelineStateName::kCountOfPipelineStateName];
//計算
Matrix4x4Calc* Model::matrix4x4Calc = nullptr;

/// <summary>
/// 静的初期化
/// </summary>
/// <param name="device">デバイス</param>
void Model::StaticInitialize(ID3D12Device* device,
	const std::array<ID3D12RootSignature*, GraphicsPipelineState::PipelineStateName::kCountOfPipelineStateName>& rootSignature,
	const std::array<ID3D12PipelineState*, GraphicsPipelineState::PipelineStateName::kCountOfPipelineStateName>& pipelineState) {

	assert(device);

	sDevice = device;

	matrix4x4Calc = Matrix4x4Calc::GetInstance();

	// グラフィックパイプライン生成
	for (uint32_t i = 0u; i < GraphicsPipelineState::PipelineStateName::kCountOfPipelineStateName; i++) {
		sRootSignature[i] = rootSignature[i];
		sPipelineState[i] = pipelineState[i];
	}

}

/// <summary>
/// 静的前処理
/// </summary>
/// <param name="cmdList">描画コマンドリスト</param>
void Model::PreDraw(ID3D12GraphicsCommandList* cmdList) {

	assert(sCommandList == nullptr);

	sCommandList = cmdList;

	//RootSignatureを設定。
	sCommandList->SetPipelineState(sPipelineState[GraphicsPipelineState::PipelineStateName::kModel]);//PS0を設定
	sCommandList->SetGraphicsRootSignature(sRootSignature[GraphicsPipelineState::PipelineStateName::kModel]);

	//形状を設定。PS0に設定しているものとは別。
	sCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

}

void Model::PreParticleDraw(ID3D12GraphicsCommandList* cmdList, const Matrix4x4& viewProjectionMatrix)
{

	assert(sCommandList == nullptr);

	sCommandList = cmdList;

	//RootSignatureを設定。
	sCommandList->SetPipelineState(sPipelineState[GraphicsPipelineState::PipelineStateName::kParticle]);//PS0を設定
	sCommandList->SetGraphicsRootSignature(sRootSignature[GraphicsPipelineState::PipelineStateName::kParticle]);

	//形状を設定。PS0に設定しているものとは別。
	sCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	ParticleManager* particleManager = ParticleManager::GetInstance();
	particleManager->Map(viewProjectionMatrix);

}

/// <summary>
/// 静的前処理
/// </summary>
/// <param name="cmdList">描画コマンドリスト</param>
void Model::PreDrawOutLine(ID3D12GraphicsCommandList* cmdList) {

	assert(sCommandList == nullptr);

	sCommandList = cmdList;

	assert(sPipelineState[GraphicsPipelineState::PipelineStateName::kOutLine]);

	//RootSignatureを設定。
	sCommandList->SetPipelineState(sPipelineState[GraphicsPipelineState::PipelineStateName::kOutLine]);//PS0を設定
	sCommandList->SetGraphicsRootSignature(sRootSignature[GraphicsPipelineState::PipelineStateName::kOutLine]);

	//形状を設定。PS0に設定しているものとは別。
	sCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

}

/// <summary>
/// 描画後処理
/// </summary>
void Model::PostDraw() {
	// コマンドリストを解除
	sCommandList = nullptr;
}

/// <summary>
/// 3Dモデル生成
/// </summary>
/// <returns></returns>
Model* Model::Create(const std::string& directoryPath, const std::string& filename, DirectXCommon* dxCommon, ITextureHandleManager* textureHandleManager) {

	// 3Dオブジェクトのインスタンスを生成
	Model* object3d = new Model();
	assert(object3d);

	// 初期化
	object3d->Initialize(directoryPath, filename, dxCommon, textureHandleManager);

	return object3d;

}

Model::MaterialData Model::LoadMaterialTemplateFile(const std::string& directoryPath, const std::string& filename) {

	//1. 中で必要となる変数
	MaterialData materialData;//構築するデータMaterialData
	std::string line;//ファイルから読んだ1行を格納するもの

	//2. ファイルを開く
	std::ifstream file(directoryPath + "/" + filename);//ファイルを開く
	assert(file.is_open());//開けなっかたら止める

	//3. 実際にファイルを読み、MaterialDataを構築していく
	while (std::getline(file, line)) {
		std::string identifier;
		std::istringstream s(line);
		s >> identifier;

		//identiferに応じた処理
		if (identifier == "map_Kd") {
			std::string texturefFilename;
			s >> texturefFilename;
			//連結してファイルパスにする
			materialData.textureFilePath = directoryPath + "/" + texturefFilename;
		}
	}
	//4. MaterialData を返す
	return materialData;

}

//objファイルを読む
Model::ModelData Model::LoadObjFile(const std::string& directoryPath, const std::string& filename) {

	//1. 変数宣言
	Model::ModelData modelData; // 構築するModelData
	std::vector<Vector4> positions; //位置
	std::vector<Vector3> normals; // 法線
	std::vector<Vector2> texcoords;  //テクスチャ座標
	std::string line; // ファイルから読んだ1行を格納するもの

	//2. ファイルを開く
	std::ifstream file(directoryPath + "/" + filename); //ファイルを開く
	assert(file.is_open()); //開けなかったら止める

	//3. ファイルを読んでモデルデータを構築
	while (std::getline(file, line)) {
		std::string identifier;
		std::istringstream s(line);
		s >> identifier;//先頭の識別子を読む

		//identifierに応じた処理
		if (identifier == "v") {
			Vector4 position;
			s >> position.x >> position.y >> position.z;
			position.x *= -1.0f;
			position.w = 1.0f;
			positions.push_back(position);
		}
		else if (identifier == "vt") {
			Vector2 texcoord;
			s >> texcoord.x >> texcoord.y;
			texcoord.y = 1.0f - texcoord.y;
			texcoords.push_back(texcoord);
		}
		else if (identifier == "vn") {
			Vector3 normal;
			s >> normal.x >> normal.y >> normal.z;
			normal.x *= -1.0f;
			normals.push_back(normal);
		}
		else if (identifier == "f") {
			VertexData triangle[3];
			//面は三角形限定。その他は未対応
			for (int32_t faceVertex = 0; faceVertex < 3; ++faceVertex) {
				std::string vertexDefintion;
				s >> vertexDefintion;
				//頂点の要素へのIndexは「位置/UV/法線」で格納されているので、分かいしてIndexを取得する
				std::istringstream v(vertexDefintion);
				uint32_t elementIndices[3];
				for (int32_t element = 0; element < 3; ++element) {
					std::string index;
					std::getline(v, index, '/');//区切りでインデックスを読んでいく
					elementIndices[element] = std::stoi(index);
				}
				//要素へのIndexから、実際の要素の値を取得して、頂点を構築する
				Vector4 position = positions[elementIndices[0] - 1];
				Vector2 texcoord = texcoords[elementIndices[1] - 1];
				Vector3 normal = normals[elementIndices[2] - 1];
				//VertexData vertex = { position, texcoord, normal };
				//modelData.vertices.push_back(vertex);
				triangle[faceVertex] = { position, texcoord, normal };
			}
			//頂点を逆順で登録することで、回り順を逆にする
			modelData.vertices.push_back(triangle[2]);
			modelData.vertices.push_back(triangle[1]);
			modelData.vertices.push_back(triangle[0]);
		}
		else if (identifier == "mtllib") {
			//materialTemplateLibrayファイルの名前を取得する
			std::string materialFilename;
			s >> materialFilename;
			//基本的にobjファイルを同一階層にmtlは存在させるので、ディレクトリ名とファイル名を渡す
			modelData.material = LoadMaterialTemplateFile(directoryPath, materialFilename);
		}
	}

	return modelData;

}


/// <summary>
/// 初期化
/// </summary>
void Model::Initialize(const std::string& directoryPath, const std::string& filename, DirectXCommon* dxCommon, ITextureHandleManager* textureHandleManager) {

	assert(sDevice);

	//メッシュ生成
	CreateMesh(directoryPath, filename);

	textureHandle_ = TextureManager::Load(modelData.material.textureFilePath, dxCommon, textureHandleManager);

	resourceDesc_ = TextureManager::GetInstance()->GetResourceDesc(textureHandle_);

	// マテリアル
	defaultMaterial_.reset(Material::Create());

}

/// <summary>
/// 更新
/// </summary>
void Model::Update() {

}

/// <summary>
/// 描画
/// </summary>
void Model::Draw(WorldTransform& worldTransform, BaseCamera& camera) {

	// nullptrチェック
	assert(sDevice);
	assert(sCommandList);

	worldTransform.Map(camera.GetViewProjectionMatrix());

	sCommandList->IASetVertexBuffers(0, 1, &vbView_); //VBVを設定

	//wvp用のCBufferの場所を設定
	sCommandList->SetGraphicsRootConstantBufferView(1, worldTransform.transformationMatrixBuff_->GetGPUVirtualAddress());

	//マテリアルCBufferの場所を設定
	sCommandList->SetGraphicsRootConstantBufferView(0, defaultMaterial_->GetMaterialBuff()->GetGPUVirtualAddress());

	// カメラCBufferの場所を設定
	sCommandList->SetGraphicsRootConstantBufferView(4, camera.GetWorldPositionBuff()->GetGPUVirtualAddress());

	//SRVのDescriptorTableの先頭を設定。2はrootParamenter[2]である
	TextureManager::GetInstance()->SetGraphicsRootDescriptorTable(sCommandList, 2, textureHandle_);

	//描画
	sCommandList->DrawInstanced(UINT(modelData.vertices.size()), 1, 0, 0);

}

/// <summary>
/// 描画
/// </summary>
void Model::Draw(WorldTransform& worldTransform, BaseCamera& camera, Material* material) {

	// nullptrチェック
	assert(sDevice);
	assert(sCommandList);

	worldTransform.Map(camera.GetViewProjectionMatrix());

	sCommandList->IASetVertexBuffers(0, 1, &vbView_); //VBVを設定

	//wvp用のCBufferの場所を設定
	sCommandList->SetGraphicsRootConstantBufferView(1, worldTransform.transformationMatrixBuff_->GetGPUVirtualAddress());

	//マテリアルCBufferの場所を設定
	sCommandList->SetGraphicsRootConstantBufferView(0, material->GetMaterialBuff()->GetGPUVirtualAddress());

	// カメラCBufferの場所を設定
	sCommandList->SetGraphicsRootConstantBufferView(4, camera.GetWorldPositionBuff()->GetGPUVirtualAddress());

	//SRVのDescriptorTableの先頭を設定。2はrootParamenter[2]である
	TextureManager::GetInstance()->SetGraphicsRootDescriptorTable(sCommandList, 2, textureHandle_);

	//描画
	sCommandList->DrawInstanced(UINT(modelData.vertices.size()), 1, 0, 0);

}

/// <summary>
/// 描画
/// </summary>
void Model::Draw(WorldTransform& worldTransform, BaseCamera& camera, Material* material,uint32_t textureHandle) {

	// nullptrチェック
	assert(sDevice);
	assert(sCommandList);

	worldTransform.Map(camera.GetViewProjectionMatrix());

	sCommandList->IASetVertexBuffers(0, 1, &vbView_); //VBVを設定

	//wvp用のCBufferの場所を設定
	sCommandList->SetGraphicsRootConstantBufferView(1, worldTransform.transformationMatrixBuff_->GetGPUVirtualAddress());

	//マテリアルCBufferの場所を設定
	sCommandList->SetGraphicsRootConstantBufferView(0, material->GetMaterialBuff()->GetGPUVirtualAddress());

	// カメラCBufferの場所を設定
	sCommandList->SetGraphicsRootConstantBufferView(4, camera.GetWorldPositionBuff()->GetGPUVirtualAddress());

	//SRVのDescriptorTableの先頭を設定。2はrootParamenter[2]である
	TextureManager::GetInstance()->SetGraphicsRootDescriptorTable(sCommandList, 2, textureHandle);

	//描画
	sCommandList->DrawInstanced(UINT(modelData.vertices.size()), 1, 0, 0);

}

void Model::ParticleDraw()
{

	// nullptrチェック
	assert(sDevice);
	assert(sCommandList);

	ParticleManager* particleManager = ParticleManager::GetInstance();

	sCommandList->IASetVertexBuffers(0, 1, &vbView_); //VBVを設定

	//マテリアルCBufferの場所を設定
	sCommandList->SetGraphicsRootConstantBufferView(0, defaultMaterial_->GetMaterialBuff()->GetGPUVirtualAddress());

	//マテリアルCBufferの場所を設定
	sCommandList->SetGraphicsRootConstantBufferView(4, particleManager->GetCurrentStartInstanceIdBuff()->GetGPUVirtualAddress());

	//SRVのDescriptorTableの先頭を設定。2はrootParamenter[2]である
	TextureManager::GetInstance()->SetGraphicsRootDescriptorTable(sCommandList, 2, textureHandle_);

	sCommandList->SetGraphicsRootDescriptorTable(1, particleManager->GetInstancingSrvHandleGPU());

	//描画
	sCommandList->DrawInstanced(UINT(modelData.vertices.size()), particleManager->GetCurrentInstanceIndex(), 0, 0);

}

void Model::OutLineDraw(WorldTransform& worldTransform, BaseCamera& camera, OutLineData& outLineData) {
	// nullptrチェック
	assert(sDevice);
	assert(sCommandList);

	worldTransform.Map(camera.GetViewProjectionMatrix());
	sCommandList->IASetVertexBuffers(0, 1, &vbView_); //VBVを設定

	//wvp用のCBufferの場所を設定
	sCommandList->SetGraphicsRootConstantBufferView(1, worldTransform.transformationMatrixBuff_->GetGPUVirtualAddress());

	//マテリアルCBufferの場所を設定
	sCommandList->SetGraphicsRootConstantBufferView(0, outLineData.forPSResource_->GetGPUVirtualAddress());

	//マテリアルCBufferの場所を設定
	sCommandList->SetGraphicsRootConstantBufferView(2, outLineData.forVSResource_->GetGPUVirtualAddress());

	
	//描画
	sCommandList->DrawInstanced(UINT(modelData.vertices.size()), 1, 0, 0);

}

/// <summary>
/// メッシュデータ生成
/// </summary>
void Model::CreateMesh(const std::string& directoryPath, const std::string& filename) {


	//モデル読み込み
	modelData = LoadObjFile(directoryPath, filename);

	vertBuff_ = BufferResource::CreateBufferResource(sDevice, sizeof(VertexData) * modelData.vertices.size());

	//リソースの先頭のアドレスから使う
	vbView_.BufferLocation = vertBuff_->GetGPUVirtualAddress();
	//使用するリソースのサイズは頂点3つ分のサイズ
	vbView_.SizeInBytes = UINT(sizeof(VertexData) * modelData.vertices.size());
	//1頂点あたりのサイズ
	vbView_.StrideInBytes = sizeof(VertexData);

	//書き込むためのアドレスを取得
	vertBuff_->Map(0, nullptr, reinterpret_cast<void**>(&vertMap));
	//頂点データをリソースにコピー
	std::memcpy(vertMap, modelData.vertices.data(), sizeof(VertexData) * modelData.vertices.size());

}


/// <summary>
/// テクスチャハンドルの設定
/// </summary>
/// <param name="textureHandle"></param>
void Model::SetTextureHandle(uint32_t textureHandle) {

	textureHandle_ = textureHandle;
	resourceDesc_ = TextureManager::GetInstance()->GetResourceDesc(textureHandle_);

}


