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
	// コライダーデバッグ2D
	RootParameterInitializeForCollision2DDebugDraw();
	// 線
	RootParameterInitializeForLine();
	// スワップチェーン
	RootParameterInitializeForSwapChain();
	// 多くのオブジェクト
	RootParameterInitializeForManyModels();

}

void RootParameterManager::RootParameterInitializeForModel()
{

	//RootParameter作成
	D3D12_ROOT_PARAMETER rootParameters[13] = {};
	
	// マテリアル
	rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;   //CBVを使う
	rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;//PixelShaderで使う
	rootParameters[0].Descriptor.ShaderRegister = 0;                   //レジスタ番号0とバインド

	// 平行光源
	rootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;   //CBVを使う
	rootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;//PixelShaderで使う
	rootParameters[1].Descriptor.ShaderRegister = 1;
	
	// カメラ
	rootParameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;   //CBVを使う
	rootParameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;//PixelShaderで使う
	rootParameters[2].Descriptor.ShaderRegister = 2;

	// ワールドトランスフォーム
	rootParameters[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;   //CBVを使う
	rootParameters[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;//VertexShaderで使う
	rootParameters[3].Descriptor.ShaderRegister = 0;

	// トランスフォームマトリックス
	rootParameters[4].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;//DescriptorTableを使う
	rootParameters[4].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;//VertexShaderで使う
	rootParameters[4].DescriptorTable.pDescriptorRanges = descriptorRanges_[kDescriptorRangeIndexLocalMatrix].data();//Tableの中身の配列を指定
	rootParameters[4].DescriptorTable.NumDescriptorRanges = static_cast<uint32_t>(descriptorRanges_[kDescriptorRangeIndexLocalMatrix].size());//Tableで利用する数
	// テクスチャ
	rootParameters[5].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;//DescriptorTableを使う
	rootParameters[5].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;//PixelShaderで使う
	rootParameters[5].DescriptorTable.pDescriptorRanges = descriptorRanges_[kDescriptorRangeIndexTexture0].data();//Tableの中身の配列を指定
	rootParameters[5].DescriptorTable.NumDescriptorRanges = static_cast<uint32_t>(descriptorRanges_[kDescriptorRangeIndexTexture0].size());//Tableで利用する数

	rootParameters[6].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;//DescriptorTableを使う
	rootParameters[6].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;//PixelShaderで使う
	rootParameters[6].DescriptorTable.pDescriptorRanges = descriptorRanges_[kDescriptorRangeIndexTexture1].data();//Tableの中身の配列を指定
	rootParameters[6].DescriptorTable.NumDescriptorRanges = static_cast<uint32_t>(descriptorRanges_[kDescriptorRangeIndexTexture1].size());//Tableで利用する数

	rootParameters[7].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;//DescriptorTableを使う
	rootParameters[7].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;//PixelShaderで使う
	rootParameters[7].DescriptorTable.pDescriptorRanges = descriptorRanges_[kDescriptorRangeIndexTexture2].data();//Tableの中身の配列を指定
	rootParameters[7].DescriptorTable.NumDescriptorRanges = static_cast<uint32_t>(descriptorRanges_[kDescriptorRangeIndexTexture2].size());//Tableで利用する数

	rootParameters[8].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;//DescriptorTableを使う
	rootParameters[8].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;//PixelShaderで使う
	rootParameters[8].DescriptorTable.pDescriptorRanges = descriptorRanges_[kDescriptorRangeIndexTexture3].data();//Tableの中身の配列を指定
	rootParameters[8].DescriptorTable.NumDescriptorRanges = static_cast<uint32_t>(descriptorRanges_[kDescriptorRangeIndexTexture3].size());//Tableで利用する数

	// ポイントライト
	rootParameters[9].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;//DescriptorTableを使う
	rootParameters[9].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;//PixelShaderで使う
	rootParameters[9].DescriptorTable.pDescriptorRanges = descriptorRanges_[kDescriptorRangeIndexPointLight].data();//Tableの中身の配列を指定
	rootParameters[9].DescriptorTable.NumDescriptorRanges = static_cast<uint32_t>(descriptorRanges_[kDescriptorRangeIndexPointLight].size());//Tableで利用する数
	// スポットライト
	rootParameters[10].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;//DescriptorTableを使う
	rootParameters[10].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;//PixelShaderで使う
	rootParameters[10].DescriptorTable.pDescriptorRanges = descriptorRanges_[kDescriptorRangeIndexSpotLight].data();//Tableの中身の配列を指定
	rootParameters[10].DescriptorTable.NumDescriptorRanges = static_cast<uint32_t>(descriptorRanges_[kDescriptorRangeIndexSpotLight].size());//Tableで利用する数

	// 霧
	rootParameters[11].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;   //CBVを使う
	rootParameters[11].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;//PixelShaderで使う
	rootParameters[11].Descriptor.ShaderRegister = 3;

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
	rootParameters[2].DescriptorTable.pDescriptorRanges = descriptorRanges_[kDescriptorRangeIndexTexture0].data();//Tableの中身の配列を指定
	rootParameters[2].DescriptorTable.NumDescriptorRanges = static_cast<uint32_t>(descriptorRanges_[kDescriptorRangeIndexTexture0].size());//Tableで利用する数
	rootParameters[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;   //CBVを使う
	rootParameters[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;//VertexShaderで使う
	rootParameters[3].Descriptor.ShaderRegister = 1;

	for (uint32_t i = 0; i < 4; ++i) {
		rootParameters_[kRootParameterIndexSprite].push_back(rootParameters[i]);
	}

}

void RootParameterManager::RootParameterInitializeForParticle()
{

	//RootParameter作成。複数設定できるので配列。今回は1つだけなので長さ1の配列
	D3D12_ROOT_PARAMETER rootParameters[5] = {};
	
	// マテリアル
	rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;   //CBVを使う
	rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;//PixelShaderで使う
	rootParameters[0].Descriptor.ShaderRegister = 0;                   //レジスタ番号0とバインド
	
	// インスタンシング
	rootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;//DescriptorTableを使う
	rootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;//VertexShaderで使う
	rootParameters[1].DescriptorTable.pDescriptorRanges = descriptorRanges_[kDescriptorRangeIndexInstancing].data();//Tableの中身の配列を指定
	rootParameters[1].DescriptorTable.NumDescriptorRanges = static_cast<uint32_t>(descriptorRanges_[kDescriptorRangeIndexInstancing].size());//Tableで利用する数
	
	// テクスチャ
	rootParameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;//DescriptorTableを使う
	rootParameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;//PixelShaderで使う
	rootParameters[2].DescriptorTable.pDescriptorRanges = descriptorRanges_[kDescriptorRangeIndexTexture0].data();//Tableの中身の配列を指定
	rootParameters[2].DescriptorTable.NumDescriptorRanges = static_cast<uint32_t>(descriptorRanges_[kDescriptorRangeIndexTexture0].size());//Tableで利用する数
	
	// 開始位置
	rootParameters[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;   //CBVを使う
	rootParameters[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;//VertexShaderで使う
	rootParameters[3].Descriptor.ShaderRegister = 0;

	for (uint32_t i = 0; i < _countof(rootParameters); ++i) {
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

void RootParameterManager::RootParameterInitializeForCollision2DDebugDraw()
{

	//RootParameter作成。複数設定できるので配列。
	D3D12_ROOT_PARAMETER rootParameters[4] = {};
	// マテリアル
	rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;   //CBVを使う
	rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;//PixelShaderで使う
	rootParameters[0].Descriptor.ShaderRegister = 0;                   //レジスタ番号0とバインド
	// Collider2DDebugDrawForGPU
	rootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;//DescriptorTableを使う
	rootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;//VertexShaderで使う
	rootParameters[1].DescriptorTable.pDescriptorRanges = descriptorRanges_[kDexcriptorRangeIndexCollider2DDebugDrawForGPU].data();//Tableの中身の配列を指定
	rootParameters[1].DescriptorTable.NumDescriptorRanges = static_cast<uint32_t>(descriptorRanges_[kDexcriptorRangeIndexCollider2DDebugDrawForGPU].size());//Tableで利用する数
	// テクスチャ
	rootParameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;//DescriptorTableを使う
	rootParameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;//PixelShaderで使う
	rootParameters[2].DescriptorTable.pDescriptorRanges = descriptorRanges_[kDescriptorRangeIndexTexture0].data();//Tableの中身の配列を指定
	rootParameters[2].DescriptorTable.NumDescriptorRanges = static_cast<uint32_t>(descriptorRanges_[kDescriptorRangeIndexTexture0].size());//Tableで利用する数

	rootParameters[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;//DescriptorTableを使う
	rootParameters[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;//PixelShaderで使う
	rootParameters[3].DescriptorTable.pDescriptorRanges = descriptorRanges_[kDescriptorRangeIndexTexture1].data();//Tableの中身の配列を指定
	rootParameters[3].DescriptorTable.NumDescriptorRanges = static_cast<uint32_t>(descriptorRanges_[kDescriptorRangeIndexTexture1].size());//Tableで利用する数

	for (uint32_t i = 0; i < _countof(rootParameters); ++i) {
		rootParameters_[kRootParameterIndexCollision2DDebugDraw].push_back(rootParameters[i]);
	}


}

void RootParameterManager::RootParameterInitializeForLine()
{

	//RootParameter作成。複数設定できるので配列。
	D3D12_ROOT_PARAMETER rootParameters[1] = {};
	// VP
	rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;   //CBVを使う
	rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;//PixelShaderで使う
	rootParameters[0].Descriptor.ShaderRegister = 0;                   //レジスタ番号0とバインド
	// LineForGPU
	rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;//DescriptorTableを使う
	rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;//PixelShaderで使う
	rootParameters[0].DescriptorTable.pDescriptorRanges = descriptorRanges_[kDexcriptorRangeIndexLineForGPU].data();//Tableの中身の配列を指定
	rootParameters[0].DescriptorTable.NumDescriptorRanges = static_cast<uint32_t>(descriptorRanges_[kDexcriptorRangeIndexLineForGPU].size());//Tableで利用する数

	for (uint32_t i = 0; i < _countof(rootParameters); ++i) {
		rootParameters_[kRootParameterIndexLine].push_back(rootParameters[i]);
	}

}

void RootParameterManager::RootParameterInitializeForSwapChain()
{

	//RootParameter作成。複数設定できるので配列。
	D3D12_ROOT_PARAMETER rootParameters[1] = {};
	// テクスチャ
	rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;//DescriptorTableを使う
	rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;//PixelShaderで使う
	rootParameters[0].DescriptorTable.pDescriptorRanges = descriptorRanges_[kDescriptorRangeIndexTexture0].data();//Tableの中身の配列を指定
	rootParameters[0].DescriptorTable.NumDescriptorRanges = static_cast<uint32_t>(descriptorRanges_[kDescriptorRangeIndexTexture0].size());//Tableで利用する数


	for (uint32_t i = 0; i < _countof(rootParameters); ++i) {
		rootParameters_[kRootParameterIndexWindowSprite].push_back(rootParameters[i]);
	}

}

void RootParameterManager::RootParameterInitializeForManyModels()
{

	//RootParameter作成
	D3D12_ROOT_PARAMETER rootParameters[13] = {};
	// マテリアル
	rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;   //CBVを使う
	rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;//PixelShaderで使う
	rootParameters[0].Descriptor.ShaderRegister = 0;                   //レジスタ番号0とバインド
	// トランスフォームマトリックス
	rootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;//DescriptorTableを使う
	rootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;//VertexShaderで使う
	rootParameters[1].DescriptorTable.pDescriptorRanges = descriptorRanges_[kDescriptorRangeIndexLocalMatrix].data();//Tableの中身の配列を指定
	rootParameters[1].DescriptorTable.NumDescriptorRanges = static_cast<uint32_t>(descriptorRanges_[kDescriptorRangeIndexLocalMatrix].size());//Tableで利用する数
	// テクスチャ
	rootParameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;//DescriptorTableを使う
	rootParameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;//PixelShaderで使う
	rootParameters[2].DescriptorTable.pDescriptorRanges = descriptorRanges_[kDescriptorRangeIndexTexture0].data();//Tableの中身の配列を指定
	rootParameters[2].DescriptorTable.NumDescriptorRanges = static_cast<uint32_t>(descriptorRanges_[kDescriptorRangeIndexTexture0].size());//Tableで利用する数

	rootParameters[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;//DescriptorTableを使う
	rootParameters[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;//PixelShaderで使う
	rootParameters[3].DescriptorTable.pDescriptorRanges = descriptorRanges_[kDescriptorRangeIndexTexture1].data();//Tableの中身の配列を指定
	rootParameters[3].DescriptorTable.NumDescriptorRanges = static_cast<uint32_t>(descriptorRanges_[kDescriptorRangeIndexTexture1].size());//Tableで利用する数

	rootParameters[4].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;//DescriptorTableを使う
	rootParameters[4].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;//PixelShaderで使う
	rootParameters[4].DescriptorTable.pDescriptorRanges = descriptorRanges_[kDescriptorRangeIndexTexture2].data();//Tableの中身の配列を指定
	rootParameters[4].DescriptorTable.NumDescriptorRanges = static_cast<uint32_t>(descriptorRanges_[kDescriptorRangeIndexTexture2].size());//Tableで利用する数

	rootParameters[5].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;//DescriptorTableを使う
	rootParameters[5].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;//PixelShaderで使う
	rootParameters[5].DescriptorTable.pDescriptorRanges = descriptorRanges_[kDescriptorRangeIndexTexture3].data();//Tableの中身の配列を指定
	rootParameters[5].DescriptorTable.NumDescriptorRanges = static_cast<uint32_t>(descriptorRanges_[kDescriptorRangeIndexTexture3].size());//Tableで利用する数

	// 平行光源
	rootParameters[6].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;   //CBVを使う
	rootParameters[6].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;//PixelShaderで使う
	rootParameters[6].Descriptor.ShaderRegister = 1;
	// カメラ
	rootParameters[7].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;   //CBVを使う
	rootParameters[7].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;//PixelShaderで使う
	rootParameters[7].Descriptor.ShaderRegister = 2;
	// ポイントライト
	rootParameters[8].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;//DescriptorTableを使う
	rootParameters[8].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;//PixelShaderで使う
	rootParameters[8].DescriptorTable.pDescriptorRanges = descriptorRanges_[kDescriptorRangeIndexPointLight].data();//Tableの中身の配列を指定
	rootParameters[8].DescriptorTable.NumDescriptorRanges = static_cast<uint32_t>(descriptorRanges_[kDescriptorRangeIndexPointLight].size());//Tableで利用する数
	// スポットライト
	rootParameters[9].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;//DescriptorTableを使う
	rootParameters[9].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;//PixelShaderで使う
	rootParameters[9].DescriptorTable.pDescriptorRanges = descriptorRanges_[kDescriptorRangeIndexSpotLight].data();//Tableの中身の配列を指定
	rootParameters[9].DescriptorTable.NumDescriptorRanges = static_cast<uint32_t>(descriptorRanges_[kDescriptorRangeIndexSpotLight].size());//Tableで利用する数
	// ワールドトランスフォーム
	rootParameters[10].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;//DescriptorTableを使う
	rootParameters[10].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;//VertexShaderで使う
	rootParameters[10].DescriptorTable.pDescriptorRanges = descriptorRanges_[kDexcriptorRangeIndexTransformationMatrix].data();//Tableの中身の配列を指定
	rootParameters[10].DescriptorTable.NumDescriptorRanges = static_cast<uint32_t>(descriptorRanges_[kDexcriptorRangeIndexTransformationMatrix].size());//Tableで利用する数

	// 霧
	rootParameters[11].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;   //CBVを使う
	rootParameters[11].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;//PixelShaderで使う
	rootParameters[11].Descriptor.ShaderRegister = 3;

	for (uint32_t i = 0; i < _countof(rootParameters); ++i) {
		rootParameters_[kRootParameterIndexManyModels].push_back(rootParameters[i]);
	}

}

void RootParameterManager::DescriptorRangeInitialize()
{

	D3D12_DESCRIPTOR_RANGE descriptorRange[1] = {};
	descriptorRange[0].BaseShaderRegister = 0;//0から始まる
	descriptorRange[0].NumDescriptors = 1;//数は一つ
	descriptorRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;//SRVを使う
	descriptorRange[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;//Offsetを自動計算

	descriptorRanges_[kDescriptorRangeIndexTexture0].push_back(descriptorRange[0]);

	D3D12_DESCRIPTOR_RANGE descriptorRangeTexture1[1] = {};
	descriptorRangeTexture1[0].BaseShaderRegister = 1;//1から始まる
	descriptorRangeTexture1[0].NumDescriptors = 1;//数は一つ
	descriptorRangeTexture1[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;//SRVを使う
	descriptorRangeTexture1[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;//Offsetを自動計算

	descriptorRanges_[kDescriptorRangeIndexTexture1].push_back(descriptorRangeTexture1[0]);

	D3D12_DESCRIPTOR_RANGE descriptorRangeTexture2[1] = {};
	descriptorRangeTexture2[0].BaseShaderRegister = 2;//2から始まる
	descriptorRangeTexture2[0].NumDescriptors = 1;//数は一つ
	descriptorRangeTexture2[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;//SRVを使う
	descriptorRangeTexture2[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;//Offsetを自動計算

	descriptorRanges_[kDescriptorRangeIndexTexture2].push_back(descriptorRangeTexture2[0]);

	D3D12_DESCRIPTOR_RANGE descriptorRangeTexture3[1] = {};
	descriptorRangeTexture3[0].BaseShaderRegister = 3;//3から始まる
	descriptorRangeTexture3[0].NumDescriptors = 1;//数は一つ
	descriptorRangeTexture3[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;//SRVを使う
	descriptorRangeTexture3[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;//Offsetを自動計算

	descriptorRanges_[kDescriptorRangeIndexTexture3].push_back(descriptorRangeTexture3[0]);

	D3D12_DESCRIPTOR_RANGE descriptorRangeForInstancing[1] = {};
	descriptorRangeForInstancing[0].BaseShaderRegister = 0;//0から始まる
	descriptorRangeForInstancing[0].NumDescriptors = 1;//数は一つ
	descriptorRangeForInstancing[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;//SRVを使う
	descriptorRangeForInstancing[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;//Offsetを自動計算

	descriptorRanges_[kDescriptorRangeIndexInstancing].push_back(descriptorRangeForInstancing[0]);
	
	D3D12_DESCRIPTOR_RANGE descriptorRangeForPointLight[1] = {};
	descriptorRangeForPointLight[0].BaseShaderRegister = 4;//1から始まる
	descriptorRangeForPointLight[0].NumDescriptors = 1;//数は一つ
	descriptorRangeForPointLight[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;//SRVを使う
	descriptorRangeForPointLight[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;//Offsetを自動計算

	descriptorRanges_[kDescriptorRangeIndexPointLight].push_back(descriptorRangeForPointLight[0]);

	D3D12_DESCRIPTOR_RANGE descriptorRangeForSpotLight[1] = {};
	descriptorRangeForSpotLight[0].BaseShaderRegister = 5;//2から始まる
	descriptorRangeForSpotLight[0].NumDescriptors = 1;//数は一つ
	descriptorRangeForSpotLight[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;//SRVを使う
	descriptorRangeForSpotLight[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;//Offsetを自動計算

	descriptorRanges_[kDescriptorRangeIndexSpotLight].push_back(descriptorRangeForSpotLight[0]);

	D3D12_DESCRIPTOR_RANGE descriptorRangeForLocalMatrix[1] = {};
	descriptorRangeForLocalMatrix[0].BaseShaderRegister = 0;//0から始まる
	descriptorRangeForLocalMatrix[0].NumDescriptors = 1;//数は一つ
	descriptorRangeForLocalMatrix[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;//SRVを使う
	descriptorRangeForLocalMatrix[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;//Offsetを自動計算

	descriptorRanges_[kDescriptorRangeIndexLocalMatrix].push_back(descriptorRangeForLocalMatrix[0]);

	D3D12_DESCRIPTOR_RANGE descriptorRangeForCollider2DDebugDrawForGPU[1] = {};
	descriptorRangeForCollider2DDebugDrawForGPU[0].BaseShaderRegister = 0;//0から始まる
	descriptorRangeForCollider2DDebugDrawForGPU[0].NumDescriptors = 1;//数は一つ
	descriptorRangeForCollider2DDebugDrawForGPU[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;//SRVを使う
	descriptorRangeForCollider2DDebugDrawForGPU[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;//Offsetを自動計算

	descriptorRanges_[kDexcriptorRangeIndexCollider2DDebugDrawForGPU].push_back(descriptorRangeForCollider2DDebugDrawForGPU[0]);

	D3D12_DESCRIPTOR_RANGE descriptorRangeForTransformMatrix[1] = {};
	descriptorRangeForTransformMatrix[0].BaseShaderRegister = 1;//0から始まる
	descriptorRangeForTransformMatrix[0].NumDescriptors = 1;//数は一つ
	descriptorRangeForTransformMatrix[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;//SRVを使う
	descriptorRangeForTransformMatrix[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;//Offsetを自動計算

	descriptorRanges_[kDexcriptorRangeIndexTransformationMatrix].push_back(descriptorRangeForTransformMatrix[0]);

	D3D12_DESCRIPTOR_RANGE descriptorRangeForLineForGPU[1] = {};
	descriptorRangeForLineForGPU[0].BaseShaderRegister = 0;//0から始まる
	descriptorRangeForLineForGPU[0].NumDescriptors = 1;//数は一つ
	descriptorRangeForLineForGPU[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;//SRVを使う
	descriptorRangeForLineForGPU[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;//Offsetを自動計算

	descriptorRanges_[kDexcriptorRangeIndexLineForGPU].push_back(descriptorRangeForLineForGPU[0]);

}
