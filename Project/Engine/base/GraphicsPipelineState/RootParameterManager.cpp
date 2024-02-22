#include "RootParameterManager.h"

using namespace Microsoft::WRL;

// ルートパラメータ
std::array<std::vector<D3D12_ROOT_PARAMETER>, kRootParameterIndexOfCount> RootParameterManager::rootParameters_;
// ディスクリプタレンジ
std::array<std::vector<D3D12_DESCRIPTOR_RANGE>, kDescriptorRangeIndexOfCount> RootParameterManager::descriptorRanges_;

RootParameterManager* RootParameterManager::GetInstance()
{
	static RootParameterManager instance;
	return &instance;
}

void RootParameterManager::Initialize()
{

	// ディスクリプタレンジ
	DescriptorRangeInitialize();

	// モデル
	RootParameterInitializeForModel();
	// スプライト
	RootParameterInitializeForSprite();
	// パーティクル
	RootParameterInitializeForParticle();
	// アウトライン
	RootParameterInitializeForOutLine();

}

void RootParameterManager::RootParameterInitializeForModel()
{

	//RootParameter作成
	D3D12_ROOT_PARAMETER rootParameters[7] = {};
	// マテリアル
	rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;   //CBVを使う
	rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;//PixelShaderで使う
	rootParameters[0].Descriptor.ShaderRegister = 0;                   //レジスタ番号0とバインド
	// トランスフォームマトリックス
	rootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;   //CBVを使う
	rootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;//VertexShaderで使う
	rootParameters[1].Descriptor.ShaderRegister = 0;                   //レジスタ番号0とバインド
	// テクスチャ
	rootParameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;//DescriptorTableを使う
	rootParameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;//PixelShaderで使う
	rootParameters[2].DescriptorTable.pDescriptorRanges = descriptorRanges_[kDescriptorRangeIndexTexture].data();//Tableの中身の配列を指定
	rootParameters[2].DescriptorTable.NumDescriptorRanges = static_cast<uint32_t>(descriptorRanges_[kDescriptorRangeIndexTexture].size());//Tableで利用する数
	// 平行光源
	rootParameters[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;   //CBVを使う
	rootParameters[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;//PixelShaderで使う
	rootParameters[3].Descriptor.ShaderRegister = 1;
	// カメラ
	rootParameters[4].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;   //CBVを使う
	rootParameters[4].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;//PixelShaderで使う
	rootParameters[4].Descriptor.ShaderRegister = 2;
	// ポイントライト
	rootParameters[5].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;//DescriptorTableを使う
	rootParameters[5].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;//PixelShaderで使う
	rootParameters[5].DescriptorTable.pDescriptorRanges = descriptorRanges_[kDescriptorRangeIndexPointLight].data();//Tableの中身の配列を指定
	rootParameters[5].DescriptorTable.NumDescriptorRanges = static_cast<uint32_t>(descriptorRanges_[kDescriptorRangeIndexPointLight].size());//Tableで利用する数
	// スポットライト
	rootParameters[6].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;//DescriptorTableを使う
	rootParameters[6].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;//PixelShaderで使う
	rootParameters[6].DescriptorTable.pDescriptorRanges = descriptorRanges_[kDescriptorRangeIndexSpotLight].data();//Tableの中身の配列を指定
	rootParameters[6].DescriptorTable.NumDescriptorRanges = static_cast<uint32_t>(descriptorRanges_[kDescriptorRangeIndexSpotLight].size());//Tableで利用する数


	for (uint32_t i = 0; i < _countof(rootParameters); ++i) {
		rootParameters_[kRootParameterIndexModel].push_back(rootParameters[i]);
	}

}

void RootParameterManager::RootParameterInitializeForSprite()
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

void RootParameterManager::RootParameterInitializeForParticle()
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

void RootParameterManager::RootParameterInitializeForOutLine()
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

void RootParameterManager::DescriptorRangeInitialize()
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
	
	D3D12_DESCRIPTOR_RANGE descriptorRangeForPointLight[1] = {};
	descriptorRangeForPointLight[0].BaseShaderRegister = 1;//0から始まる
	descriptorRangeForPointLight[0].NumDescriptors = 1;//数は一つ
	descriptorRangeForPointLight[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;//SRVを使う
	descriptorRangeForPointLight[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;//Offsetを自動計算

	descriptorRanges_[kDescriptorRangeIndexPointLight].push_back(descriptorRangeForPointLight[0]);

	D3D12_DESCRIPTOR_RANGE descriptorRangeForSpotLight[1] = {};
	descriptorRangeForSpotLight[0].BaseShaderRegister = 2;//0から始まる
	descriptorRangeForSpotLight[0].NumDescriptors = 1;//数は一つ
	descriptorRangeForSpotLight[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;//SRVを使う
	descriptorRangeForSpotLight[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;//Offsetを自動計算

	descriptorRanges_[kDescriptorRangeIndexSpotLight].push_back(descriptorRangeForSpotLight[0]);

}
