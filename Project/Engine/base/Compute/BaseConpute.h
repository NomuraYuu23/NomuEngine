#pragma once
#include <d3d12.h>
#include <wrl.h>
#include <vector>

#include "../DirectXCommon.h"

class BaseConpute
{

public:

	/// <summary>
	/// デバイス取得
	/// </summary>
	/// <param name="device">デバイス</param>
	static void GatDevice(ID3D12Device* device) { device_ = device; }

public: // 関数

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="filePath">シェーダーファイルのパス</param>
	/// <param name="entryPoint">エントリポイント</param>
	virtual void Initialize(const std::wstring& filePath, const wchar_t* entryPoint = L"main");

	/// <summary>
	/// 実行
	/// </summary>
	/// <param name="dxCommon">DirectX/param>
	virtual void Execution(DirectXCommon* dxCommon) = 0;

protected: // 関数

	/// <summary>
	/// ルートシグネチャを作る
	/// </summary>
	virtual void CreateRootSignature() = 0;

	/// <summary>
	/// パイプラインを作る
	/// </summary>
	virtual void CreatePipelineState();

	/// <summary>
	/// 実行前処理
	/// </summary>
	void PreExecution(DirectXCommon* dxCommon);

	/// <summary>
	/// 実行後処理
	/// </summary>
	void PostExecution(DirectXCommon* dxCommon);

protected: // 変数

	// デバイス
	static ID3D12Device* device_;

	// ルートシグネチャ
	Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature_;

	// シェーダー情報
	Microsoft::WRL::ComPtr<IDxcBlob> shader_;

	//パイプライン
	Microsoft::WRL::ComPtr<ID3D12PipelineState> pipelineState_;

};

