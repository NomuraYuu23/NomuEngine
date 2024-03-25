#pragma once
#include "../base/RenderTargetTexture.h"
#include "../base/DirectXCommon.h"
#include <array>
#include <string>
#include "../base/TextureUAV.h"

class Glare
{

public: // サブクラス

	/// <summary>
	/// CSに渡す定数バッファ
	/// </summary>
	struct ComputeParameters
	{

		float lamdaR/* = static_cast<float>(633e-9)*/; // ラムダR
		float lamdaG/* = static_cast<float>(532e-9)*/; // ラムダG
		float lamdaB/* = static_cast<float>(466e-9)*/; // ラムダB
		float glareIntensity/* = 0.8f*/; // グレア強度
		float threshold/* = 0.8f*/; // しきい値

	};

	// グレア用の画像名前
	enum ImageForGlareIndex {
		kImageForGlareIndexHalo, // ハロ
		kImageForGlareIndexOfCount // 数を数える用
	};

	// パイプライン名前
	enum PiolineIndex {
		kPiolineIndexMulCS, // 乗算
		kPiolineIndexFFTROWCS, // FFT縦
		kPiolineIndexFFTCOLCS, // FFT横
		kPiolineIndexIFFTROWCS, // 逆FFT縦
		kPiolineIndexIFFTCOLCS, // 逆FFT横
		kPiolineIndexAmpCS, // 増幅
		kPiolineIndexDivByMaxAampCS, // 最大振幅による除算
		kPiolineIndexAddCS, // 加算
		kPiolineIndexBinaryThresholdCS,// 二値化
		kPiolineIndexCopyCS, // コピー
		kPiolineIndexClesrCS, // クリア
		kPiolineIndexSpectrumScalingCS, // スペクトルスケール
		kPiolineIndexRaiseRealImageCS, //グレアの輝度を底上げ
		kPiolineIndexMaxMinFirstCS, // 最大値最小値計算1回目
		kPiolineIndexMaxMinSecondCS, // 最大値最小値計算2回目
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
	/// <param name="commandList">コマンドリスト</param>
	void Execution(
		const CD3DX12_GPU_DESCRIPTOR_HANDLE& imageWithGlareHandle,
		float glareIntensity,
		float threshold,
		ImageForGlareIndex imageForGlareIndex,
		ID3D12GraphicsCommandList* commandList);

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
	/// シェーダーをコンパイル
	/// </summary>
	void CompileShader();

	/// <summary>
	/// パイプライン作成
	/// </summary>
	void CreatePipline();

	/// <summary>
	/// ヘッダを動的に作る
	/// </summary>
	/// <param name="width">幅</param>
	/// <param name="height">高さ</param>
	/// <param name="row">縦か</param>
	/// <param name="inv">逆か</param>
	void CreateHeaderHLSL(
		uint32_t width, uint32_t height,
		bool row, bool inv);
	
	/// <summary>
	/// 
	/// </summary>
	/// <param name="n"></param>
	/// <returns></returns>
	int Inv2Pow(int n);
	
private: // コマンド

	/// <summary>
	/// コピーコマンド
	/// </summary>
	/// <param name="in">コピーする画像</param>
	/// <param name="out">ペーストされる画像</param>
	void CopyCommand(const CD3DX12_GPU_DESCRIPTOR_HANDLE& in, TextureUAV* out);
	void CopyCommand(uint32_t in, TextureUAV* out);

	/// <summary>
	/// 二値化コマンド
	/// </summary>
	/// <param name="in">二値化する画像</param>
	/// <param name="out">二値化された画像</param>
	void BinaryThresholdCommand(TextureUAV* in, TextureUAV* out);

	/// <summary>
	/// クリア
	/// </summary>
	/// <param name="tex">クリアする画像</param>
	void ClearCommand(TextureUAV* tex);

	/// <summary>
	/// FFTコマンド
	/// </summary>
	/// <param name="real">実部</param>
	/// <param name="image">虚部</param>
	void FFTCommand(TextureUAV* real, TextureUAV* image);

	/// <summary>
	/// IFFTコマンド
	/// </summary>
	/// <param name="real">実部</param>
	/// <param name="image">虚部</param>
	void IFFTCommand(TextureUAV* real, TextureUAV* image);

	/// <summary>
	/// 増幅コマンド
	/// </summary>
	/// <param name="inReal">入力実部</param>
	/// <param name="inImage">入力虚部</param>
	/// <param name="outReal">出力実部</param>
	/// <param name="outImage">出力虚部</param>
	void AmpCommand(TextureUAV* inReal, TextureUAV* inImage,
		TextureUAV* outReal, TextureUAV* outImage);

	/// <summary>
	/// 最大値最小値の計算
	/// </summary>
	/// <param name="tex"></param>
	/// <param name="outOnePixRealMax"></param>
	/// <param name="outOnePixImageMin"></param>
	void CalcMaxMinCommand(TextureUAV* tex, 
		TextureUAV* outOnePixRealMax, TextureUAV* outOnePixImageMin);

	/// <summary>
	/// 最大振幅による除算
	/// </summary>
	/// <param name="outOnePixRealMax"></param>
	/// <param name="outOnePixImageMin"></param>
	/// <param name="inReal"></param>
	/// <param name="inImage"></param>
	/// <param name="outReal"></param>
	/// <param name="outImage"></param>
	void DivideMaxAmpCommand(
		TextureUAV* outOnePixRealMax, TextureUAV* outOnePixImageMin,
		TextureUAV* inReal, TextureUAV* inImage,
		TextureUAV* outReal, TextureUAV* outImage);

	/// <summary>
	/// グレアの輝度を底上げ
	/// </summary>
	/// <param name="inReal">入力実部</param>
	/// <param name="inImage">入力虚部</param>
	/// <param name="outReal">出力実部</param>
	/// <param name="outImage">出力虚部</param>
	void RaiseRICommand(TextureUAV* inReal, TextureUAV* inImage,
		TextureUAV* outReal, TextureUAV* outImage);

	/// <summary>
	/// スペクトルスケール
	/// </summary>
	/// <param name="inReal">入力実部</param>
	/// <param name="inImage">入力虚部</param>
	/// <param name="outReal">出力実部</param>
	/// <param name="outImage">出力虚部</param>
	void SpectrumScalingCommand(TextureUAV* inReal, TextureUAV* inImage,
		TextureUAV* outReal, TextureUAV* outImage);

	/// <summary>
	/// 畳み込み
	/// </summary>
	/// <param name="inReal0">入力実部</param>
	/// <param name="inImage0">入力虚部</param>
	/// <param name="inReal1">入力実部</param>
	/// <param name="inImage1">入力虚部</param>
	/// <param name="outReal">出力実部</param>
	/// <param name="outImage">出力虚部</param>
	void ConvolutionCommand(
		TextureUAV* inReal0, TextureUAV* inImage0,
		TextureUAV* inReal1, TextureUAV* inImage1,
		TextureUAV* outReal, TextureUAV* outImage);

	/// <summary>
	/// 乗算
	/// </summary>
	/// <param name="inReal0">入力実部</param>
	/// <param name="inImage0">入力虚部</param>
	/// <param name="inReal1">入力実部</param>
	/// <param name="inImage1">入力虚部</param>
	/// <param name="outReal">出力実部</param>
	/// <param name="outImage">出力虚部</param>
	void MultiplyCommand(
		TextureUAV* inReal0, TextureUAV* inImage0,
		TextureUAV* inReal1, TextureUAV* inImage1,
		TextureUAV* outReal, TextureUAV* outImage);

	/// <summary>
	/// 加算
	/// </summary>
	/// <param name="tex0">テクスチャ0</param>
	/// <param name="tex1">テクスチャ1</param>
	/// <param name="out">出力</param>
	void AddCommand(
		TextureUAV* tex0, TextureUAV* tex1,
		TextureUAV* out);


private: // 変数

	// デバイス
	ID3D12Device* device_;

	// コマンドリスト
	ID3D12GraphicsCommandList* commandList_;

	// 編集する画像
	std::unique_ptr<TextureUAV> editTextures_[8];
	// グレアを掛ける画像(RTTexのハンドル)
	CD3DX12_GPU_DESCRIPTOR_HANDLE imageWithGlareHandle_;
	// グレア用の画像ハンドル
	std::array<uint32_t, kImageForGlareIndexOfCount> imageForGlareHandles_;
	
	// 最大値最小値計算で使うテクスチャ
	std::unique_ptr<TextureUAV> maxMinTextures_[2];
	std::unique_ptr<TextureUAV> lineInnerTexture_;

	// FFTで使うテクスチャ
	std::unique_ptr<TextureUAV> fftInnerTextures_[2];

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
		std::pair{L"Resources/shaders/Glare.CS.hlsl", L"mainCopy"},
		std::pair{L"Resources/shaders/Glare.CS.hlsl", L"mainCopy"},
		std::pair{L"Resources/shaders/Glare.CS.hlsl", L"mainCopy"},
		std::pair{L"Resources/shaders/Glare.CS.hlsl", L"mainCopy"},
		std::pair{L"Resources/shaders/Glare.CS.hlsl", L"mainCopy"},
		std::pair{L"Resources/shaders/Glare.CS.hlsl", L"mainCopy"},
		std::pair{L"Resources/shaders/Glare.CS.hlsl", L"mainCopy"},
		std::pair{L"Resources/shaders/Glare.CS.hlsl", L"mainCopy"},
		std::pair{L"Resources/shaders/Glare.CS.hlsl", L"mainBinaryThreshold"}, // 二値化
		std::pair{L"Resources/shaders/Glare.CS.hlsl", L"mainCopy"}, // コピー
		std::pair{L"Resources/shaders/Glare.CS.hlsl", L"mainClear"}, // クリア
		std::pair{L"Resources/shaders/Glare.CS.hlsl", L"mainCopy"},
		std::pair{L"Resources/shaders/Glare.CS.hlsl", L"mainCopy"},
		std::pair{L"Resources/shaders/Glare.CS.hlsl", L"mainCopy"},
		std::pair{L"Resources/shaders/Glare.CS.hlsl", L"mainCopy"},

	};

	const uint32_t kWidth = 854;
	const uint32_t kHeight = 480;

};

