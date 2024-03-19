#pragma once
#include "../base/RenderTargetTexture.h"
#include "../base/DirectXCommon.h"
#include <array>
#include <string>

class Glare
{

public: // サブクラス

	/// <summary>
	/// CSに渡す定数バッファ
	/// </summary>
	struct ComputeParameters
	{

		float lamdaR = static_cast<float>(633e-9); // ラムダR
		float lamdaG = static_cast<float>(532e-9); // ラムダG
		float lamdaB = static_cast<float>(466e-9); // ラムダB
		float glareIntensity = 0.8f; // グレア強度
		float threshold = 0.8f; // しきい値

	};

	// グレア用の画像名前
	enum ImageForGlareIndex {
		kImageForGlareIndexHalo, // ハロ
		kImageForGlareIndexOfCount // 数を数える用
	};

	// パイプライン名前
	enum PiolineIndex {
		kPiolineIndexOfCount // 数を数える用
	};

public: // 関数

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="imageForGlareHandles">グレア用の画像ハンドル</param>
	void Initialize(
		const std::array<uint32_t, kImageForGlareIndexOfCount>& imageForGlareHandles);

	/// <summary>
	/// 実行
	/// </summary>
	/// <param name="imageWithGlareHandle">グレアを掛ける画像(RTTexのハンドル)</param>
	/// <param name="glareIntensity">グレア強度</param>
	/// <param name="threshold">しきい値</param>
	/// <param name="imageForGlareIndex">グレアの種類</param>
	void Execution(
		const CD3DX12_GPU_DESCRIPTOR_HANDLE& imageWithGlareHandle,
		float glareIntensity,
		float threshold,
		ImageForGlareIndex imageForGlareIndex);

private:  // 関数

	/// <summary>
	///	ルートシグネチャ作成
	/// </summary>
	void CreateRootSignature();

	/// <summary>
	/// シェーダーをコンパイル
	/// </summary>
	void CompileShader();

	/// <summary>
	/// パイプライン作成
	/// </summary>
	void CreatePipline();

private: // コマンド

	void CopyCommand();

	void BinaryThreshold();

private: // 変数

	// デバイス
	ID3D12Device* device_;

	// 編集する画像
	std::unique_ptr<RenderTargetTexture> renderTargetTexture_;
	// グレアを掛ける画像(RTTexのハンドル)
	CD3DX12_GPU_DESCRIPTOR_HANDLE imageWithGlareHandle_;
	// グレア用の画像ハンドル
	std::array<uint32_t, kImageForGlareIndexOfCount> imageForGlareHandles_;

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

private: // 定数

	// シェーダー情報 <シェーダ名, エントリポイント>
	const std::array<std::pair<const std::wstring, const wchar_t*>, kPiolineIndexOfCount> shaderNames_ = 
	{

	};

};

