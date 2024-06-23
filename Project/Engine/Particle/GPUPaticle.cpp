#include "GPUPaticle.h"
#include "../base/BufferResource.h"
#include "ParticleCS.h"
#include "../base/SRVDescriptorHerpManager.h"
#include "../base/CompileShader.h"
#include "../base/Log.h"
#include "../base/TextureManager.h"
#include "BillBoardMatrix.h"

GPUPaticle* GPUPaticle::GetInstance()
{
	static GPUPaticle instance;
	return &instance;
}

void GPUPaticle::Initialize(
	ID3D12Device* device,
	ID3D12GraphicsCommandList* commandList,
	ID3D12RootSignature* rootSignature,
	ID3D12PipelineState* pipelineState)
{

	// CSの初期化
	PipelineStateCSInitialize(device);

	// バッファの初期化
	BuffInitialize(device, commandList);

	// モデル関連の初期化
	ModelInitialize();

	rootSignature_ = rootSignature;

	pipelineState_ = pipelineState;

	//Sprite用のマテリアルリソースを作る
	gpuParticleViewBuff_ = BufferResource::CreateBufferResource(device, (sizeof(GPUParticleView) + 0xff) & ~0xff);
	//書き込むためのアドレスを取得
	gpuParticleViewBuff_->Map(0, nullptr, reinterpret_cast<void**>(&gpuParticleViewMap));

	gpuParticleViewMap->billboardMatrix = Matrix4x4::MakeIdentity4x4();
	gpuParticleViewMap->viewProjection = Matrix4x4::MakeIdentity4x4();

}

void GPUPaticle::Draw(
	ID3D12GraphicsCommandList* commandList,
	BaseCamera& camera)
{

	assert(commandList);

	GPUParticleViewMapping(camera);

	// SRV
	ID3D12DescriptorHeap* ppHeaps[] = { SRVDescriptorHerpManager::descriptorHeap_.Get() };
	commandList->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);

	//形状を設定。PS0に設定しているものとは別。
	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// パイプライン
	commandList->SetPipelineState(pipelineState_);
	// ルートシグネチャ
	commandList->SetGraphicsRootSignature(rootSignature_);

	// 頂点データ
	commandList->IASetVertexBuffers(0, 1, model_->GetMesh()->GetVbView());


	// GPUパーティクル用
	commandList->SetGraphicsRootDescriptorTable(0, srvHandleGPU_);
	// GPUパーティクルのView
	commandList->SetGraphicsRootConstantBufferView(1, gpuParticleViewBuff_->GetGPUVirtualAddress());
	// テクスチャ
	TextureManager::GetInstance()->SetGraphicsRootDescriptorTable(
		commandList,
		2,
		model_->GetTextureHandles()[0]);
	// マテリアル
	commandList->SetGraphicsRootConstantBufferView(3, material_->GetMaterialBuff()->GetGPUVirtualAddress());
	// 描画
	commandList->DrawInstanced(6, kParticleMax, 0, 0);

}

void GPUPaticle::PipelineStateCSInitialize(ID3D12Device* device)
{

	HRESULT hr;

#pragma region kPiprlineStateCSIndexInitialize

	D3D12_ROOT_SIGNATURE_DESC descriptionRootsignature{};
	descriptionRootsignature.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	// ルートパラメータ
	D3D12_ROOT_PARAMETER rootParameters[1] = {};

	// UAV * 1
	D3D12_DESCRIPTOR_RANGE descriptorRange[1] = {};
	descriptorRange[0].BaseShaderRegister = 0;//iから始まる
	descriptorRange[0].NumDescriptors = 1;//数は一つ
	descriptorRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_UAV;//UAVを使う
	descriptorRange[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;//Offsetを自動計算

	rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;//DescriptorTableを使う
	rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;//全てで使う
	rootParameters[0].DescriptorTable.pDescriptorRanges = descriptorRange;//Tableの中身の配列を指定
	rootParameters[0].DescriptorTable.NumDescriptorRanges = _countof(descriptorRange);//Tableで利用する数

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

	hr = device->CreateRootSignature(0, signatureBlob->GetBufferPointer(),
		signatureBlob->GetBufferSize(), IID_PPV_ARGS(&rootSignaturesCS_[kPiprlineStateCSIndexInitialize]));
	assert(SUCCEEDED(hr));

	// シェーダコンパイル
	IDxcBlob* shader = CompileShader::Compile(
		L"Resources/shaders/GPUParticle/GPUParticle.CS.hlsl",
		L"cs_6_0",
		L"initialize");

	// パイプライン
	D3D12_COMPUTE_PIPELINE_STATE_DESC desc{};
	desc.CS.pShaderBytecode = shader->GetBufferPointer();
	desc.CS.BytecodeLength = shader->GetBufferSize();
	desc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;
	desc.NodeMask = 0;
	desc.pRootSignature = rootSignaturesCS_[kPiprlineStateCSIndexInitialize].Get();

	hr = device->CreateComputePipelineState(&desc, IID_PPV_ARGS(&pipelineStatesCS_[kPiprlineStateCSIndexInitialize]));
	assert(SUCCEEDED(hr));

#pragma endregion

}

void GPUPaticle::BuffInitialize(ID3D12Device* device,
	ID3D12GraphicsCommandList* commandList)
{

	// バッファ
	buff_ = BufferResource::CreateBufferResourceUAV(device, ((sizeof(ParticleCS) + 0xff) & ~0xff) * kParticleMax);

	/// UAV

	D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc{};

	uavDesc.Format = DXGI_FORMAT_UNKNOWN;
	uavDesc.ViewDimension = D3D12_UAV_DIMENSION_BUFFER;
	uavDesc.Buffer.FirstElement = 0;
	uavDesc.Buffer.NumElements = kParticleMax;
	uavDesc.Buffer.CounterOffsetInBytes = 0;
	uavDesc.Buffer.Flags = D3D12_BUFFER_UAV_FLAG_NONE;
	uavDesc.Buffer.StructureByteStride = sizeof(ParticleCS);

	uavHandleCPU_ = SRVDescriptorHerpManager::GetCPUDescriptorHandle();
	uavHandleGPU_ = SRVDescriptorHerpManager::GetGPUDescriptorHandle();
	uavIndexDescriptorHeap_ = SRVDescriptorHerpManager::GetNextIndexDescriptorHeap();
	SRVDescriptorHerpManager::NextIndexDescriptorHeapChange();

	device->CreateUnorderedAccessView(buff_.Get(), nullptr, &uavDesc, uavHandleCPU_);

	/// ここまでUAV

	/// SRV

	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Format = DXGI_FORMAT_UNKNOWN;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
	srvDesc.Buffer.FirstElement = 0;
	srvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
	srvDesc.Buffer.NumElements = kParticleMax;
	srvDesc.Buffer.StructureByteStride = sizeof(ParticleCS);

	srvHandleCPU_ = SRVDescriptorHerpManager::GetCPUDescriptorHandle();
	srvHandleGPU_ = SRVDescriptorHerpManager::GetGPUDescriptorHandle();
	srvIndexDescriptorHeap_ = SRVDescriptorHerpManager::GetNextIndexDescriptorHeap();
	SRVDescriptorHerpManager::NextIndexDescriptorHeapChange();

	device->CreateShaderResourceView(buff_.Get(), &srvDesc, srvHandleCPU_);

	/// ここまでSRV

	// CSによる初期化

	// SRV
	ID3D12DescriptorHeap* ppHeaps[] = { SRVDescriptorHerpManager::descriptorHeap_.Get() };
	commandList->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);

	commandList->SetPipelineState(pipelineStatesCS_[kPiprlineStateCSIndexInitialize].Get());//PS0を設定
	commandList->SetComputeRootSignature(rootSignaturesCS_[kPiprlineStateCSIndexInitialize].Get());

	commandList->SetComputeRootDescriptorTable(0, uavHandleGPU_);

	commandList->Dispatch(1, 1, 1);

}

void GPUPaticle::ModelInitialize()
{

	textureHandleManager_ = std::make_unique<ITextureHandleManager>();
	textureHandleManager_->Initialize();

	model_.reset(Model::Create(
		kDirectoryPath,
		kFilename,
		DirectXCommon::GetInstance(),
		textureHandleManager_.get()));

	material_.reset(Material::Create());

}

void GPUPaticle::GPUParticleViewMapping(BaseCamera& camera)
{

	// 全軸
	gpuParticleViewMap->billboardMatrix = BillBoardMatrix::GetBillBoardMatrixAll(camera);

	gpuParticleViewMap->viewProjection = camera.GetViewProjectionMatrix();

}
