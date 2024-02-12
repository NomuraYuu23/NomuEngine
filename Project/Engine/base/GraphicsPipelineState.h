#pragma once
#include <d3d12.h>
#include <string>
#include <dxcapi.h>
#include <cassert>
#include <format>
#include <wrl.h>
class GraphicsPipelineState
{

public: 

	enum PipelineStateName {
		kModel, // モデル
		kSprite, //スプライト
		kParticle, // パーティクル
		kOutLine, //アウトライン
		kCountOfPipelineStateName // 使わない
	};

public:

	static void Initialize(ID3D12Device* sDevice);

	// ルートシグネチャ
	static Microsoft::WRL::ComPtr<ID3D12RootSignature> sRootSignature[GraphicsPipelineState::PipelineStateName::kCountOfPipelineStateName];
	// パイプラインステートオブジェクト
	static Microsoft::WRL::ComPtr<ID3D12PipelineState> sPipelineState[GraphicsPipelineState::PipelineStateName::kCountOfPipelineStateName];

private:

	static void Log(const std::string& message);

	static IDxcBlob* CompileShader(
		//CompilerするShanderファイルへのパス
		const std::wstring& filePath,
		//Compilenに使用するProfile
		const wchar_t* profile,
		//初期化で生成したものを3つ
		IDxcUtils* dxcUtils,
		IDxcCompiler3* dxCompiler,
		IDxcIncludeHandler* includeHandler);

	static std::wstring ConvertString(const std::string& str);

	static std::string ConvertString(const std::wstring& str);

	// グラフィックスパイプライン作成
	// モデル用
	static void CreateForModel(ID3D12Device* sDevice);
	// スプライト用
	static void CreateForSprite(ID3D12Device* sDevice);
	// パーティクル用
	static void CreateForParticle(ID3D12Device* sDevice);
	// アウトライン用
	static void CreateForOutLine(ID3D12Device* sDevice);

};
