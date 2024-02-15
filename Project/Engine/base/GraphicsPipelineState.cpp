#include "GraphicsPipelineState.h"

using namespace Microsoft::WRL;

// ルートシグネチャ
Microsoft::WRL::ComPtr<ID3D12RootSignature> GraphicsPipelineState::sRootSignature[GraphicsPipelineState::PipelineStateName::kPipelineStateNameOfCount];
// パイプラインステートオブジェクト
Microsoft::WRL::ComPtr<ID3D12PipelineState> GraphicsPipelineState::sPipelineState[GraphicsPipelineState::PipelineStateName::kPipelineStateNameOfCount];
// ルートパラメータ
std::array<std::vector<D3D12_ROOT_PARAMETER>, GraphicsPipelineState::kRootParameterIndexOfCount> GraphicsPipelineState::rootParameters_;
// サンプラー
std::array<std::vector<D3D12_STATIC_SAMPLER_DESC>, GraphicsPipelineState::kSamplerIndexOfCount> GraphicsPipelineState::samplerDescs_;
// インプットレイアウト
std::array<D3D12_INPUT_LAYOUT_DESC, GraphicsPipelineState::kInputLayoutIndexOfCount> GraphicsPipelineState::inputLayoutDescs_;
// ブレンド
std::array<D3D12_BLEND_DESC, GraphicsPipelineState::kBlendStateIndexOfCount> GraphicsPipelineState::blendDescs_;
// デバイス
ID3D12Device* GraphicsPipelineState::sDevice_;
// シェーダーコンパイル用
GraphicsPipelineState::CompileShaderStruct GraphicsPipelineState::compileShaderStruct_;
// ディスクリプタレンジ
std::array<std::vector<D3D12_DESCRIPTOR_RANGE>, GraphicsPipelineState::kDescriptorRangeIndexOfCount> GraphicsPipelineState::descriptorRanges_;

std::array< std::vector<D3D12_INPUT_ELEMENT_DESC>, GraphicsPipelineState::kInputLayoutIndexOfCount> GraphicsPipelineState::inputElementDescs_;

void GraphicsPipelineState::Initialize(ID3D12Device* sDevice)
{

	// デバイス
	sDevice_ = sDevice;
	// dxcCompilerを初期化
	DxcCompilerInitialize();

	DescriptorRangeInitialize();

	RootParameterInitialize();

	SamplerInitialize();

	InputLayoutInitialize();

	BlendStateInitialize();

	// パイプラインステート作る

	CreateForModel();

	CreateForSprite();

	CreateForParticle();

	CreateForOutLine();
}

void GraphicsPipelineState::CreateForModel()
{

	CreatePSODesc createPSODesc;

	RootsignatureSetting(
		kPipelineStateNameModel,
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT,
		kRootParameterIndexModel,
		kSamplerIndexNormal);

	createPSODesc.pipelineStateName = kPipelineStateNameModel;

	createPSODesc.depthStencilState = DepthStencilStateSetting(
		true,
		D3D12_DEPTH_WRITE_MASK_ALL,
		D3D12_COMPARISON_FUNC_LESS_EQUAL);

	createPSODesc.inputLayoutDesc = InputLayoutSetting(
		kInputLayoutIndexNormal);

	createPSODesc.blendDesc = BlendStateSetting(kBlendStateIndexNormal);

	createPSODesc.rasterizerDesc = ResiterzerStateSetting(D3D12_CULL_MODE_BACK, D3D12_FILL_MODE_SOLID);

	createPSODesc.vertexShaderBlob = CompileShader(L"Resources/shaders/Object3d.vs.hlsl",
		L"vs_6_0");
	createPSODesc.pixelShaderBlob = CompileShader(L"Resources/shaders/Object3d.PS.hlsl",
		L"ps_6_0");

	//書き込むRTVの情報
	createPSODesc.numRenderTargets = 1;
	createPSODesc.RTVFormats = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	//利用するトポロジ(形状)のタイプ。
	createPSODesc.primitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

	//どのように画面に色を打ち込むのかの設定
	createPSODesc.sampleDescCount = 1;
	createPSODesc.sampleMask = D3D12_DEFAULT_SAMPLE_MASK;

	//DepthStencilの設定
	createPSODesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;

	CreatePSO(createPSODesc);

}

void GraphicsPipelineState::CreateForSprite()
{

	CreatePSODesc createPSODesc;

	RootsignatureSetting(
		kPipelineStateNameSprite,
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT,
		kRootParameterIndexSprite,
		kSamplerIndexNormal);

	createPSODesc.pipelineStateName = kPipelineStateNameSprite;

	createPSODesc.depthStencilState = DepthStencilStateSetting(
		true,
		D3D12_DEPTH_WRITE_MASK_ALL,
		D3D12_COMPARISON_FUNC_LESS_EQUAL);

	createPSODesc.inputLayoutDesc = InputLayoutSetting(
		kInputLayoutIndexNormal);

	createPSODesc.blendDesc = BlendStateSetting(kBlendStateIndexNormal);

	createPSODesc.rasterizerDesc = ResiterzerStateSetting(D3D12_CULL_MODE_NONE, D3D12_FILL_MODE_SOLID);

	createPSODesc.vertexShaderBlob = CompileShader(L"Resources/shaders/Sprite.vs.hlsl",
		L"vs_6_0");
	createPSODesc.pixelShaderBlob = CompileShader(L"Resources/shaders/Sprite.PS.hlsl",
		L"ps_6_0");

	//書き込むRTVの情報
	createPSODesc.numRenderTargets = 1;
	createPSODesc.RTVFormats = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	//利用するトポロジ(形状)のタイプ。
	createPSODesc.primitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

	//どのように画面に色を打ち込むのかの設定
	createPSODesc.sampleDescCount = 1;
	createPSODesc.sampleMask = D3D12_DEFAULT_SAMPLE_MASK;

	//DepthStencilの設定
	createPSODesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;

	CreatePSO(createPSODesc);

}

void GraphicsPipelineState::CreateForParticle()
{

	CreatePSODesc createPSODesc;

	RootsignatureSetting(
		kPipelineStateNameParticle,
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT,
		kRootParameterIndexParticle,
		kSamplerIndexNormal);

	createPSODesc.pipelineStateName = kPipelineStateNameParticle;

	createPSODesc.depthStencilState = DepthStencilStateSetting(
		true,
		D3D12_DEPTH_WRITE_MASK_ZERO,
		D3D12_COMPARISON_FUNC_LESS_EQUAL);

	createPSODesc.inputLayoutDesc = InputLayoutSetting(
		kInputLayoutIndexNormal);

	createPSODesc.blendDesc = BlendStateSetting(kBlendStateIndexNormal);

	createPSODesc.rasterizerDesc = ResiterzerStateSetting(D3D12_CULL_MODE_BACK, D3D12_FILL_MODE_SOLID);

	createPSODesc.vertexShaderBlob = CompileShader(L"Resources/shaders/Particle.vs.hlsl",
		L"vs_6_0");
	createPSODesc.pixelShaderBlob = CompileShader(L"Resources/shaders/Particle.PS.hlsl",
		L"ps_6_0");

	//書き込むRTVの情報
	createPSODesc.numRenderTargets = 1;
	createPSODesc.RTVFormats = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	//利用するトポロジ(形状)のタイプ。
	createPSODesc.primitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

	//どのように画面に色を打ち込むのかの設定
	createPSODesc.sampleDescCount = 1;
	createPSODesc.sampleMask = D3D12_DEFAULT_SAMPLE_MASK;

	//DepthStencilの設定
	createPSODesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;

	CreatePSO(createPSODesc);

}

void GraphicsPipelineState::CreateForOutLine()
{

	CreatePSODesc createPSODesc;

	RootsignatureSetting(
		kPipelineStateNameOutLine,
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT,
		kRootParameterIndexOutLine,
		kSamplerIndexNormal);

	createPSODesc.pipelineStateName = kPipelineStateNameOutLine;

	createPSODesc.depthStencilState = DepthStencilStateSetting(
		true,
		D3D12_DEPTH_WRITE_MASK_ALL,
		D3D12_COMPARISON_FUNC_LESS_EQUAL);

	createPSODesc.inputLayoutDesc = InputLayoutSetting(
		kInputLayoutIndexNormal);

	createPSODesc.blendDesc = BlendStateSetting(kBlendStateIndexNormal);

	createPSODesc.rasterizerDesc = ResiterzerStateSetting(D3D12_CULL_MODE_FRONT, D3D12_FILL_MODE_SOLID);

	createPSODesc.vertexShaderBlob = CompileShader(L"Resources/shaders/OutLine3D.vs.hlsl",
		L"vs_6_0");
	createPSODesc.pixelShaderBlob = CompileShader(L"Resources/shaders/OutLine3D.PS.hlsl",
		L"ps_6_0");

	//書き込むRTVの情報
	createPSODesc.numRenderTargets = 1;
	createPSODesc.RTVFormats = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	//利用するトポロジ(形状)のタイプ。
	createPSODesc.primitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

	//どのように画面に色を打ち込むのかの設定
	createPSODesc.sampleDescCount = 1;
	createPSODesc.sampleMask = D3D12_DEFAULT_SAMPLE_MASK;

	//DepthStencilの設定
	createPSODesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;

	CreatePSO(createPSODesc);

}

void GraphicsPipelineState::RootParameterInitialize()
{

	// モデル
	RootParameterInitializeForModel();
	// スプライト
	RootParameterInitializeForSprite();
	// パーティクル
	RootParameterInitializeForParticle();
	// アウトライン
	RootParameterInitializeForOutLine();

}

void GraphicsPipelineState::RootParameterInitializeForModel()
{

	//RootParameter作成
	D3D12_ROOT_PARAMETER rootParameters[5] = {};
	rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;   //CBVを使う
	rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;//PixelShaderで使う
	rootParameters[0].Descriptor.ShaderRegister = 0;                   //レジスタ番号0とバインド
	rootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;   //CBVを使う
	rootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;//VertexShaderで使う
	rootParameters[1].Descriptor.ShaderRegister = 0;                   //レジスタ番号0とバインド
	rootParameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;//DescriptorTableを使う
	rootParameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;//PixelShaderで使う
	rootParameters[2].DescriptorTable.pDescriptorRanges = descriptorRanges_[kDescriptorRangeIndexTexture].data();//Tableの中身の配列を指定
	rootParameters[2].DescriptorTable.NumDescriptorRanges = static_cast<uint32_t>(descriptorRanges_[kDescriptorRangeIndexTexture].size());//Tableで利用する数
	rootParameters[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;   //CBVを使う
	rootParameters[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;//PixelShaderで使う
	rootParameters[3].Descriptor.ShaderRegister = 1;
	rootParameters[4].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;   //CBVを使う
	rootParameters[4].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;//PixelShaderで使う
	rootParameters[4].Descriptor.ShaderRegister = 2;

	for (uint32_t i = 0; i < 5; ++i) {
		rootParameters_[kRootParameterIndexModel].push_back(rootParameters[i]);
	}

}

void GraphicsPipelineState::RootParameterInitializeForSprite()
{

	//RootParameter作成
	D3D12_ROOT_PARAMETER rootParameters[4] = {};
	rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;   //CBVを使う
	rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;//PixelShaderで使う
	rootParameters[0].Descriptor.ShaderRegister = 0;                   //レジスタ番号0とバインド
	rootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;   //CBVを使う
	rootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;//VertexShaderで使う
	rootParameters[1].Descriptor.ShaderRegister = 0;                   //レジスタ番号0とバインド
	rootParameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;//DescriptorTableを使う
	rootParameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;//PixelShaderで使う
	rootParameters[2].DescriptorTable.pDescriptorRanges = descriptorRanges_[kDescriptorRangeIndexTexture].data();//Tableの中身の配列を指定
	rootParameters[2].DescriptorTable.NumDescriptorRanges = static_cast<uint32_t>(descriptorRanges_[kDescriptorRangeIndexTexture].size());//Tableで利用する数
	rootParameters[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;   //CBVを使う
	rootParameters[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;//VertexShaderで使う
	rootParameters[3].Descriptor.ShaderRegister = 1;

	for (uint32_t i = 0; i < 4; ++i) {
		rootParameters_[kRootParameterIndexSprite].push_back(rootParameters[i]);
	}

}

void GraphicsPipelineState::RootParameterInitializeForParticle()
{

	//RootSignature作成
	D3D12_ROOT_SIGNATURE_DESC descriptionRootsignature{};
	descriptionRootsignature.Flags =
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	//RootParameter作成。複数設定できるので配列。今回は1つだけなので長さ1の配列
	D3D12_ROOT_PARAMETER rootParameters[5] = {};
	rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;   //CBVを使う
	rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;//PixelShaderで使う
	rootParameters[0].Descriptor.ShaderRegister = 0;                   //レジスタ番号0とバインド
	rootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;//DescriptorTableを使う
	rootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;//VertexShaderで使う
	rootParameters[1].DescriptorTable.pDescriptorRanges = descriptorRanges_[kDescriptorRangeIndexInstancing].data();//Tableの中身の配列を指定
	rootParameters[1].DescriptorTable.NumDescriptorRanges = static_cast<uint32_t>(descriptorRanges_[kDescriptorRangeIndexInstancing].size());//Tableで利用する数
	rootParameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;//DescriptorTableを使う
	rootParameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;//PixelShaderで使う
	rootParameters[2].DescriptorTable.pDescriptorRanges = descriptorRanges_[kDescriptorRangeIndexTexture].data();//Tableの中身の配列を指定
	rootParameters[2].DescriptorTable.NumDescriptorRanges = static_cast<uint32_t>(descriptorRanges_[kDescriptorRangeIndexTexture].size());//Tableで利用する数
	rootParameters[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;   //CBVを使う
	rootParameters[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;//PixelShaderで使う
	rootParameters[3].Descriptor.ShaderRegister = 1;
	rootParameters[4].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;   //CBVを使う
	rootParameters[4].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;//VertexShaderで使う
	rootParameters[4].Descriptor.ShaderRegister = 0;

	for (uint32_t i = 0; i < 5; ++i) {
		rootParameters_[kRootParameterIndexParticle].push_back(rootParameters[i]);
	}

}

void GraphicsPipelineState::RootParameterInitializeForOutLine()
{

	//RootParameter作成。複数設定できるので配列。今回は1つだけなので長さ1の配列
	D3D12_ROOT_PARAMETER rootParameters[3] = {};
	//color
	rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;   //CBVを使う
	rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;//PixelShaderで使う
	rootParameters[0].Descriptor.ShaderRegister = 0;                   //レジスタ番号0とバインド
	//transform
	rootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;   //CBVを使う
	rootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;//PixelShaderで使う
	rootParameters[1].Descriptor.ShaderRegister = 0;                   //レジスタ番号0とバインド
	//lineWidth
	rootParameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;   //CBVを使う
	rootParameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;//PixelShaderで使う
	rootParameters[2].Descriptor.ShaderRegister = 1;                   //レジスタ番号0とバインド

	for (uint32_t i = 0; i < 3; ++i) {
		rootParameters_[kRootParameterIndexOutLine].push_back(rootParameters[i]);
	}

}

void GraphicsPipelineState::DescriptorRangeInitialize()
{

	D3D12_DESCRIPTOR_RANGE descriptorRange[1] = {};
	descriptorRange[0].BaseShaderRegister = 0;//0から始まる
	descriptorRange[0].NumDescriptors = 1;//数は一つ
	descriptorRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;//SRVを使う
	descriptorRange[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;//Offsetを自動計算

	descriptorRanges_[kDescriptorRangeIndexTexture].push_back(descriptorRange[0]);

	D3D12_DESCRIPTOR_RANGE descriptorRangeForInstancing[1] = {};
	descriptorRangeForInstancing[0].BaseShaderRegister = 0;//0から始まる
	descriptorRangeForInstancing[0].NumDescriptors = 1;//数は一つ
	descriptorRangeForInstancing[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;//SRVを使う
	descriptorRangeForInstancing[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;//Offsetを自動計算

	descriptorRanges_[kDescriptorRangeIndexInstancing].push_back(descriptorRangeForInstancing[0]);

}

void GraphicsPipelineState::SamplerInitialize()
{

	D3D12_STATIC_SAMPLER_DESC samplerDesc = {};
	samplerDesc.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	samplerDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	samplerDesc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	samplerDesc.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
	samplerDesc.MaxLOD = D3D12_FLOAT32_MAX;
	samplerDesc.ShaderRegister = 0;
	samplerDesc.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	samplerDescs_[kSamplerIndexNormal].push_back(samplerDesc);

}

void GraphicsPipelineState::InputLayoutInitialize()
{

	//InputLayout
	D3D12_INPUT_ELEMENT_DESC inputElementDescs[3] = {};
	inputElementDescs[0].SemanticName = "POSITION";
	inputElementDescs[0].SemanticIndex = 0;
	inputElementDescs[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	inputElementDescs[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	inputElementDescs[1].SemanticName = "TEXCOORD";
	inputElementDescs[1].SemanticIndex = 0;
	inputElementDescs[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	inputElementDescs[1].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	inputElementDescs[2].SemanticName = "NORMAL";
	inputElementDescs[2].SemanticIndex = 0;
	inputElementDescs[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	inputElementDescs[2].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	
	for (uint32_t i = 0; i < 3; i++) {
		inputElementDescs_[kInputLayoutIndexNormal].push_back(inputElementDescs[i]);
	}

	inputLayoutDescs_[kInputLayoutIndexNormal].pInputElementDescs = inputElementDescs_[kInputLayoutIndexNormal].data();
	inputLayoutDescs_[kInputLayoutIndexNormal].NumElements = static_cast<uint32_t>(inputElementDescs_[kInputLayoutIndexNormal].size());

}


void GraphicsPipelineState::BlendStateInitialize()
{

	//すべての色要素を書き込む
	blendDescs_[kBlendStateIndexNormal].RenderTarget[0].RenderTargetWriteMask =
		D3D12_COLOR_WRITE_ENABLE_ALL;
	blendDescs_[kBlendStateIndexNormal].RenderTarget[0].BlendEnable = TRUE;
	blendDescs_[kBlendStateIndexNormal].RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	blendDescs_[kBlendStateIndexNormal].RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	blendDescs_[kBlendStateIndexNormal].RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
	blendDescs_[kBlendStateIndexNormal].RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	blendDescs_[kBlendStateIndexNormal].RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	blendDescs_[kBlendStateIndexNormal].RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;

	// Addブレンド
	blendDescs_[kBlendStateIndexAdd].RenderTarget[0].RenderTargetWriteMask =
		D3D12_COLOR_WRITE_ENABLE_ALL;
	blendDescs_[kBlendStateIndexAdd].RenderTarget[0].BlendEnable = TRUE;
	blendDescs_[kBlendStateIndexAdd].RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	blendDescs_[kBlendStateIndexAdd].RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	blendDescs_[kBlendStateIndexAdd].RenderTarget[0].DestBlend = D3D12_BLEND_ONE;
	blendDescs_[kBlendStateIndexAdd].RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	blendDescs_[kBlendStateIndexAdd].RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	blendDescs_[kBlendStateIndexAdd].RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;

}

void GraphicsPipelineState::DxcCompilerInitialize()
{

	HRESULT hr;

	//dxcCompilerを初期化
	hr = DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(&compileShaderStruct_.dxcUtils));
	assert(SUCCEEDED(hr));
	hr = DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&compileShaderStruct_.dxcCompiler));
	assert(SUCCEEDED(hr));

	//現時点でincludeはしないが、includeに対応するための設定を行っておく
	hr = compileShaderStruct_.dxcUtils->CreateDefaultIncludeHandler(&compileShaderStruct_.includeHandler);
	assert(SUCCEEDED(hr));

}

void GraphicsPipelineState::RootsignatureSetting(PipelineStateName pipelineStateName, D3D12_ROOT_SIGNATURE_FLAGS rootsignatureFlags, RootParameterIndex rootParameterIndex, SamplerIndex samplerIndex)
{

	HRESULT hr;

	D3D12_ROOT_SIGNATURE_DESC descriptionRootsignature{};
	descriptionRootsignature.Flags = rootsignatureFlags;

	descriptionRootsignature.pParameters = rootParameters_[rootParameterIndex].data(); //ルートパラメータ配列へのポインタ
	descriptionRootsignature.NumParameters = static_cast<uint32_t>(rootParameters_[rootParameterIndex].size()); //配列の長さ

	descriptionRootsignature.pStaticSamplers = samplerDescs_[samplerIndex].data();
	descriptionRootsignature.NumStaticSamplers = static_cast<uint32_t>(samplerDescs_[samplerIndex].size());

	//シリアライズしてバイナリにする
	ID3DBlob* signatureBlob = nullptr;
	ID3DBlob* errorBlob = nullptr;
	hr = D3D12SerializeRootSignature(&descriptionRootsignature,
		D3D_ROOT_SIGNATURE_VERSION_1, &signatureBlob, &errorBlob);
	if (FAILED(hr)) {
		Log(reinterpret_cast<char*>(errorBlob->GetBufferPointer()));
		assert(false);
	}
	//バイナリを元に生成
	hr = sDevice_->CreateRootSignature(0, signatureBlob->GetBufferPointer(),
		signatureBlob->GetBufferSize(), IID_PPV_ARGS(&sRootSignature[pipelineStateName]));
	assert(SUCCEEDED(hr));

}

D3D12_DEPTH_STENCIL_DESC GraphicsPipelineState::DepthStencilStateSetting(bool depthEnable, D3D12_DEPTH_WRITE_MASK depthWriteMask, D3D12_COMPARISON_FUNC depthFunc)
{

	D3D12_DEPTH_STENCIL_DESC depthStencilDesc{};
	depthStencilDesc.DepthEnable = depthEnable;
	depthStencilDesc.DepthWriteMask = depthWriteMask;
	depthStencilDesc.DepthFunc = depthFunc;
	return depthStencilDesc;

}

D3D12_INPUT_LAYOUT_DESC GraphicsPipelineState::InputLayoutSetting(InputLayoutIndex inputLayoutIndex)
{
	return inputLayoutDescs_[inputLayoutIndex];

}

D3D12_BLEND_DESC GraphicsPipelineState::BlendStateSetting(BlendStateIndex blendStateIndex)
{

	return blendDescs_[blendStateIndex];

}

D3D12_RASTERIZER_DESC GraphicsPipelineState::ResiterzerStateSetting(D3D12_CULL_MODE cullMode, D3D12_FILL_MODE fillMode)
{
	
	D3D12_RASTERIZER_DESC rasterizerDesc{};
	rasterizerDesc.CullMode = cullMode;
	rasterizerDesc.FillMode = fillMode;
	return rasterizerDesc;

}

//CompileShader
IDxcBlob* GraphicsPipelineState::CompileShader(
	//CompilerするShanderファイルへのパス
	const std::wstring& filePath,
	//Compilenに使用するProfile
	const wchar_t* profile)
{
	//htslファイルを読む
	//これからシェーダーをコンパイルする旨をログに出す
	Log(ConvertString(std::format(L"Begin CompileShader, path:{}, profile:{}\n", filePath, profile)));
	//hlslファイルを読む
	IDxcBlobEncoding* shaderSource = nullptr;
	HRESULT hr = compileShaderStruct_.dxcUtils->LoadFile(filePath.c_str(), nullptr, &shaderSource);
	//読めなかった止める
	assert(SUCCEEDED(hr));
	//読み込んだファイルの内容を設定する
	DxcBuffer shaderSourceBuffer;
	shaderSourceBuffer.Ptr = shaderSource->GetBufferPointer();
	shaderSourceBuffer.Size = shaderSource->GetBufferSize();
	shaderSourceBuffer.Encoding = DXC_CP_UTF8; //UTF8の文字コードであることを通知

	//Compileする
	LPCWSTR arguments[] = {
		filePath.c_str(), //コンパイル対象のhlslファイル名
		L"-E",L"main",//エントリーポイントの指定,基本的にmain以外にはしない
		L"-T", profile, //ShaderProfileの設定
		L"-Zi", L"-Qembed_debug", //デバッグ用の情報を埋め込む
		L"-Od", //最適化を外しておく
		L"-Zpr", //メモリレイアウトは作成
	};
	//実際にShaderをコンパイルする
	IDxcResult* shaderResult = nullptr;
	hr = compileShaderStruct_.dxcCompiler->Compile(
		&shaderSourceBuffer,//読み込んだファイル
		arguments,//コンパイルオプション
		_countof(arguments),//コンパイルオプションの数
		compileShaderStruct_.includeHandler,//includeがふくまれた諸々
		IID_PPV_ARGS(&shaderResult)//コンパイル結果
	);
	//コンパイルエラーではなくdixが起動できないなど致命的な状況
	assert(SUCCEEDED(hr));

	//警告・エラーがでていないか確認する
	//警告・エラーが出てたらログに出して止める
	IDxcBlobUtf8* shaderError = nullptr;
	shaderResult->GetOutput(DXC_OUT_ERRORS, IID_PPV_ARGS(&shaderError), nullptr);
	if (shaderError != nullptr && shaderError->GetStringLength() != 0) {
		Log(shaderError->GetStringPointer());
		assert(false);
	}

	//compile結果を受け取って返す
	//コンパイル結果から実行用のバイナリ部分を取得
	IDxcBlob* shaderBlob = nullptr;
	hr = shaderResult->GetOutput(DXC_OUT_OBJECT, IID_PPV_ARGS(&shaderBlob), nullptr);
	assert(SUCCEEDED(hr));
	//成功したログを出す
	Log(ConvertString(std::format(L"Compile Succeeded, path:{}, profile:{}\n", filePath, profile)));
	//もう使わないリソースを解放
	shaderSource->Release();
	shaderResult->Release();
	//実行用のバイナリを返却
	return shaderBlob;

}

void GraphicsPipelineState::CreatePSO(const CreatePSODesc& createPSODesc)
{

	HRESULT hr;

	//PSOを生成
	D3D12_GRAPHICS_PIPELINE_STATE_DESC graphicsPipelineStateDesc{};
	graphicsPipelineStateDesc.pRootSignature = sRootSignature[createPSODesc.pipelineStateName].Get();//RootSignature
	graphicsPipelineStateDesc.InputLayout = createPSODesc.inputLayoutDesc;//InputLayout
	graphicsPipelineStateDesc.VS = { createPSODesc.vertexShaderBlob->GetBufferPointer(),
	createPSODesc.vertexShaderBlob->GetBufferSize() };//VertexShader
	graphicsPipelineStateDesc.PS = { createPSODesc.pixelShaderBlob->GetBufferPointer(),
	createPSODesc.pixelShaderBlob->GetBufferSize() };//PixelShader
	graphicsPipelineStateDesc.BlendState = createPSODesc.blendDesc;//BlendState
	graphicsPipelineStateDesc.RasterizerState = createPSODesc.rasterizerDesc;//RasterizerState

	//書き込むRTVの情報
	graphicsPipelineStateDesc.NumRenderTargets = createPSODesc.numRenderTargets;
	graphicsPipelineStateDesc.RTVFormats[0] = createPSODesc.RTVFormats;
	//利用するトポロジ(形状)のタイプ。三角形
	graphicsPipelineStateDesc.PrimitiveTopologyType =
		createPSODesc.primitiveTopologyType;
	//どのように画面に色を打ち込むのかの設定
	graphicsPipelineStateDesc.SampleDesc.Count = createPSODesc.sampleDescCount;
	graphicsPipelineStateDesc.SampleMask = createPSODesc.sampleMask;

	//DepthStencilの設定
	graphicsPipelineStateDesc.DepthStencilState = createPSODesc.depthStencilState;
	graphicsPipelineStateDesc.DSVFormat = createPSODesc.DSVFormat;

	//実際に生成
	hr = sDevice_->CreateGraphicsPipelineState(&graphicsPipelineStateDesc,
		IID_PPV_ARGS(&sPipelineState[createPSODesc.pipelineStateName]));
	assert(SUCCEEDED(hr));

}

std::wstring GraphicsPipelineState::ConvertString(const std::string& str) {
	if (str.empty()) {
		return std::wstring();
	}

	auto sizeNeeded = MultiByteToWideChar(CP_UTF8, 0, reinterpret_cast<const char*>(&str[0]), static_cast<int>(str.size()), NULL, 0);
	if (sizeNeeded == 0) {
		return std::wstring();
	}
	std::wstring result(sizeNeeded, 0);
	MultiByteToWideChar(CP_UTF8, 0, reinterpret_cast<const char*>(&str[0]), static_cast<int>(str.size()), &result[0], sizeNeeded);
	return result;
}

std::string GraphicsPipelineState::ConvertString(const std::wstring& str) {
	if (str.empty()) {
		return std::string();
	}

	auto sizeNeeded = WideCharToMultiByte(CP_UTF8, 0, str.data(), static_cast<int>(str.size()), NULL, 0, NULL, NULL);
	if (sizeNeeded == 0) {
		return std::string();
	}
	std::string result(sizeNeeded, 0);
	WideCharToMultiByte(CP_UTF8, 0, str.data(), static_cast<int>(str.size()), result.data(), sizeNeeded, NULL, NULL);
	return result;
}

void GraphicsPipelineState::Log(const std::string& message) {
	OutputDebugStringA(message.c_str());
}
