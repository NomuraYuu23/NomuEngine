#pragma once
#include "../base/WinApp.h"
#include "../base/DirectXCommon.h"
#include <array>
#include <string>
#include "../base/TextureUAV.h"
#include "Velocity2DManager.h"
#include "ShockWaveManager.h"

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

		float distortion; // 歪み
		float vignetteSize; // ビネットの大きさ
		float vignetteChange; // ビネットの変化

		float horzGlitchPase; // グリッチの水平
		float vertGlitchPase; // グリッチの垂直
		float glitchStepValue; // グリッチのステップ値

		int32_t radialBlurSamples; // 放射状ブラーのサンプル回数
		Vector2 radialBlurCenter; // 放射状ブラーの中心座標
		float radialBlurStrength; // 放射状ブラーの広がる強さ
		float radialBlurMask; // 放射状ブラーが適用されないサイズ

		float colorLerpT; // 色変える系のLerpT
		Vector2 colorSize; // 色変える系の大きさ
		Vector2 colorPosition; // 色変える系の位置

		float padding2[2]; // パディング

		Vector4 flareColor; // フレアの色
		Vector2 flareSize; // フレアの大きさ
		Vector2 flarePosition; // フレアの位置

		Vector4 paraColor; // パラの色
		Vector2 paraSize; // パラの大きさ
		Vector2 paraPosition; // パラの位置

	};

	/// <summary>
	/// パイプライン名前
	/// </summary>
	enum PipelineIndex {
		kPipelineIndexBinaryThresholdCS,// 二値化(白黒)
		kPipelineIndexGaussianBlurHorizontal, // ガウスブラー水平
		kPipelineIndexGaussianBlurVertical, // ガウスブラー垂直
		kPipelineIndexBloomHorizontal, // ブルーム水平
		kPipelineIndexBloomVertical, // ブルーム垂直
		kPipelineIndexMotionBlur, // モーションブラー
		kPipelineIndexWhiteNoise, // ホワイトノイズ
		kPipelineIndexScanLine, // 走査線
		kPipelineIndexRGBShift, // RGBずらし
		kPipelineIndexBarrelCurved, // 樽状湾曲
		kPipelineIndexVignette, // ビネット
		kPipelineIndexGlitch, // グリッチ
		kPipelineIndexRadialBlur, // 放射状ブラー
		kPipelineIndexShockWave, // 衝撃波
		kPipelineIndexFlarePara, // フレア パラ
		kPipelineIndexGrayScale, // グレイスケール
		kPipelineIndexSepia, // セピア

		kPipelineIndexGlitchRGBShift, // グリッチRGB

		kPipelineIndexOfCount // 数を数える用（穴埋め用）
	};

	/// <summary>
	/// コマンド番号
	/// </summary>
	enum CommandIndex {
		kCommandIndexBinaryThreshold, // 白黒
		kCommandIndexGaussianBlur, // ガウスブラー
		kCommandIndexBloom, // ブルーム
		kCommandIndexMotionBlur, // モーションブラー
		kCommandIndexWhiteNoize, // ホワイトノイズ
		kCommandIndexScanLine, // 走査線
		kCommandIndexRGBShift, // RGBずらし
		kCommandIndexBarrelCurved, // 樽状湾曲
		kCommandIndexVignette, // ビネット
		kCommandIndexGlitch, // グリッチ
		kCommandIndexRadialBlur, // 放射状ブラー
		kCommandIndexShockWave, // 衝撃波
		kCommandIndexFlarePara, // フレアパラ
		kCommandIndexGrayScale, // グレイスケール
		kCommandIndexSepia, // セピア
		kCommandIndexGlitchRGBShift, // グリッチとRGB
		kCommandIndexOfCount // 数を数えるよう
	};

	/// <summary>
	/// 実行追加引数
	/// </summary>
	struct ExecutionAdditionalDesc {
		std::array<Velocity2DManager*, 4> velocity2DManagers = { nullptr, nullptr, nullptr, nullptr };
		std::array<ShockWaveManager*, 4> shockWaveManagers = { nullptr, nullptr, nullptr, nullptr };
	};

private: // 定数

	// シェーダー情報 <シェーダ名, エントリポイント>
	const std::array<std::pair<const std::wstring, const wchar_t*>, kPipelineIndexOfCount> shaderNames_ =
	{
		std::pair{L"Resources/shaders/PostEffect.CS.hlsl", L"mainBinaryThreshold"}, // 二値化
		std::pair{L"Resources/shaders/PostEffect.CS.hlsl", L"mainGaussianBlurHorizontal"}, // ガウスブラー水平
		std::pair{L"Resources/shaders/PostEffect.CS.hlsl", L"mainGaussianBlurVertical"}, // ガウスブラー垂直
		std::pair{L"Resources/shaders/PostEffect.CS.hlsl", L"mainBloomHorizontal"},  // ブルーム水平
		std::pair{L"Resources/shaders/PostEffect.CS.hlsl", L"mainBloomVertical"},  // ブルーム垂直
		std::pair{L"Resources/shaders/PostEffect.CS.hlsl", L"mainMotionBlur"}, // モーションブラー
		std::pair{L"Resources/shaders/PostEffect.CS.hlsl", L"mainWhiteNoise"}, // ホワイトノイズ
		std::pair{L"Resources/shaders/PostEffect.CS.hlsl", L"mainScanLine"}, // 走査線
		std::pair{L"Resources/shaders/PostEffect.CS.hlsl", L"mainRGBShift"}, // RGBずらし
		std::pair{L"Resources/shaders/PostEffect.CS.hlsl", L"mainBarrelCurved"}, // 樽状湾曲
		std::pair{L"Resources/shaders/PostEffect.CS.hlsl", L"mainVignette"}, // ビネット
		std::pair{L"Resources/shaders/PostEffect.CS.hlsl", L"mainGlitch"}, // グリッチ
		std::pair{L"Resources/shaders/PostEffect.CS.hlsl", L"mainRadialBlur"}, // 放射状ブラー
		std::pair{L"Resources/shaders/PostEffect.CS.hlsl", L"mainShockWave"}, // 衝撃波
		std::pair{L"Resources/shaders/PostEffect.CS.hlsl", L"mainFlarePara"}, // フレア パラ
		std::pair{L"Resources/shaders/PostEffect.CS.hlsl", L"mainGrayScale"}, // グレイスケール
		std::pair{L"Resources/shaders/PostEffect.CS.hlsl", L"mainSepia"}, // セピア
		std::pair{L"Resources/shaders/PostEffect.CS.hlsl", L"mainGlitchRGBShift"}, // グリッチRGB
	};

	// コマンド情報(コマンド実行可能回数4回)
	const std::array<std::array<PipelineIndex, 4>, kCommandIndexOfCount> CommandDatas_ = 
	{
		{
			{kPipelineIndexBinaryThresholdCS, kPipelineIndexOfCount, kPipelineIndexOfCount, kPipelineIndexOfCount}, // 白黒
			{kPipelineIndexGaussianBlurHorizontal, kPipelineIndexGaussianBlurVertical, kPipelineIndexOfCount, kPipelineIndexOfCount}, // ガウスブラー
			{kPipelineIndexBloomHorizontal, kPipelineIndexBloomVertical, kPipelineIndexOfCount, kPipelineIndexOfCount}, // ブルーム
			{kPipelineIndexMotionBlur, kPipelineIndexOfCount, kPipelineIndexOfCount, kPipelineIndexOfCount}, // モーションブラー
			{kPipelineIndexWhiteNoise, kPipelineIndexOfCount, kPipelineIndexOfCount, kPipelineIndexOfCount}, // ホワイトノイズ
			{kPipelineIndexScanLine, kPipelineIndexOfCount, kPipelineIndexOfCount, kPipelineIndexOfCount}, // 走査線
			{kPipelineIndexRGBShift, kPipelineIndexOfCount, kPipelineIndexOfCount, kPipelineIndexOfCount}, // RGBずらし
			{kPipelineIndexBarrelCurved, kPipelineIndexOfCount, kPipelineIndexOfCount, kPipelineIndexOfCount}, // 樽状湾曲
			{kPipelineIndexVignette, kPipelineIndexOfCount, kPipelineIndexOfCount, kPipelineIndexOfCount}, // ビネット
			{kPipelineIndexGlitch, kPipelineIndexOfCount, kPipelineIndexOfCount, kPipelineIndexOfCount}, // グリッチ
			{kPipelineIndexRadialBlur, kPipelineIndexOfCount, kPipelineIndexOfCount, kPipelineIndexOfCount}, // 放射状ブラー
			{kPipelineIndexShockWave, kPipelineIndexOfCount, kPipelineIndexOfCount, kPipelineIndexOfCount}, // 衝撃波
			{kPipelineIndexFlarePara, kPipelineIndexOfCount, kPipelineIndexOfCount, kPipelineIndexOfCount}, // フレアパラ
			{kPipelineIndexGrayScale, kPipelineIndexOfCount, kPipelineIndexOfCount, kPipelineIndexOfCount}, // グレイスケール
			{kPipelineIndexSepia, kPipelineIndexOfCount, kPipelineIndexOfCount, kPipelineIndexOfCount}, // セピア
			{kPipelineIndexGlitchRGBShift, kPipelineIndexOfCount, kPipelineIndexOfCount, kPipelineIndexOfCount}, // グリッチとRGB
		},
	};

	// 画像の幅
	static const uint32_t kTextureWidth = WinApp::kWindowWidth;
	// 画像の高さ
	static const uint32_t kTextureHeight = WinApp::kWindowHeight;
	// 編集する画像の数
	static const uint32_t kNumEditTexture = 2;
	// スレッド数X
	static const uint32_t kNumThreadX = 32;
	// スレッド数Y
	static const uint32_t kNumThreadY = 32;
	// スレッド数Z
	static const uint32_t kNumThreadZ = 1;

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
	/// imGui描画
	/// </summary>
	void ImGuiDraw();

	/// <summary>
	/// 編集する画像取得
	/// </summary>
	/// <param name="index">番号</param>
	/// <returns></returns>
	TextureUAV* GetEditTextures(uint32_t index) { return editTextures_[index].get(); }

	/// <summary>
	/// 実行
	/// </summary>
	/// <param name="commandList"></param>
	/// <param name="renderTargetTexture"></param>
	/// <param name="commandIndex"></param>
	/// <param name="executionAdditionalDesc"></param>
	void Execution(
		ID3D12GraphicsCommandList* commandList,
		RenderTargetTexture* renderTargetTexture,
		CommandIndex commandIndex,
		ExecutionAdditionalDesc* executionAdditionalDesc = nullptr);

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

	/// <summary>
	/// 歪み設定
	/// </summary>
	/// <param name="distortion">歪み</param>
	void SetDistortion(float distortion) { computeParametersMap_->distortion = distortion; }

	/// <summary>
	/// ビネットの大きさ設定
	/// </summary>
	/// <param name="vignetteSize">ビネットの大きさ</param>
	void SetVignetteSize(float vignetteSize) { computeParametersMap_->vignetteSize = vignetteSize; }

	/// <summary>
	/// ビネットの変化設定
	/// </summary>
	/// <param name="vignetteChange">ビネットの変化</param>
	void SetVignetteChange(float vignetteChange) { computeParametersMap_->vignetteChange = vignetteChange; }

	/// <summary>
	/// グリッチの水平設定
	/// </summary>
	/// <param name="horzGlitchPase">グリッチの水平</param>
	void SetHorzGlitchPase(float horzGlitchPase) { computeParametersMap_->horzGlitchPase = horzGlitchPase; }

	/// <summary>
	/// グリッチの垂直設定
	/// </summary>
	/// <param name="vertGlitchPase">グリッチの垂直</param>
	void SetVertGlitchPase(float vertGlitchPase) { computeParametersMap_->vertGlitchPase = vertGlitchPase; }

	/// <summary>
	/// グリッチのステップ値設定
	/// </summary>
	/// <param name="glitchStepValue">グリッチのステップ値</param>
	void SetGlitchStepValue(float glitchStepValue) { computeParametersMap_->glitchStepValue = glitchStepValue; }

	/// <summary>
	/// 放射状ブラーのサンプル回数設定
	/// </summary>
	/// <param name="radialBlurSamples">放射状ブラーのサンプル回数</param>
	void SetRadialBlurSamples(int32_t radialBlurSamples) { computeParametersMap_->radialBlurSamples = radialBlurSamples; }

	/// <summary>
	/// 中心座標設定
	/// </summary>
	/// <param name="radialBlurCenter">中心座標</param>
	void SetRadialBlurCenter(const Vector2& radialBlurCenter) { computeParametersMap_->radialBlurCenter = radialBlurCenter; }

	/// <summary>
	/// ブラーの広がる強さ設定
	/// </summary>
	/// <param name="radialBlurStrength">ブラーの広がる強さ</param>
	void SetRadialBlurStrength(float radialBlurStrength) { computeParametersMap_->radialBlurStrength = radialBlurStrength; }

	/// <summary>
	/// 放射状ブラーが適用されないサイズ設定
	/// </summary>
	/// <param name="radialBlurMask">放射状ブラーが適用されないサイズ</param>
	void SetRadialBlurMask(float radialBlurMask) { computeParametersMap_->radialBlurMask = radialBlurMask; }
	
	/// <summary>
	/// 色変える系のLerpT設定
	/// </summary>
	/// <param name="colorLerpT">色変える系のLerpT</param>
	void SetColorLerpT(float colorLerpT) { computeParametersMap_->colorLerpT = colorLerpT; }

	/// <summary>
	/// 色変える系の大きさ設定
	/// </summary>
	/// <param name="colorSize">色変える系の大きさ</param>
	void SetColorSize(const Vector2& colorSize) { computeParametersMap_->colorSize = colorSize; }

	/// <summary>
	/// 色変える系の位置設定
	/// </summary>
	/// <param name="colorPosition">色変える系の位置</param>
	void SetColorPosition(const Vector2& colorPosition) { computeParametersMap_->colorPosition = colorPosition; }

	/// <summary>
	/// フレアの色設定
	/// </summary>
	/// <param name="flareColor">フレアの色</param>
	void SetFlareColor(const Vector4& flareColor) { computeParametersMap_->flareColor = flareColor; }
	
	/// <summary>
	/// フレアの大きさ設定
	/// </summary>
	/// <param name="flareSize">フレアの大きさ</param>
	void SetFlareSize(const Vector2& flareSize) { computeParametersMap_->flareSize = flareSize; }
	
	/// <summary>
	/// フレアの位置設定
	/// </summary>
	/// <param name="flarePosition">フレアの位置</param>
	void SetFlarePosition(const Vector2& flarePosition) { computeParametersMap_->flarePosition = flarePosition; }

	/// <summary>
	/// パラの色設定
	/// </summary>
	/// <param name="paraColor">パラの色</param>
	void SetParaColor(const Vector4& paraColor) { computeParametersMap_->paraColor = paraColor; }
	
	/// <summary>
	/// パラの大きさ設定
	/// </summary>
	/// <param name="paraSize">パラの大きさ</param>
	void SetParaSize(const Vector2& paraSize) { computeParametersMap_->paraSize = paraSize; }
	
	/// <summary>
	/// パラの位置設定
	/// </summary>
	/// <param name="paraPosition">パラの位置</param>
	void SetParaPosition(const Vector2& paraPosition) { computeParametersMap_->paraPosition = paraPosition; }

private: // 変数

	// デバイス
	ID3D12Device* device_ = nullptr;

	// コマンドリスト
	ID3D12GraphicsCommandList* commandList_ = nullptr;

	// 編集する画像
	std::unique_ptr<TextureUAV> editTextures_[kNumEditTexture];

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

	// デフォルト速度バッファ
	std::unique_ptr<Velocity2DManager> velocity2DManager_;

	// デフォルト衝撃波パラメータ
	std::unique_ptr<ShockWaveManager> shockWaveManager_;

	// ディスクリプタレンジ保存
	std::vector<std::vector<D3D12_DESCRIPTOR_RANGE>> descriptorRanges_;


private: // シングルトン
	PostEffect() = default;
	~PostEffect() = default;
	PostEffect(const PostEffect&) = delete;
	const PostEffect& operator=(const PostEffect&) = delete;

};

