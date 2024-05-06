#include "WindowSpriteStorage.h"
#include "../base/BufferResource.h"
#include "../base/Log.h"
#include "../base/CompileShader.h"
#include <fstream>
#include <cmath>
#include "../2D/ImguiManager.h"

WindowSpriteStorage* WindowSpriteStorage::GetInstance()
{
	static WindowSpriteStorage instance;
	return &instance;
}

void WindowSpriteStorage::Initialize()
{

	// デバイス取得
	device_ = DirectXCommon::GetInstance()->GetDevice();

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
	computeParametersMap_->overwriteCount = 0;  // 上書き回数

	// ルートシグネチャ
	CreateRootSignature();

	//シェーダー
	CompileShader();

	// パイプライン
	CreatePipline();

	TemporaryStorageInitialize();

}

void WindowSpriteStorage::ImGuiDraw()
{
}

void WindowSpriteStorage::Reset()
{

	computeParametersMap_->overwriteCount = 0;  // 上書き回数

}

void WindowSpriteStorage::CreateRootSignature()
{

	HRESULT hr;

	D3D12_ROOT_SIGNATURE_DESC descriptionRootsignature{};
	descriptionRootsignature.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	// ディスクリプタレンジ
	
	// ソース
	D3D12_DESCRIPTOR_RANGE descriptorRangeSouce[8][1] = {};
	for (uint32_t i = 0; i < kTemporaryStorageTexturesNum; ++i) {

		descriptorRangeSouce[i][0].BaseShaderRegister = i;//0から始まる
		descriptorRangeSouce[i][0].NumDescriptors = 1;//数は一つ
		descriptorRangeSouce[i][0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;//SAVを使う
		descriptorRangeSouce[i][0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;//Offsetを自動計算

	}

	// 保存された画像をすべて上書きした画像
	D3D12_DESCRIPTOR_RANGE descriptorRangeOverwrite[1] = {};
	descriptorRangeOverwrite[0].BaseShaderRegister = 0;//0から始まる
	descriptorRangeOverwrite[0].NumDescriptors = 1;//数は一つ
	descriptorRangeOverwrite[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_UAV;//UAVを使う
	descriptorRangeOverwrite[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;//Offsetを自動計算

	// ルートパラメータ
	D3D12_ROOT_PARAMETER rootParameters[10] = {};

	// 上書き定数バッファ
	rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;   //CBVを使う
	rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL; //ALLで使う
	rootParameters[0].Descriptor.ShaderRegister = 0;                  //レジスタ番号0とバインド

	// ソース
	for (uint32_t i = 0; i < kTemporaryStorageTexturesNum; ++i) {

		rootParameters[1 + i].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;//DescriptorTableを使う
		rootParameters[1 + i].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;//全てで使う
		rootParameters[1 + i].DescriptorTable.pDescriptorRanges = descriptorRangeSouce[i];//Tableの中身の配列を指定
		rootParameters[1 + i].DescriptorTable.NumDescriptorRanges = _countof(descriptorRangeSouce[i]);//Tableで利用する数

	}

	// 保存された画像をすべて上書きした画像
	rootParameters[9].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;//DescriptorTableを使う
	rootParameters[9].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;//全てで使う
	rootParameters[9].DescriptorTable.pDescriptorRanges = descriptorRangeOverwrite;//Tableの中身の配列を指定
	rootParameters[9].DescriptorTable.NumDescriptorRanges = _countof(descriptorRangeOverwrite);//Tableで利用する数

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

void WindowSpriteStorage::CreateHeaderHLSL()
{

	// ファイルを開く
	std::ofstream file("Resources/shaders/WindowSpriteStorage.hlsli");

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

void WindowSpriteStorage::CompileShader()
{

	// ヘッダを作成
	CreateHeaderHLSL();

	for (uint32_t i = 0; i < kPipelineIndexOfCount; ++i) {

		// コンパイル
		shaders_[i] = CompileShader::Compile(
			shaderNames_[i].first,
			L"cs_6_0",
			shaderNames_[i].second);

	}

}

void WindowSpriteStorage::CreatePipline()
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

void WindowSpriteStorage::TemporaryStorageRegister(
	ID3D12GraphicsCommandList* commandList, 
	const CD3DX12_GPU_DESCRIPTOR_HANDLE& GPUHandle,
	const std::string& name)
{

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

	commandList_->SetComputeRootDescriptorTable(1, GPUHandle);

	commandList_->SetComputeRootDescriptorTable(9, temporaryStorageTextures_[computeParametersMap_->overwriteCount].second->GetUavHandleGPU());

	// 実行
	commandList_->Dispatch(x, y, z);

	// コマンドリスト
	commandList_ = nullptr;

	temporaryStorageTextures_[computeParametersMap_->overwriteCount].first = name;

	computeParametersMap_->overwriteCount++;

}

void WindowSpriteStorage::TemporaryStoragOverwrite(
	ID3D12GraphicsCommandList* commandList,
	std::array<std::string, kTemporaryStorageTexturesNum>* temporaryStorageNames)
{

	// コマンドリスト
	commandList_ = commandList;

	// コマンドリストがヌルならエラー
	assert(commandList_);

	// ルートシグネチャ
	commandList_->SetComputeRootSignature(rootSignature_.Get());
	// パイプライン
	commandList_->SetPipelineState(pipelineStates_[kPipelineIndexOverwrite].Get());

	// ディスパッチ数
	uint32_t x = (kTextureWidth + kNumThreadX - 1) / kNumThreadX;
	uint32_t y = (kTextureHeight + kNumThreadY - 1) / kNumThreadY;
	uint32_t z = 1;

	// バッファを送る
	// 定数パラメータ
	commandList_->SetComputeRootConstantBufferView(0, computeParametersBuff_->GetGPUVirtualAddress());

	if (temporaryStorageNames) {
		assert(0);
	}
	else {
		for (uint32_t i = 0; i < computeParametersMap_->overwriteCount; ++i) {
			commandList_->SetComputeRootDescriptorTable(1 + i, temporaryStorageTextures_[i].second->GetUavHandleGPU());
		}
	}

	commandList_->SetComputeRootDescriptorTable(9, temporaryStorageOverwriteTexture_->GetUavHandleGPU());

	// 実行
	commandList_->Dispatch(x, y, z);

	// コマンドリスト
	commandList_ = nullptr;

}

void WindowSpriteStorage::TemporaryStorageInitialize()
{

	// 保存する画像初期化
	for (uint32_t i = 0; i < kTemporaryStorageTexturesNum; ++i) {
		temporaryStorageTextures_[i].first = "NoName";
		temporaryStorageTextures_[i].second = std::make_unique<TextureUAV>();
		temporaryStorageTextures_[i].second->Initialize(
			device_,
			kTextureWidth,
			kTextureHeight);
	}

	// 保存された画像をすべて上書きした画像
	temporaryStorageOverwriteTexture_ = std::make_unique<TextureUAV>();
	temporaryStorageOverwriteTexture_->Initialize(
		device_,
		kTextureWidth,
		kTextureHeight);

}
