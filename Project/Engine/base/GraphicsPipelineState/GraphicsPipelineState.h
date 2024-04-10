#pragma once
#include <d3d12.h>
#include <string>
#include <dxcapi.h>
#include <cassert>
#include <format>
#include <wrl.h>
#include <array>
#include <vector>

#include "RootParameterManager.h"
#include "SamplerManager.h"
#include "InputLayoutManager.h"
#include "BlendStateManager.h"

class GraphicsPipelineState
{

public: // 変数

	enum PipelineStateName {
		kPipelineStateNameModel, // モデル
		kPipelineStateNameSprite, //スプライト
		kPipelineStateNameParticle, // パーティクル
		kPipelineStateNameOutLine, //アウトライン
		kPipelineStateNameCollision2DDebugDraw, // コライダーデバッグ2D
		kPipelineStateNameLine, // 線
		kPipelineStateNameWindowSprite, // スワップチェーン
		kPipelineStateNameManyModels, // たくさんのモデル
		kPipelineStateNameOfCount // 使わない
	};

	// ルートシグネチャ
	static Microsoft::WRL::ComPtr<ID3D12RootSignature> sRootSignature[GraphicsPipelineState::PipelineStateName::kPipelineStateNameOfCount];
	// パイプラインステートオブジェクト
	static Microsoft::WRL::ComPtr<ID3D12PipelineState> sPipelineState[GraphicsPipelineState::PipelineStateName::kPipelineStateNameOfCount];

public: // 関数

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="sDevice">デバイス</param>
	static void Initialize(ID3D12Device* sDevice);

private: // グラフィックスパイプライン作成関数

	/// <summary>
	/// モデル
	/// </summary>
	static void CreateForModel();
	/// <summary>
	/// スプライト
	/// </summary>
	static void CreateForSprite();
	/// <summary>
	/// パーティクル
	/// </summary>
	static void CreateForParticle();
	/// <summary>
	/// アウトライン
	/// </summary>
	static void CreateForOutLine();
	/// <summary>
	/// コライダーデバッグ2D
	/// </summary>
	static void CreateForCollision2DDebugDraw();
	/// <summary>
	/// 線
	/// </summary>
	static void CreateForLine();
	/// <summary>
	/// ポストエフェクト
	/// </summary>
	static void CreateForSwapChain();
	/// <summary>
	/// たくさんのモデル
	/// </summary>
	static void CreateForManyModels();

private: // 以下パイプラインの変数やenum

	// デバイス
	static ID3D12Device* sDevice_;

	/// <summary>
	/// PSO用
	/// </summary>
	struct CreatePSODesc {
		PipelineStateName pipelineStateName; // パイプラインステートの名前
		D3D12_INPUT_LAYOUT_DESC inputLayoutDesc{}; //InputLayout
		IDxcBlob* vertexShaderBlob = nullptr; //VertexShader
		IDxcBlob* pixelShaderBlob = nullptr; //PixelShader
		D3D12_BLEND_DESC blendDesc{};//BlendStateの設定
		D3D12_RASTERIZER_DESC rasterizerDesc{};	//ResiterzerStateの設定
		uint32_t numRenderTargets; //RTVの情報
		DXGI_FORMAT RTVFormats; //RTVの情報
		D3D12_PRIMITIVE_TOPOLOGY_TYPE primitiveTopologyType; //利用するトポロジ(形状)のタイプ。
		uint32_t sampleDescCount; //どのように画面に色を打ち込むのかの設定
		uint32_t sampleMask; //どのように画面に色を打ち込むのかの設定
		D3D12_DEPTH_STENCIL_DESC depthStencilState{}; //DepthStencilの設定
		DXGI_FORMAT DSVFormat; //DepthStencilの設定
	};

private: // パイプラインステートオブジェクト作成

	/// <summary>
	/// RootSignature設定
	/// </summary>
	/// <param name="pipelineStateName">パイプラインステートの名前</param>
	/// <param name="rootParameterIndex">ルートパラメータの名前</param>
	/// <param name="samplerIndex">サンプラーの名前</param>
	static void RootsignatureSetting(PipelineStateName pipelineStateName, D3D12_ROOT_SIGNATURE_FLAGS rootsignatureFlags,
		RootParameterIndex rootParameterIndex, SamplerIndex samplerIndex);

	/// <summary>
	/// DepthStencilStateの設定
	/// </summary>
	/// <param name="depthEnable">Depthの機能を有効化</param>
	/// <param name="depthWriteMask">書き込み</param>
	/// <param name="depthFunc">比較関数</param>
	/// <returns></returns>
	static D3D12_DEPTH_STENCIL_DESC DepthStencilStateSetting(bool depthEnable, D3D12_DEPTH_WRITE_MASK depthWriteMask, D3D12_COMPARISON_FUNC depthFunc);

	/// <summary>
	/// InputLayoutの設定
	/// </summary>
	/// <param name="inputLayoutIndex">インプットレイアウトの名前</param>
	/// <returns></returns>
	static D3D12_INPUT_LAYOUT_DESC InputLayoutSetting(InputLayoutIndex inputLayoutIndex);

	/// <summary>
	/// BlendStateの設定
	/// </summary>
	/// <param name="blendStateIndex">ブレンドステートの名前</param>
	/// <returns></returns>
	static D3D12_BLEND_DESC BlendStateSetting(BlendStateIndex blendStateIndex);

	/// <summary>
	/// ResiterzerStateの設定
	/// </summary>
	/// <param name="cullMode">カリング設定</param>
	/// <param name="fillMode">塗りつぶしか</param>
	/// <returns></returns>
	static D3D12_RASTERIZER_DESC ResiterzerStateSetting(D3D12_CULL_MODE cullMode, D3D12_FILL_MODE fillMode);

	/// <summary>
	/// PSOを生成 
	/// </summary>
	/// <param name="createPSODesc_">作るための引数</param>
	static void CreatePSO(const CreatePSODesc& createPSODesc_);

private: // シングルトン

	GraphicsPipelineState() = default;
	~GraphicsPipelineState() = default;
	GraphicsPipelineState(const GraphicsPipelineState&) = delete;
	GraphicsPipelineState& operator=(const GraphicsPipelineState&) = delete;

};
