#pragma once
#include <cstdint>
#include "WinApp.h"
#include "../base/TextureUAV.h"

class WindowSpriteStorage
{

public: // 関数

	/// <summary>
	/// インスタンス取得
	/// </summary>
	static WindowSpriteStorage* GetInstance();

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// imGui描画
	/// </summary>
	void ImGuiDraw();

	/// <summary>
	/// リセット
	/// </summary>
	void Reset();

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

public: // サブクラス

	/// <summary>
	/// パイプライン名前
	/// </summary>
	enum PipelineIndex {
		kPipelineIndexCopyCS, // コピー
		kPipelineIndexOverwrite, // 上書き
		kPipelineIndexOfCount // 数を数える用
	};

	/// <summary>
	///  CSに渡す定数バッファ
	/// </summary>
	struct ComputeParameters {

		uint32_t threadIdOffsetX; // スレッドのオフセットX
		uint32_t threadIdTotalX; // スレッドの総数X
		uint32_t threadIdOffsetY; // スレッドのオフセットY
		uint32_t threadIdTotalY; // スレッドの総数Y
		uint32_t threadIdOffsetZ; // スレッドのオフセットZ
		uint32_t threadIdTotalZ; // スレッドの総数Z
		float padding[2]; // パディング

		Vector4 clearColor; // クリアするときの色

		uint32_t overwriteCount; // 上書き回数

	};

private: // 定数

	// シェーダー情報 <シェーダ名, エントリポイント>
	const std::array<std::pair<const std::wstring, const wchar_t*>, kPipelineIndexOfCount> shaderNames_ =
	{
		std::pair{L"Resources/shaders/WindowSpriteStorage.CS.hlsl", L"mainCopy"}, // レンダーターゲット画像の修正＆コピー
		std::pair{L"Resources/shaders/WindowSpriteStorage.CS.hlsl", L"mainOverwrite"}, // 上書き

	};

	// 画像の幅
	const uint32_t kTextureWidth = WinApp::kWindowWidth;
	// 画像の高さ
	const uint32_t kTextureHeight = WinApp::kWindowHeight;
	// スレッド数X
	const uint32_t kNumThreadX = 32;
	// スレッド数Y
	const uint32_t kNumThreadY = 32;
	// スレッド数Z
	const uint32_t kNumThreadZ = 1;

private: // 変数

	// デバイス
	ID3D12Device* device_ = nullptr;

	// コマンドリスト
	ID3D12GraphicsCommandList* commandList_ = nullptr;

	//OverwriteParameter用のリソースを作る。
	Microsoft::WRL::ComPtr<ID3D12Resource> computeParametersBuff_;
	// computeParametersのマップ
	ComputeParameters* computeParametersMap_ = nullptr;

	//パイプライン
	std::array<Microsoft::WRL::ComPtr<ID3D12PipelineState>, kPipelineIndexOfCount > pipelineStates_;
	// ルートシグネチャ
	Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature_;
	// シェーダー情報
	std::array<Microsoft::WRL::ComPtr<IDxcBlob>, kPipelineIndexOfCount> shaders_;

private: // 一時保存 定数

	// 保存する画像最大数
	static const uint32_t kTemporaryStorageTexturesNum = 8;

public: // 一時保存 関数

	/// <summary>
	/// 一時保存 登録
	/// </summary>
	/// <param name="commandList">コマンドリスト</param>
	/// <param name="GPUHandle">GPUハンドル</param>
	/// <param name="">名前</param>
	void TemporaryStorageRegister(
		ID3D12GraphicsCommandList* commandList,
		const CD3DX12_GPU_DESCRIPTOR_HANDLE& GPUHandle,
		const std::string& name);

	/// <summary>
	/// 一時保存に登録された画像で上書きする
	/// </summary>
	/// <param name="commandList">コマンドリスト</param>
	/// <param name="temporaryStorageNames">保存された画像の名前 上書き順（ない場合は登録順）</param>
	void TemporaryStoragOverwrite(
		ID3D12GraphicsCommandList* commandList,
		std::array<std::string, kTemporaryStorageTexturesNum>* temporaryStorageNames = nullptr);

private: // 一時保存 関数

	/// <summary>
	/// 一時保存 初期化
	/// </summary>
	void TemporaryStorageInitialize();

private: // 一時保存 変数

	// 保存する画像
	std::array<std::pair<std::string, std::unique_ptr<TextureUAV>>, kTemporaryStorageTexturesNum> temporaryStorageTextures_;

	// 保存された画像をすべて上書きした画像
	std::unique_ptr<TextureUAV> temporaryStorageOverwriteTexture_;

public: // アクセッサ

	TextureUAV* GetTemporaryStorageOverwriteTexture() { return temporaryStorageOverwriteTexture_.get(); }

private: // シングルトン
	WindowSpriteStorage() = default;
	~WindowSpriteStorage() = default;
	WindowSpriteStorage(const WindowSpriteStorage&) = delete;
	const WindowSpriteStorage& operator=(const WindowSpriteStorage&) = delete;

};

