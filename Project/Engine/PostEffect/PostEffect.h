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
		float padding[2]; // パディング
		uint32_t threadIdTotalZ; // スレッドの総数Z
		Vector4 clearColor; // クリアするときの色
		float threshold; // しきい値
	};

	/// <summary>
	/// パイプライン名前
	/// </summary>
	enum PipelineIndex {
		kPipelineIndexCopyCS, // コピー
		kPipelineIndexClesrCS, // クリア
		kPipelineIndexBinaryThresholdCS,// 二値化(白黒)
		kPipelineIndexOfCount // 数を数える用
	};

private: // 定数

	// シェーダー情報 <シェーダ名, エントリポイント>
	const std::array<std::pair<const std::wstring, const wchar_t*>, kPipelineIndexOfCount> shaderNames_ =
	{
		std::pair{L"Resources/shaders/PostEffect.CS.hlsl", L"mainCopy"}, // コピー
		std::pair{L"Resources/shaders/PostEffect.CS.hlsl", L"mainClear"}, // クリア
		std::pair{L"Resources/shaders/PostEffect.CS.hlsl", L"mainBinaryThreshold"}, // 二値化
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
	/// <param name="index"></param>
	/// <returns></returns>
	TextureUAV* GetEditTextures(uint32_t index) { return editTextures_[index].get(); }

	/// <summary>
	/// コピーコマンド
	/// </summary>
 	/// <param name="commandList">コマンドリスト</param>
	/// <param name="copyGPUHandle">コピーする画像のGPUハンドル</param>
	/// <param name="editTextureIndex">編集する画像番号</param>
	void CopyCommand(
		ID3D12GraphicsCommandList* commandList,
		uint32_t editTextureIndex,
		const CD3DX12_GPU_DESCRIPTOR_HANDLE& copyGPUHandle);

	/// <summary>
	/// クリアコマンド
	/// </summary>
	/// <param name="commandList">コマンドリスト</param>
	/// <param name="editTextureIndex">編集する画像番号</param>
	/// <param name="color">色</param>
	void ClearCommand(
		ID3D12GraphicsCommandList* commandList,
		uint32_t editTextureIndex,
		const Vector4& color = { 0.1f, 0.25f, 0.5f, 1.0f });

	/// <summary>
	/// 二値化
	/// </summary>
	/// <param name="commandList">コマンドリスト</param>
	/// <param name="editTextureIndex">編集する画像番号</param>
	/// <param name="threshold">しきい値</param>
	/// <param name="copyGPUHandle">二値化する画像のGPUハンドル</param>
	void BinaryThresholdCommand(
		ID3D12GraphicsCommandList* commandList,
		uint32_t editTextureIndex, 
		float threshold,
		const CD3DX12_GPU_DESCRIPTOR_HANDLE& binaryThresholdGPUHandle);

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

