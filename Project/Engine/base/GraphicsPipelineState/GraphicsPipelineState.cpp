#include "GraphicsPipelineState.h"
#include "../CompileShader.h"
#include "../Log.h"

using namespace Microsoft::WRL;

// ルートシグネチャ
Microsoft::WRL::ComPtr<ID3D12RootSignature> GraphicsPipelineState::sRootSignature[GraphicsPipelineState::PipelineStateIndex::kPipelineStateIndexOfCount];
// パイプラインステートオブジェクト
Microsoft::WRL::ComPtr<ID3D12PipelineState> GraphicsPipelineState::sPipelineState[GraphicsPipelineState::PipelineStateIndex::kPipelineStateIndexOfCount];
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

	// 引数
	CreateDesc desc;

	// モデル
	desc.pipelineStateIndex = kPipelineStateIndexModel;
	desc.rootParameterIndex = kRootParameterIndexModel;
	desc.samplerIndex = kSamplerIndexNormal;
	desc.depthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	desc.inputLayoutIndex = kInputLayoutIndexModel;
	desc.blendStateIndex = kBlendStateIndexNormal;
	desc.cullMode = D3D12_CULL_MODE_BACK;
	desc.fillMode = D3D12_FILL_MODE_SOLID;
	desc.filePathVS = L"Resources/shaders/Object3d.VS.hlsl";
	desc.filePathPS = L"Resources/shaders/Object3d.PS.hlsl";
	desc.primitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	Create(desc);

	// スプライト
	desc.pipelineStateIndex = kPipelineStateIndexSprite;
	desc.rootParameterIndex = kRootParameterIndexSprite;
	desc.samplerIndex = kSamplerIndexNormal;
	desc.depthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	desc.inputLayoutIndex = kInputLayoutIndexNormal;
	desc.blendStateIndex = kBlendStateIndexNormal;
	desc.cullMode = D3D12_CULL_MODE_NONE;
	desc.fillMode = D3D12_FILL_MODE_SOLID;
	desc.filePathVS = L"Resources/shaders/Sprite.VS.hlsl";
	desc.filePathPS = L"Resources/shaders/Sprite.PS.hlsl";
	desc.primitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	Create(desc);

	// パーティクル
	desc.pipelineStateIndex = kPipelineStateIndexParticle;
	desc.rootParameterIndex = kRootParameterIndexParticle;
	desc.samplerIndex = kSamplerIndexNormal;
	desc.depthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
	desc.inputLayoutIndex = kInputLayoutIndexNormal;
	desc.blendStateIndex = kBlendStateIndexAdd;
	desc.cullMode = D3D12_CULL_MODE_BACK;
	desc.fillMode = D3D12_FILL_MODE_SOLID;
	desc.filePathVS = L"Resources/shaders/Particle.VS.hlsl";
	desc.filePathPS = L"Resources/shaders/Particle.PS.hlsl";
	desc.primitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	Create(desc);

	// 2Dコライダーデバッグ
	desc.pipelineStateIndex = kPipelineStateIndexCollision2DDebugDraw;
	desc.rootParameterIndex = kRootParameterIndexCollision2DDebugDraw;
	desc.samplerIndex = kSamplerIndexNormal;
	desc.depthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
	desc.inputLayoutIndex = kInputLayoutIndexNormal;
	desc.blendStateIndex = kBlendStateIndexAdd;
	desc.cullMode = D3D12_CULL_MODE_BACK;
	desc.fillMode = D3D12_FILL_MODE_SOLID;
	desc.filePathVS = L"Resources/shaders/Collider2DDebug.VS.hlsl";
	desc.filePathPS = L"Resources/shaders/Collider2DDebug.PS.hlsl";
	desc.primitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	Create(desc);

	// 線
	desc.pipelineStateIndex = kPipelineStateIndexLine;
	desc.rootParameterIndex = kRootParameterIndexLine;
	desc.samplerIndex = kSamplerIndexNormal;
	desc.depthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
	desc.inputLayoutIndex = kInputLayoutIndexNone;
	desc.blendStateIndex = kBlendStateIndexAdd;
	desc.cullMode = D3D12_CULL_MODE_NONE;
	desc.fillMode = D3D12_FILL_MODE_WIREFRAME;
	desc.filePathVS = L"Resources/shaders/Line.VS.hlsl";
	desc.filePathPS = L"Resources/shaders/Line.PS.hlsl";
	desc.primitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE;
	Create(desc);

	// ウィンドウスプライト
	desc.pipelineStateIndex = kPipelineStateIndexWindowSprite;
	desc.rootParameterIndex = kRootParameterIndexWindowSprite;
	desc.samplerIndex = kSamplerIndexPostEffect;
	desc.depthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
	desc.inputLayoutIndex = kInputLayoutIndexNone;
	desc.blendStateIndex = kBlendStateIndexNormal;
	desc.cullMode = D3D12_CULL_MODE_NONE;
	desc.fillMode = D3D12_FILL_MODE_SOLID;
	desc.filePathVS = L"Resources/shaders/WindowSprite.VS.hlsl";
	desc.filePathPS = L"Resources/shaders/WindowSprite.PS.hlsl";
	desc.primitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	Create(desc);

	// たくさんモデル
	desc.pipelineStateIndex = kPipelineStateIndexManyModels;
	desc.rootParameterIndex = kRootParameterIndexManyModels;
	desc.samplerIndex = kSamplerIndexNormal;
	desc.depthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	desc.inputLayoutIndex = kInputLayoutIndexModel;
	desc.blendStateIndex = kBlendStateIndexNormal;
	desc.cullMode = D3D12_CULL_MODE_NONE;
	desc.fillMode = D3D12_FILL_MODE_SOLID;
	desc.filePathVS = L"Resources/shaders/ManyModels.VS.hlsl";
	desc.filePathPS = L"Resources/shaders/ManyModels.PS.hlsl";
	desc.primitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	Create(desc);

}

void GraphicsPipelineState::Create(const CreateDesc& desc)
{

	CreatePSODesc createPSODesc;

	RootsignatureSetting(
		desc.pipelineStateIndex,
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT,
		desc.rootParameterIndex,
		desc.samplerIndex);

	createPSODesc.pipelineStateIndex = desc.pipelineStateIndex;

	createPSODesc.depthStencilState = DepthStencilStateSetting(
		true,
		desc.depthWriteMask,
		D3D12_COMPARISON_FUNC_LESS_EQUAL);

	createPSODesc.inputLayoutDesc = InputLayoutSetting(
		desc.inputLayoutIndex);

	createPSODesc.blendDesc = BlendStateSetting(desc.blendStateIndex);

	createPSODesc.rasterizerDesc = ResiterzerStateSetting(desc.cullMode, desc.fillMode);

	createPSODesc.vertexShaderBlob = CompileShader::Compile(desc.filePathVS, L"vs_6_0");
	createPSODesc.pixelShaderBlob = CompileShader::Compile(desc.filePathPS, L"ps_6_0");

	//書き込むRTVの情報
	createPSODesc.numRenderTargets = 1;
	createPSODesc.RTVFormats = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	//利用するトポロジ(形状)のタイプ。
	createPSODesc.primitiveTopologyType = desc.primitiveTopologyType;

	//どのように画面に色を打ち込むのかの設定
	createPSODesc.sampleDescCount = 1;
	createPSODesc.sampleMask = D3D12_DEFAULT_SAMPLE_MASK;

	//DepthStencilの設定
	createPSODesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;

	CreatePSO(createPSODesc);

}

void GraphicsPipelineState::RootsignatureSetting(PipelineStateIndex pipelineStateName, D3D12_ROOT_SIGNATURE_FLAGS rootsignatureFlags, RootParameterIndex rootParameterIndex, SamplerIndex samplerIndex)
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
	graphicsPipelineStateDesc.pRootSignature = sRootSignature[createPSODesc.pipelineStateIndex].Get();//RootSignature
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
		IID_PPV_ARGS(&sPipelineState[createPSODesc.pipelineStateIndex]));
	assert(SUCCEEDED(hr));

}
