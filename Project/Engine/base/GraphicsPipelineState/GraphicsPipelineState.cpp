#include "GraphicsPipelineState.h"
#include "../CompileShader.h"
#include "../Log.h"

using namespace Microsoft::WRL;

// ルートシグネチャ
Microsoft::WRL::ComPtr<ID3D12RootSignature> GraphicsPipelineState::sRootSignature[GraphicsPipelineState::PipelineStateName::kPipelineStateNameOfCount];
// パイプラインステートオブジェクト
Microsoft::WRL::ComPtr<ID3D12PipelineState> GraphicsPipelineState::sPipelineState[GraphicsPipelineState::PipelineStateName::kPipelineStateNameOfCount];
// デバイス
ID3D12Device* GraphicsPipelineState::sDevice_;

void GraphicsPipelineState::Initialize(ID3D12Device* sDevice)
{

	// デバイス
	sDevice_ = sDevice;

	// ルートパラメータ
	RootParameterManager::Initialize();
	// サンプラー
	SamplerManager::Initialize();
	// インプットレイアウト
	InputLayoutManager::Initialize();
	// ブレンドステート
	BlendStateManager::Initialize();

	// パイプラインステート作る

	CreateForModel();

	CreateForSprite();

	CreateForParticle();

	CreateForOutLine();

	CreateForCollision2DDebugDraw();

	CreateForLine();

	CreateForSwapChain();

	CreateForManyModels();

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
		kInputLayoutIndexModel);

	createPSODesc.blendDesc = BlendStateSetting(kBlendStateIndexNormal);

	createPSODesc.rasterizerDesc = ResiterzerStateSetting(D3D12_CULL_MODE_BACK, D3D12_FILL_MODE_SOLID);

	createPSODesc.vertexShaderBlob = CompileShader::Compile(L"Resources/shaders/Object3d.vs.hlsl",
		L"vs_6_0");
	createPSODesc.pixelShaderBlob = CompileShader::Compile(L"Resources/shaders/Object3d.PS.hlsl",
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

	createPSODesc.vertexShaderBlob = CompileShader::Compile(L"Resources/shaders/Sprite.vs.hlsl",
		L"vs_6_0");
	createPSODesc.pixelShaderBlob = CompileShader::Compile(L"Resources/shaders/Sprite.PS.hlsl",
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

	createPSODesc.blendDesc = BlendStateSetting(kBlendStateIndexAdd);

	createPSODesc.rasterizerDesc = ResiterzerStateSetting(D3D12_CULL_MODE_BACK, D3D12_FILL_MODE_SOLID);

	createPSODesc.vertexShaderBlob = CompileShader::Compile(L"Resources/shaders/Particle.vs.hlsl",
		L"vs_6_0");
	createPSODesc.pixelShaderBlob = CompileShader::Compile(L"Resources/shaders/Particle.PS.hlsl",
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

	createPSODesc.vertexShaderBlob = CompileShader::Compile(L"Resources/shaders/OutLine3D.vs.hlsl",
		L"vs_6_0");
	createPSODesc.pixelShaderBlob = CompileShader::Compile(L"Resources/shaders/OutLine3D.PS.hlsl",
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

void GraphicsPipelineState::CreateForCollision2DDebugDraw()
{

	CreatePSODesc createPSODesc;

	RootsignatureSetting(
		kPipelineStateNameCollision2DDebugDraw,
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT,
		kRootParameterIndexCollision2DDebugDraw,
		kSamplerIndexNormal);

	createPSODesc.pipelineStateName = kPipelineStateNameCollision2DDebugDraw;

	createPSODesc.depthStencilState = DepthStencilStateSetting(
		true,
		D3D12_DEPTH_WRITE_MASK_ZERO,
		D3D12_COMPARISON_FUNC_LESS_EQUAL);

	createPSODesc.inputLayoutDesc = InputLayoutSetting(
		kInputLayoutIndexNormal);

	createPSODesc.blendDesc = BlendStateSetting(kBlendStateIndexAdd);

	createPSODesc.rasterizerDesc = ResiterzerStateSetting(D3D12_CULL_MODE_BACK, D3D12_FILL_MODE_SOLID);

	createPSODesc.vertexShaderBlob = CompileShader::Compile(L"Resources/shaders/Collider2DDebug.VS.hlsl",
		L"vs_6_0");
	createPSODesc.pixelShaderBlob = CompileShader::Compile(L"Resources/shaders/Collider2DDebug.PS.hlsl",
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

void GraphicsPipelineState::CreateForLine()
{

	CreatePSODesc createPSODesc;

	RootsignatureSetting(
		kPipelineStateNameLine,
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT,
		kRootParameterIndexLine,
		kSamplerIndexNormal);

	createPSODesc.pipelineStateName = kPipelineStateNameLine;

	createPSODesc.depthStencilState = DepthStencilStateSetting(
		true,
		D3D12_DEPTH_WRITE_MASK_ZERO,
		D3D12_COMPARISON_FUNC_LESS_EQUAL);

	createPSODesc.inputLayoutDesc = InputLayoutSetting(
		kInputLayoutIndexLine);

	createPSODesc.blendDesc = BlendStateSetting(kBlendStateIndexNormal);

	createPSODesc.rasterizerDesc = ResiterzerStateSetting(D3D12_CULL_MODE_NONE, D3D12_FILL_MODE_WIREFRAME);

	createPSODesc.vertexShaderBlob = CompileShader::Compile(L"Resources/shaders/Line.VS.hlsl",
		L"vs_6_0");
	createPSODesc.pixelShaderBlob = CompileShader::Compile(L"Resources/shaders/Line.PS.hlsl",
		L"ps_6_0");

	//書き込むRTVの情報
	createPSODesc.numRenderTargets = 1;
	createPSODesc.RTVFormats = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	//利用するトポロジ(形状)のタイプ。
	createPSODesc.primitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE;

	//どのように画面に色を打ち込むのかの設定
	createPSODesc.sampleDescCount = 1;
	createPSODesc.sampleMask = D3D12_DEFAULT_SAMPLE_MASK;

	//DepthStencilの設定
	createPSODesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;

	CreatePSO(createPSODesc);

}

void GraphicsPipelineState::CreateForSwapChain()
{

	CreatePSODesc createPSODesc;

	RootsignatureSetting(
		kPipelineStateNameSwapChain,
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT,
		kRootParameterIndexSwapChain,
		kSamplerIndexPostEffect);

	createPSODesc.pipelineStateName = kPipelineStateNameSwapChain;

	createPSODesc.depthStencilState = DepthStencilStateSetting(
		true,
		D3D12_DEPTH_WRITE_MASK_ALL,
		D3D12_COMPARISON_FUNC_LESS_EQUAL);

	createPSODesc.inputLayoutDesc = InputLayoutSetting(
		kInputLayoutIndexNormal);

	createPSODesc.blendDesc = BlendStateSetting(kBlendStateIndexNormal);

	createPSODesc.rasterizerDesc = ResiterzerStateSetting(D3D12_CULL_MODE_NONE, D3D12_FILL_MODE_SOLID);

	createPSODesc.vertexShaderBlob = CompileShader::Compile(L"Resources/shaders/SwapChain.VS.hlsl",
		L"vs_6_0");
	createPSODesc.pixelShaderBlob = CompileShader::Compile(L"Resources/shaders/SwapChain.PS.hlsl",
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

void GraphicsPipelineState::CreateForManyModels()
{

	CreatePSODesc createPSODesc;

	RootsignatureSetting(
		kPipelineStateNameManyModels,
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT,
		kRootParameterIndexManyModels,
		kSamplerIndexNormal);

	createPSODesc.pipelineStateName = kPipelineStateNameManyModels;

	createPSODesc.depthStencilState = DepthStencilStateSetting(
		true,
		D3D12_DEPTH_WRITE_MASK_ALL,
		D3D12_COMPARISON_FUNC_LESS_EQUAL);

	createPSODesc.inputLayoutDesc = InputLayoutSetting(
		kInputLayoutIndexModel);

	createPSODesc.blendDesc = BlendStateSetting(kBlendStateIndexNormal);

	createPSODesc.rasterizerDesc = ResiterzerStateSetting(D3D12_CULL_MODE_NONE, D3D12_FILL_MODE_SOLID);

	createPSODesc.vertexShaderBlob = CompileShader::Compile(L"Resources/shaders/ManyModels.VS.hlsl",
		L"vs_6_0");
	createPSODesc.pixelShaderBlob = CompileShader::Compile(L"Resources/shaders/ManyModels.PS.hlsl",
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

void GraphicsPipelineState::RootsignatureSetting(PipelineStateName pipelineStateName, D3D12_ROOT_SIGNATURE_FLAGS rootsignatureFlags, RootParameterIndex rootParameterIndex, SamplerIndex samplerIndex)
{

	HRESULT hr;

	D3D12_ROOT_SIGNATURE_DESC descriptionRootsignature{};
	descriptionRootsignature.Flags = rootsignatureFlags;

	descriptionRootsignature.pParameters = RootParameterManager::rootParameters_[rootParameterIndex].data(); //ルートパラメータ配列へのポインタ
	descriptionRootsignature.NumParameters = static_cast<uint32_t>(RootParameterManager::rootParameters_[rootParameterIndex].size()); //配列の長さ

	descriptionRootsignature.pStaticSamplers = SamplerManager::samplers_[samplerIndex].data();
	descriptionRootsignature.NumStaticSamplers = static_cast<uint32_t>(SamplerManager::samplers_[samplerIndex].size());

	//シリアライズしてバイナリにする
	ID3DBlob* signatureBlob = nullptr;
	ID3DBlob* errorBlob = nullptr;
	hr = D3D12SerializeRootSignature(&descriptionRootsignature,
		D3D_ROOT_SIGNATURE_VERSION_1, &signatureBlob, &errorBlob);
	if (FAILED(hr)) {
		Log::Message(reinterpret_cast<char*>(errorBlob->GetBufferPointer()));
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
	return InputLayoutManager::inputLayouts_[inputLayoutIndex];

}

D3D12_BLEND_DESC GraphicsPipelineState::BlendStateSetting(BlendStateIndex blendStateIndex)
{

	return BlendStateManager::blendStates_[blendStateIndex];

}

D3D12_RASTERIZER_DESC GraphicsPipelineState::ResiterzerStateSetting(D3D12_CULL_MODE cullMode, D3D12_FILL_MODE fillMode)
{
	
	D3D12_RASTERIZER_DESC rasterizerDesc{};
	rasterizerDesc.CullMode = cullMode;
	rasterizerDesc.FillMode = fillMode;
	return rasterizerDesc;

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
