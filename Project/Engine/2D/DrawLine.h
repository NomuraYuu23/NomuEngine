#pragma once
#include <DirectXMath.h>
#include <Windows.h>
#include <d3d12.h>
#include <string>
#include <wrl.h>
#include <dxcapi.h>
#include <list>
#include <memory>
#include "../2D/ColorVertexData.h"
#include "../Math/Matrix4x4.h"
#include "../Camera/BaseCamera.h"

#pragma comment(lib, "dxcompiler.lib")

class DrawLine
{

public:

	/// <summary>
	/// 静的初期化
	/// </summary>
	/// <param name="device">デバイス</param>
	/// <param name="rootSignature">ルートシグネチャ</param>
	/// <param name="pipelineState">パイプラインステート</param>
	static void StaticInitialize(
		ID3D12Device* device,
		ID3D12RootSignature* rootSignature,
		ID3D12PipelineState* pipelineState);

	/// <summary>
	/// 静的前処理
	/// </summary>
	/// <param name="cmdList">描画コマンドリスト</param>
	static void PreDraw(ID3D12GraphicsCommandList* cmdList);

	/// <summary>
	/// 描画後処理
	/// </summary>
	static void PostDraw();

	/// <summary>
	/// 線を作る
	/// </summary>
	/// <returns></returns>
	static DrawLine* Create();

private:

	// 頂点数
	static const int kVertNum = 2;
	// デバイス
	static ID3D12Device* sDevice;
	// コマンドリスト
	static ID3D12GraphicsCommandList* sCommandList;
	// ルートシグネチャ
	static ID3D12RootSignature* sRootSignature;
	// パイプラインステートオブジェクト
	static ID3D12PipelineState* sPipelineState;

public:
	
	/// <summary>
	/// コンストラクタ
	/// </summary>
	DrawLine();

	/// <summary>
	/// 初期化
	/// </summary>
	/// <returns>成否</returns>
	bool Initialize();

	/// <summary>
	/// 描画
	/// </summary>
	/// <param name="position1">位置0</param>
	/// <param name="position2">位置1</param>
	/// <param name="color0">色0</param>
	/// <param name="color1">色1</param>
	/// <param name="camera">カメラ</param>
	void Draw(
		const Vector3& position0,
		const Vector3& position1,
		const Vector4& color0,
		const Vector4& color1,
		BaseCamera& camera);

private:

	// 頂点バッファ
	Microsoft::WRL::ComPtr<ID3D12Resource> vertBuff_;
	// 頂点バッファマップ
	ColorVertexData* vertMap = nullptr;
	// 頂点バッファビュー
	D3D12_VERTEX_BUFFER_VIEW vbView_{};

};

