#include "RootParameterManager.h"

using namespace Microsoft::WRL;

// ルートパラメータ
std::array<std::vector<D3D12_ROOT_PARAMETER>, kRootParameterIndexOfCount> RootParameterManager::rootParameters_;
// ディスクリプタレンジPS
std::array<std::vector<D3D12_DESCRIPTOR_RANGE>, kDescriptorRangePSIndexOfCount> RootParameterManager::descriptorRangesPS_;
// ディスクリプタレンジVS
std::array<std::vector<D3D12_DESCRIPTOR_RANGE>, kDescriptorRangeVSIndexOfCount> RootParameterManager::descriptorRangesVS_;

RootParameterManager* RootParameterManager::GetInstance()
{
	static RootParameterManager instance;
	return &instance;
}

void RootParameterManager::Initialize()
{

	// ディスクリプタレンジ
	DescriptorRangeInitialize();

	// データ
	std::vector<Item> data;

#pragma region アニメーションありモデル
	//CreateForAnimModel();
	data.push_back(kCBV_PSIndexMaterial); // マテリアル 
	data.push_back(kCBV_PSIndexDirectionalLight); // 平行光源
	data.push_back(kCBV_PSIndexCamera); // カメラ
	data.push_back(kCBV_VSIndexWorldTransform); // ワールドトランスフォーム
	data.push_back(kDescriptorRangeVSIndexLocalMatrix); // ローカルトランスフォーム
	data.push_back(kDescriptorRangePSIndexTexture0); // テクスチャ
	data.push_back(kDescriptorRangePSIndexTexture1); // テクスチャ
	data.push_back(kDescriptorRangePSIndexTexture2); // テクスチャ
	data.push_back(kDescriptorRangePSIndexTexture3); // テクスチャ
	data.push_back(kDescriptorRangePSIndexTexture4); // テクスチャ
	data.push_back(kDescriptorRangePSIndexTexture5); // テクスチャ
	data.push_back(kDescriptorRangePSIndexTexture6); // テクスチャ
	data.push_back(kDescriptorRangePSIndexTexture7); // テクスチャ
	data.push_back(kDescriptorRangePSIndexPointLight); // ポイントライト
	data.push_back(kDescriptorRangePSIndexSpotLight); // スポットライト
	data.push_back(kCBV_PSIndexFog); // 霧
	Analyze(kRootParameterIndexAnimModel, data); // 解析
	data.clear(); // クリア

#pragma endregion

#pragma region アニメーションなしモデル
	//CreateForNormalModel();
	data.push_back(kCBV_PSIndexMaterial); // マテリアル 
	data.push_back(kCBV_PSIndexDirectionalLight); // 平行光源
	data.push_back(kCBV_PSIndexCamera); // カメラ
	data.push_back(kCBV_VSIndexWorldTransform); // ワールドトランスフォーム
	data.push_back(kDescriptorRangePSIndexTexture0); // テクスチャ
	data.push_back(kDescriptorRangePSIndexTexture1); // テクスチャ
	data.push_back(kDescriptorRangePSIndexTexture2); // テクスチャ
	data.push_back(kDescriptorRangePSIndexTexture3); // テクスチャ
	data.push_back(kDescriptorRangePSIndexTexture4); // テクスチャ
	data.push_back(kDescriptorRangePSIndexTexture5); // テクスチャ
	data.push_back(kDescriptorRangePSIndexTexture6); // テクスチャ
	data.push_back(kDescriptorRangePSIndexTexture7); // テクスチャ
	data.push_back(kDescriptorRangePSIndexPointLight); // ポイントライト
	data.push_back(kDescriptorRangePSIndexSpotLight); // スポットライト
	data.push_back(kCBV_PSIndexFog); // 霧
	Analyze(kRootParameterIndexNormalModel, data); // 解析
	data.clear(); // クリア
#pragma endregion

#pragma region アニメーションなしアウトライン
	//CreateForNormalOutline();
	data.push_back(kCBV_VSIndexWorldTransform); // ワールドトランスフォーム
	data.push_back(kCBV_VSIndexOutlineData); // アウトラインデータ
	Analyze(kRootParameterIndexNormalOutline, data); // 解析
	data.clear(); // クリア
#pragma endregion

#pragma region スプライト
	//RootParameterInitializeForSprite();
	data.push_back(kCBV_VSIndexSpriteForGPU); // スプライト用
	data.push_back(kCBV_PSIndexMaterial); // マテリアル
	data.push_back(kDescriptorRangePSIndexTexture0); // テクスチャ
	Analyze(kRootParameterIndexSprite, data); // 解析
	data.clear(); // クリア
#pragma endregion

#pragma region パーティクル
	//RootParameterInitializeForParticle();
	data.push_back(kCBV_PSIndexMaterial); // マテリアル
	data.push_back(kDescriptorRangeVSIndexParticleForGPU); // パーティクル用
	data.push_back(kDescriptorRangePSIndexTexture0); // テクスチャ
	data.push_back(kCBV_VSIndexParticleStart); // 開始位置
	Analyze(kRootParameterIndexParticle, data); // 解析
	data.clear(); // クリア
#pragma endregion

#pragma region コライダーデバッグ2D
	//RootParameterInitializeForCollision2DDebugDraw();
	data.push_back(kCBV_PSIndexMaterial); // マテリアル
	data.push_back(kDescriptorRangeVSIndexCollider2DDebugDrawForGPU); // コライダーデバッグ2D用
	data.push_back(kDescriptorRangePSIndexTexture0); // テクスチャ
	data.push_back(kDescriptorRangePSIndexTexture1); // テクスチャ
	Analyze(kRootParameterIndexCollision2DDebugDraw, data); // 解析
	data.clear(); // クリア
#pragma endregion

#pragma region 線
	//RootParameterInitializeForLine();
	data.push_back(kCBV_VSIndexViewProjection); // VP
	data.push_back(kDescriptorRangeVSIndexLineForGPU); // LineForGPU
	Analyze(kRootParameterIndexLine, data); // 解析
	data.clear(); // クリア
#pragma endregion

#pragma region ウィンドウスプライト
	//RootParameterInitializeForSwapChain();
	data.push_back(kDescriptorRangePSIndexTexture0); // テクスチャ
	Analyze(kRootParameterIndexWindowSprite, data); // 解析
	data.clear(); // クリア
#pragma endregion

#pragma region たくさんのアニメーションモデル
	//RootParameterInitializeForManyAnimModels();
	data.push_back(kDescriptorRangePSIndexMaterials); // マテリアル
	data.push_back(kDescriptorRangeVSIndexLocalMatrix); // ローカルトランスフォーム
	data.push_back(kDescriptorRangePSIndexTexture0); // テクスチャ
	data.push_back(kDescriptorRangePSIndexTexture1); // テクスチャ
	data.push_back(kDescriptorRangePSIndexTexture2); // テクスチャ
	data.push_back(kDescriptorRangePSIndexTexture3); // テクスチャ
	data.push_back(kDescriptorRangePSIndexTexture4); // テクスチャ
	data.push_back(kDescriptorRangePSIndexTexture5); // テクスチャ
	data.push_back(kDescriptorRangePSIndexTexture6); // テクスチャ
	data.push_back(kDescriptorRangePSIndexTexture7); // テクスチャ
	data.push_back(kCBV_PSIndexDirectionalLight); // 平行光源 
	data.push_back(kCBV_PSIndexCamera); //  カメラ
	data.push_back(kDescriptorRangePSIndexPointLight); // ポイントライト
	data.push_back(kDescriptorRangePSIndexSpotLight); // スポットライト
	data.push_back(kDescriptorRangeVSIndexTransformationMatrix); // ワールドトランスフォーム
	data.push_back(kCBV_PSIndexFog); // 霧
	Analyze(kRootParameterIndexManyAnimModels, data); // 解析
	data.clear(); // クリア
#pragma endregion

#pragma region たくさんのアニメーション無しモデル
	// RootParameterInitializeForManyNormalModels();
	data.push_back(kDescriptorRangePSIndexMaterials); // マテリアル
	data.push_back(kDescriptorRangePSIndexTexture0); // テクスチャ
	data.push_back(kDescriptorRangePSIndexTexture1); // テクスチャ
	data.push_back(kDescriptorRangePSIndexTexture2); // テクスチャ
	data.push_back(kDescriptorRangePSIndexTexture3); // テクスチャ
	data.push_back(kDescriptorRangePSIndexTexture4); // テクスチャ
	data.push_back(kDescriptorRangePSIndexTexture5); // テクスチャ
	data.push_back(kDescriptorRangePSIndexTexture6); // テクスチャ
	data.push_back(kDescriptorRangePSIndexTexture7); // テクスチャ
	data.push_back(kCBV_PSIndexDirectionalLight); // 平行光源 
	data.push_back(kCBV_PSIndexCamera); //  カメラ
	data.push_back(kDescriptorRangePSIndexPointLight); // ポイントライト
	data.push_back(kDescriptorRangePSIndexSpotLight); // スポットライト
	data.push_back(kDescriptorRangeVSIndexTransformationMatrix); // ワールドトランスフォーム
	data.push_back(kCBV_PSIndexFog); // 霧
	Analyze(kRootParameterIndexManyNormalModels, data); // 解析
	data.clear(); // クリア
#pragma endregion

	rootParameters_;

}

//void RootParameterManager::CreateForAnimModel()
//{
//
//	//RootParameter作成
//	D3D12_ROOT_PARAMETER rootParameters[12] = {};
//	
//	// マテリアル
//	rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;   //CBVを使う
//	rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;//PixelShaderで使う
//	rootParameters[0].Descriptor.ShaderRegister = 0;                   //レジスタ番号0とバインド
//
//	// 平行光源
//	rootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;   //CBVを使う
//	rootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;//PixelShaderで使う
//	rootParameters[1].Descriptor.ShaderRegister = 1;
//	
//	// カメラ
//	rootParameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;   //CBVを使う
//	rootParameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;//PixelShaderで使う
//	rootParameters[2].Descriptor.ShaderRegister = 2;
//
//	// ワールドトランスフォーム
//	rootParameters[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;   //CBVを使う
//	rootParameters[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;//VertexShaderで使う
//	rootParameters[3].Descriptor.ShaderRegister = 0;
//
//	// トランスフォームマトリックス
//	rootParameters[4].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;//DescriptorTableを使う
//	rootParameters[4].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;//VertexShaderで使う
//	rootParameters[4].DescriptorTable.pDescriptorRanges = descriptorRanges_[kDescriptorRangeIndexLocalMatrix].data();//Tableの中身の配列を指定
//	rootParameters[4].DescriptorTable.NumDescriptorRanges = static_cast<uint32_t>(descriptorRanges_[kDescriptorRangeIndexLocalMatrix].size());//Tableで利用する数
//	// テクスチャ
//	rootParameters[5].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;//DescriptorTableを使う
//	rootParameters[5].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;//PixelShaderで使う
//	rootParameters[5].DescriptorTable.pDescriptorRanges = descriptorRanges_[kDescriptorRangeIndexTexture0].data();//Tableの中身の配列を指定
//	rootParameters[5].DescriptorTable.NumDescriptorRanges = static_cast<uint32_t>(descriptorRanges_[kDescriptorRangeIndexTexture0].size());//Tableで利用する数
//
//	rootParameters[6].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;//DescriptorTableを使う
//	rootParameters[6].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;//PixelShaderで使う
//	rootParameters[6].DescriptorTable.pDescriptorRanges = descriptorRanges_[kDescriptorRangeIndexTexture1].data();//Tableの中身の配列を指定
//	rootParameters[6].DescriptorTable.NumDescriptorRanges = static_cast<uint32_t>(descriptorRanges_[kDescriptorRangeIndexTexture1].size());//Tableで利用する数
//
//	rootParameters[7].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;//DescriptorTableを使う
//	rootParameters[7].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;//PixelShaderで使う
//	rootParameters[7].DescriptorTable.pDescriptorRanges = descriptorRanges_[kDescriptorRangeIndexTexture2].data();//Tableの中身の配列を指定
//	rootParameters[7].DescriptorTable.NumDescriptorRanges = static_cast<uint32_t>(descriptorRanges_[kDescriptorRangeIndexTexture2].size());//Tableで利用する数
//
//	rootParameters[8].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;//DescriptorTableを使う
//	rootParameters[8].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;//PixelShaderで使う
//	rootParameters[8].DescriptorTable.pDescriptorRanges = descriptorRanges_[kDescriptorRangeIndexTexture3].data();//Tableの中身の配列を指定
//	rootParameters[8].DescriptorTable.NumDescriptorRanges = static_cast<uint32_t>(descriptorRanges_[kDescriptorRangeIndexTexture3].size());//Tableで利用する数
//
//	// ポイントライト
//	rootParameters[9].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;//DescriptorTableを使う
//	rootParameters[9].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;//PixelShaderで使う
//	rootParameters[9].DescriptorTable.pDescriptorRanges = descriptorRanges_[kDescriptorRangeIndexPointLight].data();//Tableの中身の配列を指定
//	rootParameters[9].DescriptorTable.NumDescriptorRanges = static_cast<uint32_t>(descriptorRanges_[kDescriptorRangeIndexPointLight].size());//Tableで利用する数
//	// スポットライト
//	rootParameters[10].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;//DescriptorTableを使う
//	rootParameters[10].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;//PixelShaderで使う
//	rootParameters[10].DescriptorTable.pDescriptorRanges = descriptorRanges_[kDescriptorRangeIndexSpotLight].data();//Tableの中身の配列を指定
//	rootParameters[10].DescriptorTable.NumDescriptorRanges = static_cast<uint32_t>(descriptorRanges_[kDescriptorRangeIndexSpotLight].size());//Tableで利用する数
//
//	// 霧
//	rootParameters[11].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;   //CBVを使う
//	rootParameters[11].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;//PixelShaderで使う
//	rootParameters[11].Descriptor.ShaderRegister = 3;
//
//	for (uint32_t i = 0; i < _countof(rootParameters); ++i) {
//		rootParameters_[kRootParameterIndexAnimModel].push_back(rootParameters[i]);
//	}
//
//}
//
//void RootParameterManager::CreateForNormalModel()
//{
//
//	//RootParameter作成
//	D3D12_ROOT_PARAMETER rootParameters[11] = {};
//
//	// マテリアル
//	rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;   //CBVを使う
//	rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;//PixelShaderで使う
//	rootParameters[0].Descriptor.ShaderRegister = 0;                   //レジスタ番号0とバインド
//
//	// 平行光源
//	rootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;   //CBVを使う
//	rootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;//PixelShaderで使う
//	rootParameters[1].Descriptor.ShaderRegister = 1;
//
//	// カメラ
//	rootParameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;   //CBVを使う
//	rootParameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;//PixelShaderで使う
//	rootParameters[2].Descriptor.ShaderRegister = 2;
//
//	// ワールドトランスフォーム
//	rootParameters[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;   //CBVを使う
//	rootParameters[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;//VertexShaderで使う
//	rootParameters[3].Descriptor.ShaderRegister = 0;
//	
//	// テクスチャ
//	rootParameters[4].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;//DescriptorTableを使う
//	rootParameters[4].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;//PixelShaderで使う
//	rootParameters[4].DescriptorTable.pDescriptorRanges = descriptorRanges_[kDescriptorRangeIndexTexture0].data();//Tableの中身の配列を指定
//	rootParameters[4].DescriptorTable.NumDescriptorRanges = static_cast<uint32_t>(descriptorRanges_[kDescriptorRangeIndexTexture0].size());//Tableで利用する数
//
//	rootParameters[5].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;//DescriptorTableを使う
//	rootParameters[5].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;//PixelShaderで使う
//	rootParameters[5].DescriptorTable.pDescriptorRanges = descriptorRanges_[kDescriptorRangeIndexTexture1].data();//Tableの中身の配列を指定
//	rootParameters[5].DescriptorTable.NumDescriptorRanges = static_cast<uint32_t>(descriptorRanges_[kDescriptorRangeIndexTexture1].size());//Tableで利用する数
//
//	rootParameters[6].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;//DescriptorTableを使う
//	rootParameters[6].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;//PixelShaderで使う
//	rootParameters[6].DescriptorTable.pDescriptorRanges = descriptorRanges_[kDescriptorRangeIndexTexture2].data();//Tableの中身の配列を指定
//	rootParameters[6].DescriptorTable.NumDescriptorRanges = static_cast<uint32_t>(descriptorRanges_[kDescriptorRangeIndexTexture2].size());//Tableで利用する数
//
//	rootParameters[7].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;//DescriptorTableを使う
//	rootParameters[7].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;//PixelShaderで使う
//	rootParameters[7].DescriptorTable.pDescriptorRanges = descriptorRanges_[kDescriptorRangeIndexTexture3].data();//Tableの中身の配列を指定
//	rootParameters[7].DescriptorTable.NumDescriptorRanges = static_cast<uint32_t>(descriptorRanges_[kDescriptorRangeIndexTexture3].size());//Tableで利用する数
//
//	// ポイントライト
//	rootParameters[8].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;//DescriptorTableを使う
//	rootParameters[8].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;//PixelShaderで使う
//	rootParameters[8].DescriptorTable.pDescriptorRanges = descriptorRanges_[kDescriptorRangeIndexPointLight].data();//Tableの中身の配列を指定
//	rootParameters[8].DescriptorTable.NumDescriptorRanges = static_cast<uint32_t>(descriptorRanges_[kDescriptorRangeIndexPointLight].size());//Tableで利用する数
//	// スポットライト
//	rootParameters[9].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;//DescriptorTableを使う
//	rootParameters[9].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;//PixelShaderで使う
//	rootParameters[9].DescriptorTable.pDescriptorRanges = descriptorRanges_[kDescriptorRangeIndexSpotLight].data();//Tableの中身の配列を指定
//	rootParameters[9].DescriptorTable.NumDescriptorRanges = static_cast<uint32_t>(descriptorRanges_[kDescriptorRangeIndexSpotLight].size());//Tableで利用する数
//
//	// 霧
//	rootParameters[10].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;   //CBVを使う
//	rootParameters[10].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;//PixelShaderで使う
//	rootParameters[10].Descriptor.ShaderRegister = 3;
//
//	for (uint32_t i = 0; i < _countof(rootParameters); ++i) {
//		rootParameters_[kRootParameterIndexNormalModel].push_back(rootParameters[i]);
//	}
//
//}
//
//void RootParameterManager::CreateForNormalOutline()
//{
//
//	//RootParameter作成
//	D3D12_ROOT_PARAMETER rootParameters[2] = {};
//
//	// ワールドトランスフォーム
//	rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;   //CBVを使う
//	rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;//VertexShaderで使う
//	rootParameters[0].Descriptor.ShaderRegister = 0;
//
//	// アウトラインデータ
//	rootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;   //CBVを使う
//	rootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;//VertexShaderで使う
//	rootParameters[1].Descriptor.ShaderRegister = 1;
//
//	for (uint32_t i = 0; i < _countof(rootParameters); ++i) {
//		rootParameters_[kRootParameterIndexNormalOutline].push_back(rootParameters[i]);
//	}
//
//}
//
//void RootParameterManager::RootParameterInitializeForSprite()
//{
//
//	//RootParameter作成
//	D3D12_ROOT_PARAMETER rootParameters[4] = {};
//	rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;   //CBVを使う
//	rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;//PixelShaderで使う
//	rootParameters[0].Descriptor.ShaderRegister = 0;                   //レジスタ番号0とバインド
//
//	rootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;   //CBVを使う
//	rootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;//VertexShaderで使う
//	rootParameters[1].Descriptor.ShaderRegister = 0;                   //レジスタ番号0とバインド
//	
//	rootParameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;//DescriptorTableを使う
//	rootParameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;//PixelShaderで使う
//	rootParameters[2].DescriptorTable.pDescriptorRanges = descriptorRanges_[kDescriptorRangeIndexTexture0].data();//Tableの中身の配列を指定
//	rootParameters[2].DescriptorTable.NumDescriptorRanges = static_cast<uint32_t>(descriptorRanges_[kDescriptorRangeIndexTexture0].size());//Tableで利用する数
//	
//	rootParameters[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;   //CBVを使う
//	rootParameters[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;//VertexShaderで使う
//	rootParameters[3].Descriptor.ShaderRegister = 1;
//
//	for (uint32_t i = 0; i < 4; ++i) {
//		rootParameters_[kRootParameterIndexSprite].push_back(rootParameters[i]);
//	}
//
//}
//
//void RootParameterManager::RootParameterInitializeForParticle()
//{
//
//	//RootParameter作成。複数設定できるので配列。今回は1つだけなので長さ1の配列
//	D3D12_ROOT_PARAMETER rootParameters[5] = {};
//	
//	// マテリアル
//	rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;   //CBVを使う
//	rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;//PixelShaderで使う
//	rootParameters[0].Descriptor.ShaderRegister = 0;                   //レジスタ番号0とバインド
//	
//	// インスタンシング
//	rootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;//DescriptorTableを使う
//	rootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;//VertexShaderで使う
//	rootParameters[1].DescriptorTable.pDescriptorRanges = descriptorRanges_[kDescriptorRangeIndexInstancing].data();//Tableの中身の配列を指定
//	rootParameters[1].DescriptorTable.NumDescriptorRanges = static_cast<uint32_t>(descriptorRanges_[kDescriptorRangeIndexInstancing].size());//Tableで利用する数
//	
//	// テクスチャ
//	rootParameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;//DescriptorTableを使う
//	rootParameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;//PixelShaderで使う
//	rootParameters[2].DescriptorTable.pDescriptorRanges = descriptorRanges_[kDescriptorRangeIndexTexture0].data();//Tableの中身の配列を指定
//	rootParameters[2].DescriptorTable.NumDescriptorRanges = static_cast<uint32_t>(descriptorRanges_[kDescriptorRangeIndexTexture0].size());//Tableで利用する数
//	
//	// 開始位置
//	rootParameters[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;   //CBVを使う
//	rootParameters[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;//VertexShaderで使う
//	rootParameters[3].Descriptor.ShaderRegister = 0;
//
//	for (uint32_t i = 0; i < _countof(rootParameters); ++i) {
//		rootParameters_[kRootParameterIndexParticle].push_back(rootParameters[i]);
//	}
//
//}
//
//void RootParameterManager::RootParameterInitializeForCollision2DDebugDraw()
//{
//
//	//RootParameter作成。複数設定できるので配列。
//	D3D12_ROOT_PARAMETER rootParameters[4] = {};
//	// マテリアル
//	rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;   //CBVを使う
//	rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;//PixelShaderで使う
//	rootParameters[0].Descriptor.ShaderRegister = 0;                   //レジスタ番号0とバインド
//	// Collider2DDebugDrawForGPU
//	rootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;//DescriptorTableを使う
//	rootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;//VertexShaderで使う
//	rootParameters[1].DescriptorTable.pDescriptorRanges = descriptorRanges_[kDescriptorRangeIndexCollider2DDebugDrawForGPU].data();//Tableの中身の配列を指定
//	rootParameters[1].DescriptorTable.NumDescriptorRanges = static_cast<uint32_t>(descriptorRanges_[kDescriptorRangeIndexCollider2DDebugDrawForGPU].size());//Tableで利用する数
//	// テクスチャ
//	rootParameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;//DescriptorTableを使う
//	rootParameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;//PixelShaderで使う
//	rootParameters[2].DescriptorTable.pDescriptorRanges = descriptorRanges_[kDescriptorRangeIndexTexture0].data();//Tableの中身の配列を指定
//	rootParameters[2].DescriptorTable.NumDescriptorRanges = static_cast<uint32_t>(descriptorRanges_[kDescriptorRangeIndexTexture0].size());//Tableで利用する数
//
//	rootParameters[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;//DescriptorTableを使う
//	rootParameters[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;//PixelShaderで使う
//	rootParameters[3].DescriptorTable.pDescriptorRanges = descriptorRanges_[kDescriptorRangeIndexTexture1].data();//Tableの中身の配列を指定
//	rootParameters[3].DescriptorTable.NumDescriptorRanges = static_cast<uint32_t>(descriptorRanges_[kDescriptorRangeIndexTexture1].size());//Tableで利用する数
//
//	for (uint32_t i = 0; i < _countof(rootParameters); ++i) {
//		rootParameters_[kRootParameterIndexCollision2DDebugDraw].push_back(rootParameters[i]);
//	}
//
//
//}
//
//void RootParameterManager::RootParameterInitializeForLine()
//{
//
//	//RootParameter作成。複数設定できるので配列。
//	D3D12_ROOT_PARAMETER rootParameters[2] = {};
//	// VP
//	rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;   //CBVを使う
//	rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;//PixelShaderで使う
//	rootParameters[0].Descriptor.ShaderRegister = 0;                   //レジスタ番号0とバインド
//	// LineForGPU
//	rootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;//DescriptorTableを使う
//	rootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;//PixelShaderで使う
//	rootParameters[1].DescriptorTable.pDescriptorRanges = descriptorRanges_[kDescriptorRangeIndexLineForGPU].data();//Tableの中身の配列を指定
//	rootParameters[1].DescriptorTable.NumDescriptorRanges = static_cast<uint32_t>(descriptorRanges_[kDescriptorRangeIndexLineForGPU].size());//Tableで利用する数
//
//	for (uint32_t i = 0; i < _countof(rootParameters); ++i) {
//		rootParameters_[kRootParameterIndexLine].push_back(rootParameters[i]);
//	}
//
//}
//
//void RootParameterManager::RootParameterInitializeForSwapChain()
//{
//
//	//RootParameter作成。複数設定できるので配列。
//	D3D12_ROOT_PARAMETER rootParameters[1] = {};
//	// テクスチャ
//	rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;//DescriptorTableを使う
//	rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;//PixelShaderで使う
//	rootParameters[0].DescriptorTable.pDescriptorRanges = descriptorRanges_[kDescriptorRangeIndexTexture0].data();//Tableの中身の配列を指定
//	rootParameters[0].DescriptorTable.NumDescriptorRanges = static_cast<uint32_t>(descriptorRanges_[kDescriptorRangeIndexTexture0].size());//Tableで利用する数
//
//
//	for (uint32_t i = 0; i < _countof(rootParameters); ++i) {
//		rootParameters_[kRootParameterIndexWindowSprite].push_back(rootParameters[i]);
//	}
//
//}
//
//void RootParameterManager::RootParameterInitializeForManyAnimModels()
//{
//
//	//RootParameter作成
//	D3D12_ROOT_PARAMETER rootParameters[12] = {};
//	// マテリアル
//	rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;//DescriptorTableを使う
//	rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;//PixelShaderで使う
//	rootParameters[0].DescriptorTable.pDescriptorRanges = descriptorRanges_[kDescriptorRangeIndexMaterials].data();//Tableの中身の配列を指定
//	rootParameters[0].DescriptorTable.NumDescriptorRanges = static_cast<uint32_t>(descriptorRanges_[kDescriptorRangeIndexMaterials].size());//Tableで利用する数
//
//	// トランスフォームマトリックス
//	rootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;//DescriptorTableを使う
//	rootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;//VertexShaderで使う
//	rootParameters[1].DescriptorTable.pDescriptorRanges = descriptorRanges_[kDescriptorRangeIndexLocalMatrix].data();//Tableの中身の配列を指定
//	rootParameters[1].DescriptorTable.NumDescriptorRanges = static_cast<uint32_t>(descriptorRanges_[kDescriptorRangeIndexLocalMatrix].size());//Tableで利用する数
//	// テクスチャ
//	rootParameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;//DescriptorTableを使う
//	rootParameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;//PixelShaderで使う
//	rootParameters[2].DescriptorTable.pDescriptorRanges = descriptorRanges_[kDescriptorRangeIndexTexture0].data();//Tableの中身の配列を指定
//	rootParameters[2].DescriptorTable.NumDescriptorRanges = static_cast<uint32_t>(descriptorRanges_[kDescriptorRangeIndexTexture0].size());//Tableで利用する数
//
//	rootParameters[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;//DescriptorTableを使う
//	rootParameters[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;//PixelShaderで使う
//	rootParameters[3].DescriptorTable.pDescriptorRanges = descriptorRanges_[kDescriptorRangeIndexTexture1].data();//Tableの中身の配列を指定
//	rootParameters[3].DescriptorTable.NumDescriptorRanges = static_cast<uint32_t>(descriptorRanges_[kDescriptorRangeIndexTexture1].size());//Tableで利用する数
//
//	rootParameters[4].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;//DescriptorTableを使う
//	rootParameters[4].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;//PixelShaderで使う
//	rootParameters[4].DescriptorTable.pDescriptorRanges = descriptorRanges_[kDescriptorRangeIndexTexture2].data();//Tableの中身の配列を指定
//	rootParameters[4].DescriptorTable.NumDescriptorRanges = static_cast<uint32_t>(descriptorRanges_[kDescriptorRangeIndexTexture2].size());//Tableで利用する数
//
//	rootParameters[5].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;//DescriptorTableを使う
//	rootParameters[5].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;//PixelShaderで使う
//	rootParameters[5].DescriptorTable.pDescriptorRanges = descriptorRanges_[kDescriptorRangeIndexTexture3].data();//Tableの中身の配列を指定
//	rootParameters[5].DescriptorTable.NumDescriptorRanges = static_cast<uint32_t>(descriptorRanges_[kDescriptorRangeIndexTexture3].size());//Tableで利用する数
//
//	// 平行光源
//	rootParameters[6].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;   //CBVを使う
//	rootParameters[6].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;//PixelShaderで使う
//	rootParameters[6].Descriptor.ShaderRegister = 0;
//	// カメラ
//	rootParameters[7].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;   //CBVを使う
//	rootParameters[7].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;//PixelShaderで使う
//	rootParameters[7].Descriptor.ShaderRegister = 1;
//	// ポイントライト
//	rootParameters[8].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;//DescriptorTableを使う
//	rootParameters[8].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;//PixelShaderで使う
//	rootParameters[8].DescriptorTable.pDescriptorRanges = descriptorRanges_[kDescriptorRangeIndexPointLight].data();//Tableの中身の配列を指定
//	rootParameters[8].DescriptorTable.NumDescriptorRanges = static_cast<uint32_t>(descriptorRanges_[kDescriptorRangeIndexPointLight].size());//Tableで利用する数
//	// スポットライト
//	rootParameters[9].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;//DescriptorTableを使う
//	rootParameters[9].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;//PixelShaderで使う
//	rootParameters[9].DescriptorTable.pDescriptorRanges = descriptorRanges_[kDescriptorRangeIndexSpotLight].data();//Tableの中身の配列を指定
//	rootParameters[9].DescriptorTable.NumDescriptorRanges = static_cast<uint32_t>(descriptorRanges_[kDescriptorRangeIndexSpotLight].size());//Tableで利用する数
//	// ワールドトランスフォーム
//	rootParameters[10].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;//DescriptorTableを使う
//	rootParameters[10].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;//VertexShaderで使う
//	rootParameters[10].DescriptorTable.pDescriptorRanges = descriptorRanges_[kDescriptorRangeIndexTransformationMatrix].data();//Tableの中身の配列を指定
//	rootParameters[10].DescriptorTable.NumDescriptorRanges = static_cast<uint32_t>(descriptorRanges_[kDescriptorRangeIndexTransformationMatrix].size());//Tableで利用する数
//
//	// 霧
//	rootParameters[11].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;   //CBVを使う
//	rootParameters[11].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;//PixelShaderで使う
//	rootParameters[11].Descriptor.ShaderRegister = 2;
//
//	for (uint32_t i = 0; i < _countof(rootParameters); ++i) {
//		rootParameters_[kRootParameterIndexManyAnimModels].push_back(rootParameters[i]);
//	}
//
//}
//
//void RootParameterManager::RootParameterInitializeForManyNormalModels()
//{
//
//	//RootParameter作成
//	D3D12_ROOT_PARAMETER rootParameters[11] = {};
//	// マテリアル
//	rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;//DescriptorTableを使う
//	rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;//PixelShaderで使う
//	rootParameters[0].DescriptorTable.pDescriptorRanges = descriptorRanges_[kDescriptorRangeIndexMaterials].data();//Tableの中身の配列を指定
//	rootParameters[0].DescriptorTable.NumDescriptorRanges = static_cast<uint32_t>(descriptorRanges_[kDescriptorRangeIndexMaterials].size());//Tableで利用する数
//
//	// テクスチャ
//	rootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;//DescriptorTableを使う
//	rootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;//PixelShaderで使う
//	rootParameters[1].DescriptorTable.pDescriptorRanges = descriptorRanges_[kDescriptorRangeIndexTexture0].data();//Tableの中身の配列を指定
//	rootParameters[1].DescriptorTable.NumDescriptorRanges = static_cast<uint32_t>(descriptorRanges_[kDescriptorRangeIndexTexture0].size());//Tableで利用する数
//
//	rootParameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;//DescriptorTableを使う
//	rootParameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;//PixelShaderで使う
//	rootParameters[2].DescriptorTable.pDescriptorRanges = descriptorRanges_[kDescriptorRangeIndexTexture1].data();//Tableの中身の配列を指定
//	rootParameters[2].DescriptorTable.NumDescriptorRanges = static_cast<uint32_t>(descriptorRanges_[kDescriptorRangeIndexTexture1].size());//Tableで利用する数
//
//	rootParameters[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;//DescriptorTableを使う
//	rootParameters[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;//PixelShaderで使う
//	rootParameters[3].DescriptorTable.pDescriptorRanges = descriptorRanges_[kDescriptorRangeIndexTexture2].data();//Tableの中身の配列を指定
//	rootParameters[3].DescriptorTable.NumDescriptorRanges = static_cast<uint32_t>(descriptorRanges_[kDescriptorRangeIndexTexture2].size());//Tableで利用する数
//
//	rootParameters[4].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;//DescriptorTableを使う
//	rootParameters[4].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;//PixelShaderで使う
//	rootParameters[4].DescriptorTable.pDescriptorRanges = descriptorRanges_[kDescriptorRangeIndexTexture3].data();//Tableの中身の配列を指定
//	rootParameters[4].DescriptorTable.NumDescriptorRanges = static_cast<uint32_t>(descriptorRanges_[kDescriptorRangeIndexTexture3].size());//Tableで利用する数
//
//	// 平行光源
//	rootParameters[5].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;   //CBVを使う
//	rootParameters[5].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;//PixelShaderで使う
//	rootParameters[5].Descriptor.ShaderRegister = 0;
//	// カメラ
//	rootParameters[6].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;   //CBVを使う
//	rootParameters[6].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;//PixelShaderで使う
//	rootParameters[6].Descriptor.ShaderRegister = 1;
//	// ポイントライト
//	rootParameters[7].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;//DescriptorTableを使う
//	rootParameters[7].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;//PixelShaderで使う
//	rootParameters[7].DescriptorTable.pDescriptorRanges = descriptorRanges_[kDescriptorRangeIndexPointLight].data();//Tableの中身の配列を指定
//	rootParameters[7].DescriptorTable.NumDescriptorRanges = static_cast<uint32_t>(descriptorRanges_[kDescriptorRangeIndexPointLight].size());//Tableで利用する数
//	// スポットライト
//	rootParameters[8].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;//DescriptorTableを使う
//	rootParameters[8].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;//PixelShaderで使う
//	rootParameters[8].DescriptorTable.pDescriptorRanges = descriptorRanges_[kDescriptorRangeIndexSpotLight].data();//Tableの中身の配列を指定
//	rootParameters[8].DescriptorTable.NumDescriptorRanges = static_cast<uint32_t>(descriptorRanges_[kDescriptorRangeIndexSpotLight].size());//Tableで利用する数
//	// ワールドトランスフォーム
//	rootParameters[9].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;//DescriptorTableを使う
//	rootParameters[9].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;//VertexShaderで使う
//	rootParameters[9].DescriptorTable.pDescriptorRanges = descriptorRanges_[kDescriptorRangeIndexTransformationMatrix].data();//Tableの中身の配列を指定
//	rootParameters[9].DescriptorTable.NumDescriptorRanges = static_cast<uint32_t>(descriptorRanges_[kDescriptorRangeIndexTransformationMatrix].size());//Tableで利用する数
//
//	// 霧
//	rootParameters[10].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;   //CBVを使う
//	rootParameters[10].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;//PixelShaderで使う
//	rootParameters[10].Descriptor.ShaderRegister = 2;
//
//	for (uint32_t i = 0; i < _countof(rootParameters); ++i) {
//		rootParameters_[kRootParameterIndexManyNormalModels].push_back(rootParameters[i]);
//	}
//
//}

void RootParameterManager::DescriptorRangeInitialize()
{

	// PS
	for (uint32_t i = 0; i < kDescriptorRangePSIndexOfCount; ++i) {

		D3D12_DESCRIPTOR_RANGE descriptorRange[1] = {};
		descriptorRange[0].BaseShaderRegister = i;//iから始まる
		descriptorRange[0].NumDescriptors = 1;//数は一つ
		descriptorRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;//SRVを使う
		descriptorRange[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;//Offsetを自動計算

		descriptorRangesPS_[i].push_back(descriptorRange[0]);
	}

	// VS
	for (uint32_t i = 0; i < kDescriptorRangeVSIndexOfCount; ++i) {

		D3D12_DESCRIPTOR_RANGE descriptorRange[1] = {};
		descriptorRange[0].BaseShaderRegister = i;//iから始まる
		descriptorRange[0].NumDescriptors = 1;//数は一つ
		descriptorRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;//SRVを使う
		descriptorRange[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;//Offsetを自動計算

		descriptorRangesVS_[i].push_back(descriptorRange[0]);
	}

}

void RootParameterManager::Analyze(RootParameterIndex rootParameterIndex, const std::vector<Item>& data)
{

	for (uint32_t i = 0; i < data.size(); ++i) {
		Item item = data[i];

		D3D12_ROOT_PARAMETER rootParameter{};

		if (std::holds_alternative<DescriptorRangePSIndex>(item)) {

			DescriptorRangePSIndex index = std::get<DescriptorRangePSIndex>(item);
			rootParameter.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;//DescriptorTableを使う
			rootParameter.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;//PixelShaderで使う
			rootParameter.DescriptorTable.pDescriptorRanges = descriptorRangesPS_[index].data();//Tableの中身の配列を指定
			rootParameter.DescriptorTable.NumDescriptorRanges = static_cast<uint32_t>(descriptorRangesPS_[index].size());//Tableで利用する数
		
		}
		else if (std::holds_alternative<DescriptorRangeVSIndex>(item)) {
			
			DescriptorRangeVSIndex index = std::get<DescriptorRangeVSIndex>(item);
			rootParameter.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;//DescriptorTableを使う
			rootParameter.ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;//VertexShaderで使う
			rootParameter.DescriptorTable.pDescriptorRanges = descriptorRangesVS_[index].data();//Tableの中身の配列を指定
			rootParameter.DescriptorTable.NumDescriptorRanges = static_cast<uint32_t>(descriptorRangesVS_[index].size());//Tableで利用する数

		}
		else if (std::holds_alternative<CBV_PSIndex>(item)) {
			CBV_PSIndex index = std::get<CBV_PSIndex>(item);
			rootParameter.ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;   //CBVを使う
			rootParameter.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;//PixelShaderで使う
			rootParameter.Descriptor.ShaderRegister = index; //レジスタ番号indexとバインド
		}
		else if (std::holds_alternative<CBV_VSIndex>(item)) {
			CBV_VSIndex index = std::get<CBV_VSIndex>(item);
			rootParameter.ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;   //CBVを使う
			rootParameter.ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;//VertexShaderで使う
			rootParameter.Descriptor.ShaderRegister = index;//レジスタ番号indexとバインド
		}

		rootParameters_[rootParameterIndex].push_back(rootParameter);

	}

}
