#include "Glare.h"
#include "../base/WinApp.h"
#include "../base/BufferResource.h"
#include "../base/Log.h"
#include "../base/CompileShader.h"
#include "../base/TextureManager.h"
#include <fstream>
#include <cmath>

void Glare::Initialize(const std::array<uint32_t, kImageForGlareIndexOfCount>& imageForGlareHandles)
{

	// デバイス取得
	device_ = DirectXCommon::GetInstance()->GetDevice();

	// 定数バッファ作成
	computeParametersBuff_ = BufferResource::CreateBufferResource(device_, ((sizeof(ComputeParameters) + 0xff) & ~0xff));
	computeParametersBuff_->Map(0, nullptr, reinterpret_cast<void**>(&computeParametersMap_));

	// 定数バッファに渡す値の設定
	computeParametersMap_->lamdaR = static_cast<float>(633e-9); // ラムダR
	computeParametersMap_->lamdaG = static_cast<float>(532e-9); // ラムダG
	computeParametersMap_->lamdaB = static_cast<float>(466e-9); // ラムダB
	computeParametersMap_->glareIntensity = 0.8f; // グレア強度
	computeParametersMap_->threshold = 0.8f; // しきい値

	// ルートシグネチャ
	CreateRootSignature();

	//シェーダー
	CompileShader();

	// パイプライン
	CreatePipline();

	// レンダーターゲット初期化
	for (uint32_t i = 0; i < 8; ++i) {
		editTextures_[i] = std::make_unique<TextureUAV>();
		editTextures_[i]->Initialize(
			device_,
			kWidth,
			kHeight);
	}

	// 最大値最小値計算で使うテクスチャ
	maxMinTextures_[0] = std::make_unique<TextureUAV>();
	maxMinTextures_[0]->Initialize(
		device_,
		kWidth,
		kHeight);
	maxMinTextures_[1] = std::make_unique<TextureUAV>();
	maxMinTextures_[1]->Initialize(
		device_,
		kWidth,
		kHeight);
	lineInnerTexture_ = std::make_unique<TextureUAV>();
	lineInnerTexture_->Initialize(
		device_,
		kWidth,
		kHeight);

	// FFTで使うテクスチャ
	fftInnerTextures_[0] = std::make_unique<TextureUAV>();
	fftInnerTextures_[0]->Initialize(
		device_,
		kWidth,
		kHeight);
	fftInnerTextures_[1] = std::make_unique<TextureUAV>();
	fftInnerTextures_[1]->Initialize(
		device_,
		kWidth,
		kHeight);

	// グレア用画像
	imageForGlareHandles_ = imageForGlareHandles;

}

void Glare::Execution(
	const CD3DX12_GPU_DESCRIPTOR_HANDLE& imageWithGlareHandle,
	float glareIntensity,
	float threshold,
	ImageForGlareIndex imageForGlareIndex,
	ID3D12GraphicsCommandList* commandList)
{
	
	// リスト
	commandList_ = commandList;

	// バリア
	for (uint32_t i = 0; i < 8; ++i) {
		editTextures_[i]->Barrier(commandList_);
	}

	// 定数バッファを更新
	computeParametersMap_->glareIntensity = glareIntensity;
	computeParametersMap_->threshold = threshold;

	// グレアをかける画像をコピー
	CopyCommand(imageWithGlareHandle, editTextures_[0].get());

	// コピーした画像で2値化画像を作る
	BinaryThresholdCommand(editTextures_[0].get(), editTextures_[1].get());
	// 2番テクスチャをクリア
	ClearCommand(editTextures_[2].get());

	return;

	// グレアを作る画像を決定
	assert(imageForGlareIndex < kImageForGlareIndexOfCount);
	CopyCommand(imageForGlareHandles_[imageForGlareIndex], editTextures_[3].get());

	// 4番テクスチャをクリア
	ClearCommand(editTextures_[4].get());

	// FFT(グレア生成)
	FFTCommand(editTextures_[3].get(), editTextures_[4].get());

	// 増幅
	AmpCommand(editTextures_[3].get(), editTextures_[4].get(),
		editTextures_[5].get(), editTextures_[6].get());

	// 最大値最小値計算
	CalcMaxMinCommand(editTextures_[5].get(),
		maxMinTextures_[0].get(), maxMinTextures_[1].get());

	// 最大振幅による除算
	DivideMaxAmpCommand(maxMinTextures_[0].get(), maxMinTextures_[1].get(),
		editTextures_[5].get(), editTextures_[6].get(), 
		editTextures_[3].get(), editTextures_[4].get());

	// グレアの高度を上げる
	RaiseRICommand(editTextures_[3].get(), editTextures_[4].get(),
		editTextures_[5].get(), editTextures_[6].get());

	// 波長スケーリング
	SpectrumScalingCommand(editTextures_[5].get(), editTextures_[6].get(),
		editTextures_[3].get(), editTextures_[4].get());

	// ここまででグレア画像を作成（writeTextures_[3]）

	// 二値化画像とグレア画像を畳み込む
	ConvolutionCommand(
		editTextures_[1].get(), editTextures_[2].get(),
		editTextures_[3].get(), editTextures_[4].get(),
		editTextures_[5].get(), editTextures_[6].get());

	// 増幅
	AmpCommand(editTextures_[5].get(), editTextures_[6].get(),
		editTextures_[0].get(), editTextures_[1].get());

	// 最大値最小値計算
	CalcMaxMinCommand(editTextures_[0].get(),
		maxMinTextures_[0].get(), maxMinTextures_[1].get());

	// 最大振幅による除算
	DivideMaxAmpCommand(maxMinTextures_[0].get(), maxMinTextures_[1].get(),
		editTextures_[5].get(), editTextures_[6].get(),
		editTextures_[3].get(), editTextures_[4].get());

	// グレアをかける画像をコピー
	CopyCommand(imageWithGlareHandle, editTextures_[0].get());

	// 元画像に作成した画像を加算する
	AddCommand(editTextures_[0].get(),
		editTextures_[3].get(), editTextures_[1].get());

	// 画像完成（writeTextures_[1]）

}

void Glare::CreateRootSignature()
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

void Glare::CompileShader()
{

	for (uint32_t i = 0; i < kPiolineIndexOfCount; ++i) {

		if (i == kPiolineIndexFFTROWCS) {
			CreateHeaderHLSL(
				kWidth, kHeight,
				true, false);
		}
		else if (i == kPiolineIndexFFTCOLCS){
			CreateHeaderHLSL(
				kWidth, kHeight,
				false, false);
		}
		else if (i == kPiolineIndexIFFTROWCS) {
			CreateHeaderHLSL(
				kWidth, kHeight,
				true, true);
		}
		else {
			CreateHeaderHLSL(
				kWidth, kWidth,
				false, true);
		}

		shaders_[i] = CompileShader::Compile(
			shaderNames_[i].first,
			L"cs_6_0", 
			shaderNames_[i].second);

	}

}

void Glare::CreatePipline()
{

	for (uint32_t i = 0; i < kPiolineIndexOfCount; ++i) {

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

void Glare::CreateHeaderHLSL(uint32_t width, uint32_t height, bool row, bool inv)
{

	std::ofstream file("Resources/shaders/Glare.hlsl");

	assert(file);

	file << "#define" << " " << "WIDTH" << " " << width << "\n";
	file << "#define" << " " << "HEIGHT" << " " << height << "\n";
	file << "#define" << " " << "PI" << " " << 3.14159265 << "\n";
	file << "#define" << " " << "RAD" << " " << "PI / 180.0" << "\n";

	if (row) {
		file << "#define" << " " << "ROW" << " " << "1" << "\n";
		file << "#define" << " " << "LENGTH" << " " << "WIDTH" << "\n";
		file << "#define" << " " << "BUTTERFLY_COUNT" << " " << Inv2Pow(width) << "\n";
	}
	else {
		file << "#define" << " " << "ROW" << " " << "0" << "\n";
		file << "#define" << " " << "LENGTH" << " " << "HEIGHT" << "\n";
		file << "#define" << " " << "BUTTERFLY_COUNT" << " " << Inv2Pow(height) << "\n";
	}


	if (inv) {
		file << "#define" << " " << "INVESE" << " " << "1" << "\n";
	}
	else {
		file << "#define" << " " << "INVESE" << " " << "0" << "\n";
	}

	file.close();

}

int Glare::Inv2Pow(int n)
{
	
	int m = 0;
	m = static_cast<int>(std::log(static_cast<double>(n)) / std::log(2.0));
	return m;

}


void Glare::CopyCommand(const CD3DX12_GPU_DESCRIPTOR_HANDLE& in, TextureUAV* out)
{

	assert(commandList_);

	// パイプライン
	commandList_->SetComputeRootSignature(rootSignature_.Get());
	commandList_->SetPipelineState(pipelineStates_[kPiolineIndexCopyCS].Get());
	// バッファを送る
	commandList_->SetComputeRootConstantBufferView(0, computeParametersBuff_->GetGPUVirtualAddress());
	commandList_->SetComputeRootDescriptorTable(1, in);
	out->SetRootDescriptorTable(commandList_, 3);

	// 実行
	commandList_->Dispatch(1, kHeight, 1);

}

void Glare::CopyCommand(uint32_t in, TextureUAV* out)
{

	assert(commandList_);

	// バッファを送る
	commandList_->SetComputeRootConstantBufferView(0, computeParametersBuff_->GetGPUVirtualAddress());
	TextureManager::GetInstance()->SetGraphicsRootDescriptorTable(commandList_, 1, in);
	out->SetRootDescriptorTable(commandList_, 3);

	// パイプライン
	commandList_->SetPipelineState(pipelineStates_[kPiolineIndexCopyCS].Get());
	// 実行
	commandList_->Dispatch(1, kHeight, 1);

}

void Glare::BinaryThresholdCommand(TextureUAV* in, TextureUAV* out)
{

	assert(commandList_);

	// バッファを送る
	commandList_->SetComputeRootConstantBufferView(0, computeParametersBuff_->GetGPUVirtualAddress());
	in->SetRootDescriptorTable(commandList_, 1);
	out->SetRootDescriptorTable(commandList_, 3);
	// パイプライン
	commandList_->SetPipelineState(pipelineStates_[kPiolineIndexBinaryThresholdCS].Get());
	// 実行
	commandList_->Dispatch(1, kHeight, 1);

}

void Glare::ClearCommand(TextureUAV* tex)
{

	assert(commandList_);

	// バッファを送る
	commandList_->SetComputeRootConstantBufferView(0, computeParametersBuff_->GetGPUVirtualAddress());
	tex->SetRootDescriptorTable(commandList_, 3);
	// パイプライン
	commandList_->SetPipelineState(pipelineStates_[kPiolineIndexClesrCS].Get());

	// 実行
	commandList_->Dispatch(1, kHeight, 1);

}

void Glare::FFTCommand(TextureUAV* real, TextureUAV* image)
{

	assert(commandList_);

	// 縦方向
	// バッファを送る
	commandList_->SetComputeRootConstantBufferView(0, computeParametersBuff_->GetGPUVirtualAddress());
	real->SetRootDescriptorTable(commandList_, 1);
	image->SetRootDescriptorTable(commandList_, 2);
	editTextures_[2]->SetRootDescriptorTable(commandList_, 3);
	editTextures_[3]->SetRootDescriptorTable(commandList_, 4);
	// パイプライン
	commandList_->SetPipelineState(pipelineStates_[kPiolineIndexFFTROWCS].Get());
	// 実行
	commandList_->Dispatch(1, kHeight, 1);

	// 横方向
	// バッファを送る
	commandList_->SetComputeRootConstantBufferView(0, computeParametersBuff_->GetGPUVirtualAddress());
	editTextures_[2]->SetRootDescriptorTable(commandList_, 1);
	editTextures_[3]->SetRootDescriptorTable(commandList_, 2);
	real->SetRootDescriptorTable(commandList_, 3);
	image->SetRootDescriptorTable(commandList_, 4);
	// パイプライン
	commandList_->SetPipelineState(pipelineStates_[kPiolineIndexFFTCOLCS].Get());
	// 実行
	commandList_->Dispatch(1, kWidth, 1);

}

void Glare::IFFTCommand(TextureUAV* real, TextureUAV* image)
{

	assert(commandList_);

	// 縦方向
	// バッファを送る
	commandList_->SetComputeRootConstantBufferView(0, computeParametersBuff_->GetGPUVirtualAddress());
	real->SetRootDescriptorTable(commandList_, 1);
	image->SetRootDescriptorTable(commandList_, 2);
	fftInnerTextures_[0]->SetRootDescriptorTable(commandList_, 3);
	fftInnerTextures_[1]->SetRootDescriptorTable(commandList_, 4);
	// パイプライン
	commandList_->SetPipelineState(pipelineStates_[kPiolineIndexIFFTROWCS].Get());
	// 実行
	commandList_->Dispatch(1, kHeight, 1);

	// 横方向
	// バッファを送る
	commandList_->SetComputeRootConstantBufferView(0, computeParametersBuff_->GetGPUVirtualAddress());
	fftInnerTextures_[0]->SetRootDescriptorTable(commandList_, 1);
	fftInnerTextures_[1]->SetRootDescriptorTable(commandList_, 2);
	real->SetRootDescriptorTable(commandList_, 3);
	image->SetRootDescriptorTable(commandList_, 4);
	// パイプライン
	commandList_->SetPipelineState(pipelineStates_[kPiolineIndexIFFTCOLCS].Get());
	// 実行
	commandList_->Dispatch(1, kWidth, 1);


}

void Glare::AmpCommand(TextureUAV* inReal, TextureUAV* inImage, TextureUAV* outReal, TextureUAV* outImage)
{

	assert(commandList_);
	// バッファを送る
	commandList_->SetComputeRootConstantBufferView(0, computeParametersBuff_->GetGPUVirtualAddress());
	inReal->SetRootDescriptorTable(commandList_, 1);
	inImage->SetRootDescriptorTable(commandList_, 2);
	outReal->SetRootDescriptorTable(commandList_, 3);
	outImage->SetRootDescriptorTable(commandList_, 4);
	// パイプライン
	commandList_->SetPipelineState(pipelineStates_[kPiolineIndexAmpCS].Get());
	// 実行
	commandList_->Dispatch(1, kHeight, 1);

}

void Glare::CalcMaxMinCommand(TextureUAV* tex, TextureUAV* outOnePixRealMax, TextureUAV* outOnePixImageMin)
{

	assert(commandList_);
	// バッファを送る
	commandList_->SetComputeRootConstantBufferView(0, computeParametersBuff_->GetGPUVirtualAddress());
	tex->SetRootDescriptorTable(commandList_, 1);
	lineInnerTexture_->SetRootDescriptorTable(commandList_, 3);
	// パイプライン
	commandList_->SetPipelineState(pipelineStates_[kPiolineIndexMaxMinFirstCS].Get());
	// 実行
	commandList_->Dispatch(kWidth, 1, 1);
	// バッファを送る
	lineInnerTexture_->SetRootDescriptorTable(commandList_, 1);
	outOnePixRealMax->SetRootDescriptorTable(commandList_, 3);
	outOnePixImageMin->SetRootDescriptorTable(commandList_, 4);
	// パイプライン
	commandList_->SetPipelineState(pipelineStates_[kPiolineIndexMaxMinSecondCS].Get());
	// 実行
	commandList_->Dispatch(1, 1, 1);

}

void Glare::DivideMaxAmpCommand(TextureUAV* outOnePixRealMax, TextureUAV* outOnePixImageMin, TextureUAV* inReal, TextureUAV* inImage, TextureUAV* outReal, TextureUAV* outImage)
{

	assert(commandList_);
	// バッファを送る
	commandList_->SetComputeRootConstantBufferView(0, computeParametersBuff_->GetGPUVirtualAddress());
	outOnePixRealMax->SetRootDescriptorTable(commandList_, 1);
	outOnePixImageMin->SetRootDescriptorTable(commandList_, 2);
	inReal->SetRootDescriptorTable(commandList_, 3);
	inImage->SetRootDescriptorTable(commandList_, 4);
	outReal->SetRootDescriptorTable(commandList_, 5);
	outImage->SetRootDescriptorTable(commandList_, 6);
	// パイプライン
	commandList_->SetPipelineState(pipelineStates_[kPiolineIndexDivByMaxAampCS].Get());
	// 実行
	commandList_->Dispatch(1, kHeight, 1);

}

void Glare::RaiseRICommand(TextureUAV* inReal, TextureUAV* inImage, TextureUAV* outReal, TextureUAV* outImage)
{

	assert(commandList_);
	// バッファを送る
	commandList_->SetComputeRootConstantBufferView(0, computeParametersBuff_->GetGPUVirtualAddress());
	inReal->SetRootDescriptorTable(commandList_, 1);
	inImage->SetRootDescriptorTable(commandList_, 2);
	outReal->SetRootDescriptorTable(commandList_, 3);
	outImage->SetRootDescriptorTable(commandList_, 4);
	// パイプライン
	commandList_->SetPipelineState(pipelineStates_[kPiolineIndexRaiseRealImageCS].Get());
	// 実行
	commandList_->Dispatch(1, kHeight, 1);

}

void Glare::SpectrumScalingCommand(TextureUAV* inReal, TextureUAV* inImage, TextureUAV* outReal, TextureUAV* outImage)
{

	assert(commandList_);
	// バッファを送る
	commandList_->SetComputeRootConstantBufferView(0, computeParametersBuff_->GetGPUVirtualAddress());
	inReal->SetRootDescriptorTable(commandList_, 1);
	inImage->SetRootDescriptorTable(commandList_, 2);
	outReal->SetRootDescriptorTable(commandList_, 3);
	outImage->SetRootDescriptorTable(commandList_, 4);
	// パイプライン
	commandList_->SetPipelineState(pipelineStates_[kPiolineIndexSpectrumScalingCS].Get());
	// 実行
	commandList_->Dispatch(1, kHeight, 1);

}

void Glare::ConvolutionCommand(TextureUAV* inReal0, TextureUAV* inImage0, TextureUAV* inReal1, TextureUAV* inImage1, TextureUAV* outReal, TextureUAV* outImage)
{

	// 一方のFFT
	FFTCommand(inReal0, inImage0);
	// もう一方のFFT
	FFTCommand(inReal1, inImage1);
	// 乗算
	MultiplyCommand(inReal0, inImage0, inReal1, inImage1, outReal, outImage);
	// 逆FFT
	IFFTCommand(outReal, outImage);

}

void Glare::MultiplyCommand(TextureUAV* inReal0, TextureUAV* inImage0, TextureUAV* inReal1, TextureUAV* inImage1, TextureUAV* outReal, TextureUAV* outImage)
{

	assert(commandList_);
	// バッファを送る
	commandList_->SetComputeRootConstantBufferView(0, computeParametersBuff_->GetGPUVirtualAddress());
	inReal0->SetRootDescriptorTable(commandList_, 1);
	inImage0->SetRootDescriptorTable(commandList_, 2);
	inReal1->SetRootDescriptorTable(commandList_, 3);
	inImage1->SetRootDescriptorTable(commandList_, 4);
	outReal->SetRootDescriptorTable(commandList_, 5);
	outImage->SetRootDescriptorTable(commandList_, 6);
	// パイプライン
	commandList_->SetPipelineState(pipelineStates_[kPiolineIndexMulCS].Get());
	// 実行
	commandList_->Dispatch(1, kHeight, 1);

}

void Glare::AddCommand(TextureUAV* tex0, TextureUAV* tex1, TextureUAV* out)
{

	assert(commandList_);
	// バッファを送る
	commandList_->SetComputeRootConstantBufferView(0, computeParametersBuff_->GetGPUVirtualAddress());
	tex0->SetRootDescriptorTable(commandList_, 1);
	tex1->SetRootDescriptorTable(commandList_, 2);
	out->SetRootDescriptorTable(commandList_, 3);
	// パイプライン
	commandList_->SetPipelineState(pipelineStates_[kPiolineIndexAddCS].Get());
	// 実行
	commandList_->Dispatch(1, kHeight, 1);

}
