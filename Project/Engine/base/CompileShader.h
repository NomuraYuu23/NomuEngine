#pragma once
#include <d3d12.h>
#include <wrl.h>
#include <dxcapi.h>
#include <format>

class CompileShader
{

public:

	/// <summary>
	/// シェーダーコンパイル用
	/// </summary>
	struct CompileShaderStruct {
		IDxcUtils* dxcUtils = nullptr;
		IDxcCompiler3* dxcCompiler = nullptr;
		IDxcIncludeHandler* includeHandler = nullptr;
	};

	// シェーダーコンパイル用
	static CompileShaderStruct compileShaderStruct_;

	// デバイス
	static ID3D12Device* sDevice_;

public:


	/// <summary>
	/// dxcCompilerを初期化
	/// </summary>
	/// <param name="sDevice">デバイス</param>
	static void Initialize(ID3D12Device* sDevice);

	/// <summary>
	/// Shaderをコンパイルする
	/// </summary>
	/// <param name="filePath">CompilerするShanderファイルへのパス</param>
	/// <param name="profile">Compilenに使用するProfile</param>
	/// <param name="entryPoint">エントリポイント</param>
	/// <returns></returns>
	static IDxcBlob* Compile(
		const std::wstring& filePath,
		const wchar_t* profile,
		const wchar_t* entryPoint = L"main");

private: // シングルトン

	CompileShader() = default;
	~CompileShader() = default;
	CompileShader(const CompileShader&) = delete;
	CompileShader& operator=(const CompileShader&) = delete;

};

