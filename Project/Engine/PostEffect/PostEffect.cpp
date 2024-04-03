#include "PostEffect.h"
#include "../base/BufferResource.h"
#include "../base/Log.h"
#include "../base/CompileShader.h"
#include <fstream>
#include <cmath>

PostEffect* PostEffect::GetInstance()
{
	static PostEffect instance;
	return &instance;
}

void PostEffect::Initialize()
{

	// デバイス取得
	device_ = DirectXCommon::GetInstance()->GetDevice();

	// 定数バッファ作成
	computeParametersBuff_ = BufferResource::CreateBufferResource(device_, ((sizeof(ComputeParameters) + 0xff) & ~0xff));
	computeParametersBuff_->Map(0, nullptr, reinterpret_cast<void**>(&computeParametersMap_));

	// 定数バッファに渡す値の設定
	computeParametersMap_->threadIdOffsetX = 0; // スレッドのオフセットX
	computeParametersMap_->threadIdTotalX = 1; // スレッドの総数X
	computeParametersMap_->threadIdOffsetY = 0; // スレッドのオフセットY
	computeParametersMap_->threadIdTotalY = 1; // スレッドの総数Y
	computeParametersMap_->threadIdOffsetZ = 0; // スレッドのオフセットZ
	computeParametersMap_->threadIdTotalZ = 1; // スレッドの総数Z

	computeParametersMap_->clearColor = { 0.1f, 0.25f, 0.5f, 1.0f }; // クリアするときの色
	computeParametersMap_->threshold = 0.8f; // しきい値

	computeParametersMap_->kernelSize = 7; // カーネルサイズ
	computeParametersMap_->sigma = 20.0f; // 標準偏差

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

	// 定数設定
	computeParametersMap_->threadIdOffsetX = 0; // スレッドのオフセットX
	computeParametersMap_->threadIdTotalX = kTextureWidth; // スレッドの総数X
	computeParametersMap_->threadIdOffsetY = 0; // スレッドのオフセットY
	computeParametersMap_->threadIdTotalY = kTextureHeight; // スレッドの総数Y
	computeParametersMap_->threadIdOffsetZ = 0; // スレッドのオフセットZ
	computeParametersMap_->threadIdTotalZ = 1; // スレッドの総数Z

	// バッファを送る

	// 定数パラメータ
	commandList_->SetComputeRootConstantBufferView(0, computeParametersBuff_->GetGPUVirtualAddress());
	// コピーする画像
	commandList_->SetComputeRootDescriptorTable(1, copyGPUHandle);
	// 編集する画像セット
	editTextures_[editTextureIndex]->SetRootDescriptorTable(commandList_, 3);

	// ディスパッチ数
	uint32_t x = (kTextureWidth + kNumThreadX - 1) / kNumThreadX;
	uint32_t y = (kTextureHeight + kNumThreadY - 1) / kNumThreadY;
	uint32_t z = 1;

	// 実行
	commandList_->Dispatch(x, y, z);

	// コマンドリスト
	commandList_ = nullptr;

}

void PostEffect::ClearCommand(
	ID3D12GraphicsCommandList* commandList,
	uint32_t editTextureIndex,
	const Vector4& color)
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

	// 定数設定
	computeParametersMap_->threadIdOffsetX = 0; // スレッドのオフセットX
	computeParametersMap_->threadIdTotalX = kTextureWidth; // スレッドの総数X
	computeParametersMap_->threadIdOffsetY = 0; // スレッドのオフセットY
	computeParametersMap_->threadIdTotalY = kTextureHeight; // スレッドの総数Y
	computeParametersMap_->threadIdOffsetZ = 0; // スレッドのオフセットZ
	computeParametersMap_->threadIdTotalZ = 1; // スレッドの総数Z

	computeParametersMap_->clearColor = color; // クリアするときの色

	// バッファを送る

	// 定数パラメータ
	commandList_->SetComputeRootConstantBufferView(0, computeParametersBuff_->GetGPUVirtualAddress());
	// 編集する画像セット
	editTextures_[editTextureIndex]->SetRootDescriptorTable(commandList_, 3);

	// ディスパッチ数
	uint32_t x = (kTextureWidth + kNumThreadX - 1) / kNumThreadX;
	uint32_t y = (kTextureHeight + kNumThreadY - 1) / kNumThreadY;
	uint32_t z = 1;

	// 実行
	commandList_->Dispatch(x, y, z);

	// コマンドリスト
	commandList_ = nullptr;

}

void PostEffect::BinaryThresholdCommand(
	ID3D12GraphicsCommandList* commandList,
	uint32_t editTextureIndex,
	float threshold,
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

	// 定数設定
	computeParametersMap_->threadIdOffsetX = 0; // スレッドのオフセットX
	computeParametersMap_->threadIdTotalX = kTextureWidth; // スレッドの総数X
	computeParametersMap_->threadIdOffsetY = 0; // スレッドのオフセットY
	computeParametersMap_->threadIdTotalY = kTextureHeight; // スレッドの総数Y
	computeParametersMap_->threadIdOffsetZ = 0; // スレッドのオフセットZ
	computeParametersMap_->threadIdTotalZ = 1; // スレッドの総数Z

	computeParametersMap_->threshold = threshold; // しきい値

	// バッファを送る

	// 定数パラメータ
	commandList_->SetComputeRootConstantBufferView(0, computeParametersBuff_->GetGPUVirtualAddress());
	// 二値化する画像をセット
	commandList_->SetComputeRootDescriptorTable(1, binaryThresholdGPUHandle);
	// 編集する画像セット
	editTextures_[editTextureIndex]->SetRootDescriptorTable(commandList_, 3);

	// ディスパッチ数
	uint32_t x = (kTextureWidth + kNumThreadX - 1) / kNumThreadX;
	uint32_t y = (kTextureHeight + kNumThreadY - 1) / kNumThreadY;
	uint32_t z = 1;

	// 実行
	commandList_->Dispatch(x, y, z);

	// コマンドリスト
	commandList_ = nullptr;


}

void PostEffect::GaussianBlurCommand(
	ID3D12GraphicsCommandList* commandList,
	uint32_t editTextureIndex,
	int32_t kernelSize,
	float sigma,
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
	// パイプライン
	commandList_->SetPipelineState(pipelineStates_[kPipelineIndexGaussianBlur].Get());

	// 定数設定
	computeParametersMap_->threadIdOffsetX = 0; // スレッドのオフセットX
	computeParametersMap_->threadIdTotalX = kTextureWidth; // スレッドの総数X
	computeParametersMap_->threadIdOffsetY = 0; // スレッドのオフセットY
	computeParametersMap_->threadIdTotalY = kTextureHeight; // スレッドの総数Y
	computeParametersMap_->threadIdOffsetZ = 0; // スレッドのオフセットZ
	computeParametersMap_->threadIdTotalZ = 1; // スレッドの総数Z

	computeParametersMap_->kernelSize = kernelSize; // カーネルサイズ
	computeParametersMap_->sigma = sigma; // 標準偏差

	// バッファを送る

	// 定数パラメータ
	commandList_->SetComputeRootConstantBufferView(0, computeParametersBuff_->GetGPUVirtualAddress());
	// ガウスブラーを掛ける画像をセット
	commandList_->SetComputeRootDescriptorTable(1, gaussianBluGPUHandle);
	// 編集する画像セット
	editTextures_[editTextureIndex]->SetRootDescriptorTable(commandList_, 3);

	// ディスパッチ数
	uint32_t x = (kTextureWidth + kNumThreadX - 1) / kNumThreadX;
	uint32_t y = (kTextureHeight + kNumThreadY - 1) / kNumThreadY;
	uint32_t z = 1;

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

	// ソースR
	D3D12_DESCRIPTOR_RANGE souceReal[1] = {};
	souceReal[0].BaseShaderRegister = 0;//0から始まる
	souceReal[0].NumDescriptors = 1;//数は一つ
	souceReal[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;//UAVを使う
	souceReal[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;//Offsetを自動計算

	// ソースI
	D3D12_DESCRIPTOR_RANGE souceImaginary[1] = {};
	souceImaginary[0].BaseShaderRegister = 1;//0から始まる
	souceImaginary[0].NumDescriptors = 1;//数は一つ
	souceImaginary[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;//UAVを使う
	souceImaginary[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;//Offsetを自動計算

	// 行き先R0
	D3D12_DESCRIPTOR_RANGE destinationReal0[1] = {};
	destinationReal0[0].BaseShaderRegister = 0;//0から始まる
	destinationReal0[0].NumDescriptors = 1;//数は一つ
	destinationReal0[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_UAV;//UAVを使う
	destinationReal0[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;//Offsetを自動計算


	// 行き先I0
	D3D12_DESCRIPTOR_RANGE destinationImaginary0[1] = {};
	destinationImaginary0[0].BaseShaderRegister = 1;//0から始まる
	destinationImaginary0[0].NumDescriptors = 1;//数は一つ
	destinationImaginary0[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_UAV;//UAVを使う
	destinationImaginary0[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;//Offsetを自動計算


	// 行き先R1
	D3D12_DESCRIPTOR_RANGE destinationReal1[1] = {};
	destinationReal1[0].BaseShaderRegister = 2;//0から始まる
	destinationReal1[0].NumDescriptors = 1;//数は一つ
	destinationReal1[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_UAV;//UAVを使う
	destinationReal1[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;//Offsetを自動計算


	// 行き先I1
	D3D12_DESCRIPTOR_RANGE destinationImaginary1[1] = {};
	destinationImaginary1[0].BaseShaderRegister = 3;//0から始まる
	destinationImaginary1[0].NumDescriptors = 1;//数は一つ
	destinationImaginary1[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_UAV;//UAVを使う
	destinationImaginary1[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;//Offsetを自動計算


	// ルートパラメータ
	D3D12_ROOT_PARAMETER rootParameters[7] = {};
	// 定数バッファ
	rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;   //CBVを使う
	rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL; //ALLで使う
	rootParameters[0].Descriptor.ShaderRegister = 0;                  //レジスタ番号0とバインド

	// 元画像テクスチャReal
	rootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;//DescriptorTableを使う
	rootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;//全てで使う
	rootParameters[1].DescriptorTable.pDescriptorRanges = souceReal;//Tableの中身の配列を指定
	rootParameters[1].DescriptorTable.NumDescriptorRanges = _countof(souceReal);//Tableで利用する数

	// 元画像テクスチャImaginary
	rootParameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;//DescriptorTableを使う
	rootParameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;//全てで使う
	rootParameters[2].DescriptorTable.pDescriptorRanges = souceImaginary;//Tableの中身の配列を指定
	rootParameters[2].DescriptorTable.NumDescriptorRanges = _countof(souceImaginary);//Tableで利用する数

	// 行先画像テクスチャReal0
	rootParameters[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;//DescriptorTableを使う
	rootParameters[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;//全てで使う
	rootParameters[3].DescriptorTable.pDescriptorRanges = destinationReal0;//Tableの中身の配列を指定
	rootParameters[3].DescriptorTable.NumDescriptorRanges = _countof(destinationReal0);//Tableで利用する数

	// 行先画像テクスチャImaginary0
	rootParameters[4].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;//DescriptorTableを使う
	rootParameters[4].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;//全てで使う
	rootParameters[4].DescriptorTable.pDescriptorRanges = destinationImaginary0;//Tableの中身の配列を指定
	rootParameters[4].DescriptorTable.NumDescriptorRanges = _countof(destinationImaginary0);//Tableで利用する数

	// 行先画像テクスチャReal1
	rootParameters[5].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;//DescriptorTableを使う
	rootParameters[5].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;//全てで使う
	rootParameters[5].DescriptorTable.pDescriptorRanges = destinationReal1;//Tableの中身の配列を指定
	rootParameters[5].DescriptorTable.NumDescriptorRanges = _countof(destinationReal1);//Tableで利用する数

	// 行先画像テクスチャImaginary1
	rootParameters[6].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;//DescriptorTableを使う
	rootParameters[6].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;//全てで使う
	rootParameters[6].DescriptorTable.pDescriptorRanges = destinationImaginary1;//Tableの中身の配列を指定
	rootParameters[6].DescriptorTable.NumDescriptorRanges = _countof(destinationImaginary1);//Tableで利用する数

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
