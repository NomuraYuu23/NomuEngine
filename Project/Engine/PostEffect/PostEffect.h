#pragma once
#include "../base/WinApp.h"
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
		uint32_t threadIdOffsetX; // スレッドのオフセットX
		uint32_t threadIdTotalX; // スレッドの総数X
		uint32_t threadIdOffsetY; // スレッドのオフセットY
		uint32_t threadIdTotalY; // スレッドの総数Y
		uint32_t threadIdOffsetZ; // スレッドのオフセットZ
		uint32_t threadIdTotalZ; // スレッドの総数Z
		float padding[2]; // パディング

		Vector4 clearColor; // クリアするときの色
		float threshold; // 明度のしきい値
		int32_t kernelSize; // カーネルサイズ
		float sigma; // 標準偏差
		float time; // 時間

		Vector2 rShift; // Rずらし
		Vector2 gShift; // Gずらし
		Vector2 bShift; // Bずらし

	};

	/// <summary>
	/// パイプライン名前
	/// </summary>
	enum PipelineIndex {
		kPipelineIndexCopyCS, // コピー
		kPipelineIndexClesrCS, // クリア
		kPipelineIndexBinaryThresholdCS,// 二値化(白黒)
		kPipelineIndexGaussianBlurHorizontal, // ガウスブラー水平
		kPipelineIndexGaussianBlurVertical, // ガウスブラー垂直
		kPipelineIndexBrightnessThreshold, // 明度分け
		kPipelineIndexBlurAdd, // ブラー用加算
		kPipelineIndexOverwrite, // 上書き
		kPipelineIndexRTTCorrection, // レンダーターゲット画像の修正
		kPipelineIndexMotionBlur, // モーションブラー
		kPipliineIndexWhiteNoise, // ホワイトノイズ
		kPipliineIndexScanLine, // 走査線
		kPipliineIndexRGBShift, // RGBずらし
		kPipelineIndexOfCount // 数を数える用
	};

private: // 定数

	// シェーダー情報 <シェーダ名, エントリポイント>
	const std::array<std::pair<const std::wstring, const wchar_t*>, kPipelineIndexOfCount> shaderNames_ =
	{
		std::pair{L"Resources/shaders/PostEffect.CS.hlsl", L"mainCopy"}, // コピー
		std::pair{L"Resources/shaders/PostEffect.CS.hlsl", L"mainClear"}, // クリア
		std::pair{L"Resources/shaders/PostEffect.CS.hlsl", L"mainBinaryThreshold"}, // 二値化
		std::pair{L"Resources/shaders/PostEffect.CS.hlsl", L"mainGaussianBlurHorizontal"}, // ガウスブラー水平
		std::pair{L"Resources/shaders/PostEffect.CS.hlsl", L"mainGaussianBlurVertical"}, // ガウスブラー垂直
		std::pair{L"Resources/shaders/PostEffect.CS.hlsl", L"mainBrightnessThreshold"}, // 明度分け
		std::pair{L"Resources/shaders/PostEffect.CS.hlsl", L"mainBlurAdd"}, // ブラー用加算
		std::pair{L"Resources/shaders/PostEffect.CS.hlsl", L"mainOverwrite"}, // 上書き
		std::pair{L"Resources/shaders/PostEffect.CS.hlsl", L"mainRTTCorrection"}, // レンダーターゲット画像の修正
		std::pair{L"Resources/shaders/PostEffect.CS.hlsl", L"mainMotionBlur"}, // モーションブラー
		std::pair{L"Resources/shaders/PostEffect.CS.hlsl", L"mainWhiteNoise"}, // ホワイトノイズ
		std::pair{L"Resources/shaders/PostEffect.CS.hlsl", L"mainScanLine"}, // 走査線
		std::pair{L"Resources/shaders/PostEffect.CS.hlsl", L"mainRGBShift"} // RGBずらし
	};
	
	// 画像の幅
	const uint32_t kTextureWidth = WinApp::kWindowWidth;
	// 画像の高さ
	const uint32_t kTextureHeight = WinApp::kWindowHeight;
	// 編集する画像の数
	const uint32_t kNumEditTexture = 8;
	// スレッド数X
	const uint32_t kNumThreadX = 32;
	// スレッド数Y
	const uint32_t kNumThreadY = 32;
	// スレッド数Z
	const uint32_t kNumThreadZ = 1;

public: // 関数

	/// <summary>
	/// インスタンス取得
	/// </summary>
	static PostEffect* GetInstance();

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 編集する画像取得
	/// </summary>
	/// <param name="index">番号</param>
	/// <returns></returns>
	TextureUAV* GetEditTextures(uint32_t index) { return editTextures_[index].get(); }

	/// <summary>
	/// コピー
	/// </summary>
 	/// <param name="commandList">コマンドリスト</param>
	/// <param name="copyGPUHandle">コピーする画像のGPUハンドル</param>
	/// <param name="editTextureIndex">編集する画像番号</param>
	void CopyCommand(
		ID3D12GraphicsCommandList* commandList,
		uint32_t editTextureIndex,
		const CD3DX12_GPU_DESCRIPTOR_HANDLE& copyGPUHandle);

	/// <summary>
	/// クリア
	/// </summary>
	/// <param name="commandList">コマンドリスト</param>
	/// <param name="editTextureIndex">編集する画像番号</param>
	void ClearCommand(
		ID3D12GraphicsCommandList* commandList,
		uint32_t editTextureIndex);

	/// <summary>
	/// 二値化
	/// </summary>
	/// <param name="commandList">コマンドリスト</param>
	/// <param name="editTextureIndex">編集する画像番号</param>
	/// <param name="copyGPUHandle">二値化する画像のGPUハンドル</param>
	void BinaryThresholdCommand(
		ID3D12GraphicsCommandList* commandList,
		uint32_t editTextureIndex, 
		const CD3DX12_GPU_DESCRIPTOR_HANDLE& binaryThresholdGPUHandle);


	/// <summary>
	/// ガウスブラー
	/// </summary>
	/// <param name="commandList">コマンドリスト</param>
	/// <param name="editTextureIndex">編集する画像番号</param>
	/// <param name="gaussianBluGPUHandle">ガウスブラーをかける画像のGPUハンドル</param>
	void GaussianBlurCommand(
		ID3D12GraphicsCommandList* commandList,
		uint32_t editTextureIndex,
		const CD3DX12_GPU_DESCRIPTOR_HANDLE& gaussianBluGPUHandle);

	/// <summary>
	/// ブルーム
	/// </summary>
	/// <param name="commandList">コマンドリスト</param>
	/// <param name="editTextureIndex">編集する画像番号</param>
	/// <param name="bloomGPUHandle">ブルームをかける画像のGPUハンドル</param>
	void BloomCommand(
		ID3D12GraphicsCommandList* commandList,
		uint32_t editTextureIndex,
		const CD3DX12_GPU_DESCRIPTOR_HANDLE& bloomGPUHandle);

	/// <summary>
	/// 画像上書き
	/// </summary>
	/// <param name="commandList">コマンドリスト</param>
	/// <param name="editTextureIndex">編集する画像番号</param>
	/// <param name="addGPUHandle0">合成する画像のGPUハンドル0</param>
	/// <param name="addGPUHandle1">合成する画像のGPUハンドル1</param>
	void OverwriteCommand(
		ID3D12GraphicsCommandList* commandList,
		uint32_t editTextureIndex,
		const CD3DX12_GPU_DESCRIPTOR_HANDLE& addGPUHandle0,
		const CD3DX12_GPU_DESCRIPTOR_HANDLE& addGPUHandle1);

	/// <summary>
	/// レンダーターゲット画像の修正（クリアカラーを{0.0f,0.0f,0.0f,0.0f}に変更する）
	/// </summary>
	/// <param name="commandList">コマンドリスト</param>
	/// <param name="editTextureIndex">編集する画像番号</param>
	/// <param name="textureGPUHandle">画像のGPUハンドル</param>
	void RTTCorrectionCommand(
		ID3D12GraphicsCommandList* commandList,
		uint32_t editTextureIndex,
		const CD3DX12_GPU_DESCRIPTOR_HANDLE& textureGPUHandle);

	/// <summary>
	/// モーションブラー
	/// </summary>
	/// <param name="commandList">コマンドリスト</param>
	/// <param name="editTextureIndex">編集する画像番号</param>
	/// <param name="motionBlurGPUHandle">画像のGPUハンドル</param>
	/// <param name="velocityBuff">速度バッファ</param>
	void MotionBlurCommand(
		ID3D12GraphicsCommandList* commandList,
		uint32_t editTextureIndex,
		const CD3DX12_GPU_DESCRIPTOR_HANDLE& motionBlurGPUHandle,
		ID3D12Resource* velocityBuff);

	/// <summary>
	/// ホワイトノイズ
	/// </summary>
	/// <param name="commandList">コマンドリスト</param>
	/// <param name="editTextureIndex">編集する画像番号</param>
	/// <param name="whiteNoizeGPUHandle">画像のGPUハンドル</param>
	void WhiteNoizeCommand(
		ID3D12GraphicsCommandList* commandList,
		uint32_t editTextureIndex,
		const CD3DX12_GPU_DESCRIPTOR_HANDLE& whiteNoizeGPUHandle);

	/// <summary>
	/// 走査線
	/// </summary>
	/// <param name="commandList">コマンドリスト</param>
	/// <param name="editTextureIndex">編集する画像番号</param>
	/// <param name="whiteNoizeGPUHandle">画像のGPUハンドル</param>
	void ScanLineCommand(
		ID3D12GraphicsCommandList* commandList,
		uint32_t editTextureIndex,
		const CD3DX12_GPU_DESCRIPTOR_HANDLE& scanLineGPUHandle);

	/// <summary>
	/// RGBずらし
	/// </summary>
	/// <param name="commandList">コマンドリスト</param>
	/// <param name="editTextureIndex">編集する画像番号</param>
	/// <param name="rgbShiftGPUHandle">画像のGPUハンドル</param>
	void RGBShift(
		ID3D12GraphicsCommandList* commandList,
		uint32_t editTextureIndex,
		const CD3DX12_GPU_DESCRIPTOR_HANDLE& rgbShiftGPUHandle);


private: // 関数

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

public: // アクセッサ

	/// <summary>
	/// クリアするときの色設定
	/// </summary>
	/// <param name="color">クリアするときの色</param>
	void SetClearColor(const Vector4& clearColor) { computeParametersMap_->clearColor = clearColor; }

	/// <summary>
	/// 明度のしきい値設定
	/// </summary>
	/// <param name="threshold">明度のしきい値</param>
	void SetThreshold(float threshold) { computeParametersMap_->threshold = threshold; }

	/// <summary>
	/// カーネルサイズ設定
	/// </summary>
	/// <param name="kernelSize">カーネルサイズ</param>
	void SetKernelSize(int32_t kernelSize) { computeParametersMap_->kernelSize = kernelSize; }

	/// <summary>
	/// 標準偏差設定
	/// </summary>
	/// <param name="sigma">標準偏差</param>
	void SetSigma(float sigma) { computeParametersMap_->sigma = sigma; }

	/// <summary>
	/// 時間設定
	/// </summary>
	/// <param name="time">時間</param>
	void SetTime(float time) { computeParametersMap_->time = time; }

	/// <summary>
	/// Rずらし設定
	/// </summary>
	/// <param name="rShift">Rずらし</param>
	void SetRShift(const Vector2& rShift) { computeParametersMap_->rShift = rShift; }
	
	/// <summary>
	/// Gずらし設定
	/// </summary>
	/// <param name="gShift">Gずらし</param>
	void SetGShift(const Vector2& gShift) { computeParametersMap_->gShift = gShift; }

	/// <summary>
	/// Bずらし設定
	/// </summary>
	/// <param name="bShift">Bずらし</param>
	void SetBShift(const Vector2& bShift) { computeParametersMap_->bShift = bShift; }

private: // 変数

	// デバイス
	ID3D12Device* device_;

	// コマンドリスト
	ID3D12GraphicsCommandList* commandList_;

	// 編集する画像
	std::unique_ptr<TextureUAV> editTextures_[8];

	// 内部編集画像
	std::unique_ptr<TextureUAV> internalEditTextures_[8];

	//computeParameters用のリソースを作る。
	Microsoft::WRL::ComPtr<ID3D12Resource> computeParametersBuff_;
	// computeParametersのマップ
	ComputeParameters* computeParametersMap_ = nullptr;

	//パイプライン
	std::array<Microsoft::WRL::ComPtr<ID3D12PipelineState>, kPipelineIndexOfCount > pipelineStates_;
	// ルートシグネチャ
	Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature_;
	// シェーダー情報
	std::array<Microsoft::WRL::ComPtr<IDxcBlob>, kPipelineIndexOfCount> shaders_;

private: // シングルトン
	PostEffect() = default;
	~PostEffect() = default;
	PostEffect(const PostEffect&) = delete;
	const PostEffect& operator=(const PostEffect&) = delete;

};

