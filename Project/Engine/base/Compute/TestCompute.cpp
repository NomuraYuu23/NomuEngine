#include "TestCompute.h"
#include <cassert>
#include "../Log.h"
#include "../SRVDescriptorHerpManager.h"
#include "../CompileShader.h"

void TestCompute::Initialize(
	const std::wstring& filePath,
	const wchar_t* entryPoint)
{

	BaseConpute::Initialize(filePath, entryPoint);
	
	// リソース
	CreateResource();
	// UAV
	CreateUAV();
	// マップ
	Map();

}

void TestCompute::Execution(DirectXCommon* dxCommon)
{
	
	ID3D12GraphicsCommandList* list = dxCommon->GetCommadList();

	// ルートシグネチャ
	dxCommon->GetCommadList()->SetComputeRootSignature(rootSignature_.Get());
	// パイプライン
	dxCommon->GetCommadList()->SetPipelineState(pipelineState_.Get());

	// ヒープ
	ID3D12DescriptorHeap* ppHeaps[] = { SRVDescriptorHerpManager::descriptorHeap_.Get() };
	list->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);

	// ルートパラメータ
	list->SetComputeRootDescriptorTable(0, uavHandleGPU_);

	// シェーダー実行
	list->Dispatch(static_cast<uint32_t>(256), 1, 1);

	// コマンド実行
	CommandExecution(dxCommon);

	// データ
	float s = static_cast<float*>(data_)[5];

}

void TestCompute::CreateRootSignature()
{

	HRESULT hr;

	D3D12_ROOT_SIGNATURE_DESC descriptionRootsignature{};
	descriptionRootsignature.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	// ディスクリプタレンジ
	D3D12_DESCRIPTOR_RANGE descriptorRange[1] = {};
	descriptorRange[0].BaseShaderRegister = 0;//0から始まる
	descriptorRange[0].NumDescriptors = 1;//数は一つ
	descriptorRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_UAV;//UAVを使う
	descriptorRange[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;//Offsetを自動計算

	// ルートパラメータ
	D3D12_ROOT_PARAMETER rootParameters[1] = {};
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

	hr = device_->CreateRootSignature(0, signatureBlob->GetBufferPointer(),
		signatureBlob->GetBufferSize(), IID_PPV_ARGS(&rootSignature_));
	assert(SUCCEEDED(hr));

}

void TestCompute::CreatePipelineState()
{

	D3D12_COMPUTE_PIPELINE_STATE_DESC desc{};
	desc.CS.pShaderBytecode = shader_->GetBufferPointer();
	desc.CS.BytecodeLength = shader_->GetBufferSize();
	desc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;
	desc.NodeMask = 0;
	desc.pRootSignature = rootSignature_.Get();

	HRESULT hr = device_->CreateComputePipelineState(&desc, IID_PPV_ARGS(&pipelineState_));

}

void TestCompute::CreateShader(const std::wstring& filePath, const wchar_t* entryPoint)
{

	shader_ = CompileShader::Compile(
		filePath,
		L"cs_6_0",
		entryPoint);

}

void TestCompute::CreateResource()
{

	D3D12_HEAP_PROPERTIES prop{};

	prop.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_WRITE_BACK;
	prop.CreationNodeMask = 1;
	prop.MemoryPoolPreference = D3D12_MEMORY_POOL_L0;
	prop.Type = D3D12_HEAP_TYPE_CUSTOM;
	prop.VisibleNodeMask = 1;

	D3D12_RESOURCE_DESC desc{};
	desc.Alignment = 0;
	desc.DepthOrArraySize = 1;
	desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	desc.Flags = D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
	desc.Format = DXGI_FORMAT_UNKNOWN;
	desc.Height = 1;
	desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	desc.MipLevels = 1;
	desc.SampleDesc = { 1,0 };
	desc.Width = (sizeof(float) * 256 + 0xff) & ~0xff;

	HRESULT hr = device_->CreateCommittedResource(
		&prop,
		D3D12_HEAP_FLAG_NONE,
		&desc,
		D3D12_RESOURCE_STATE_UNORDERED_ACCESS,
		nullptr,
		IID_PPV_ARGS(&resource_));

}

void TestCompute::CreateUAV()
{

	D3D12_UNORDERED_ACCESS_VIEW_DESC desc{};
	desc.ViewDimension = D3D12_UAV_DIMENSION_BUFFER;
	desc.Format = DXGI_FORMAT_UNKNOWN;
	desc.Buffer.NumElements = static_cast<uint32_t>(256);
	desc.Buffer.StructureByteStride = sizeof(float);

	uavHandleCPU_ = SRVDescriptorHerpManager::GetCPUDescriptorHandle();
	uavHandleGPU_ = SRVDescriptorHerpManager::GetGPUDescriptorHandle();
	indexDescriptorHeap_ = SRVDescriptorHerpManager::GetNextIndexDescriptorHeap();
	SRVDescriptorHerpManager::NextIndexDescriptorHeapChange();

	device_->CreateUnorderedAccessView(
		resource_.Get(),
		nullptr,
		&desc,
		uavHandleCPU_);

}

void TestCompute::Map()
{

	D3D12_RANGE range{ 0, 1 };
	resource_->Map(0,&range, &data_);

}
