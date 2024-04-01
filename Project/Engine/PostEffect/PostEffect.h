#pragma once
#include "../base/DirectXCommon.h"
#include <array>
#include <string>
#include "../base/TextureUAV.h"

class PostEffect
{

public: // サブクラス

	/// <summary>
	/// CSに渡す定数バッファ
	/// </summary>
	struct ComputeParameters
	{
		float threshold/* = 0.8f*/; // しきい値
	};

	// パイプライン名前
	enum PiolineIndex {
		kPiolineIndexBinaryThresholdCS,// 二値化(白黒)
		kPiolineIndexCopyCS, // コピー
		kPiolineIndexClesrCS, // クリア
		kPiolineIndexOfCount // 数を数える用
	};

private: // 定数

	// シェーダー情報 <シェーダ名, エントリポイント>
	const std::array<std::pair<const std::wstring, const wchar_t*>, kPiolineIndexOfCount> shaderNames_ =
	{
		std::pair{L"Resources/shaders/Glare.CS.hlsl", L"mainBinaryThreshold"}, // 二値化
		std::pair{L"Resources/shaders/Glare.CS.hlsl", L"mainCopy"}, // コピー
		std::pair{L"Resources/shaders/Glare.CS.hlsl", L"mainClear"}, // クリア
	};
	
	// 画像の幅
	const uint32_t kTextureWidth = 128;
	// 画像の高さ
	const uint32_t kTextureHeight = 128;
	// 編集する画像の数
	const uint32_t kNumEditTexture = 8;

public: // 関数

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 編集する画像取得
	/// </summary>
	/// <param name="index"></param>
	/// <returns></returns>
	TextureUAV* GetEditTextures(uint32_t index) { return editTextures_[index].get(); }

private:  // 関数

	/// <summary>
	///	ルートシグネチャ作成
	/// </summary>
	void CreateRootSignature();

	/// <summary>
	/// ヘッダを動的に作る
	/// </summary>
	void CreateHeaderHLSL();

	/// <summary>
	/// シェーダーをコンパイル
	/// </summary>
	void CompileShader();

	/// <summary>
	/// パイプライン作成
	/// </summary>
	void CreatePipline();

private: // 変数

	// デバイス
	ID3D12Device* device_;

	// コマンドリスト
	ID3D12GraphicsCommandList* commandList_;

	// 編集する画像
	std::unique_ptr<TextureUAV> editTextures_[8];

	//computeParameters用のリソースを作る。
	Microsoft::WRL::ComPtr<ID3D12Resource> computeParametersBuff_;
	// computeParametersのマップ
	ComputeParameters* computeParametersMap_ = nullptr;

	//パイプライン
	std::array<Microsoft::WRL::ComPtr<ID3D12PipelineState>, kPiolineIndexOfCount > pipelineStates_;
	// ルートシグネチャ
	Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature_;
	// シェーダー情報
	std::array<Microsoft::WRL::ComPtr<IDxcBlob>, kPiolineIndexOfCount> shaders_;

};

