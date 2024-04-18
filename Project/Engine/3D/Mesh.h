#pragma once
#include <DirectXMath.h>
#include <Windows.h>
#include <d3d12.h>
#include <string>
#include <wrl.h>
#include <dxcapi.h>

#pragma comment(lib, "dxcompiler.lib")

#include "VertexData.h"
#include "VertexInfluence.h"

class Mesh
{

public:

	/// <summary>
	/// メッシュデータ生成
	/// </summary>
	void CreateMesh(
		ID3D12Device* sDevice,
		const std::vector<VertexData>& vertices,
		const std::vector<VertexInfluence>& vertexInfluences);

	/// <summary>
	///  頂点バッファビュー
	/// </summary>
	/// <returns></returns>
	D3D12_VERTEX_BUFFER_VIEW* GetVbView() { return &vbView_; }

	/// <summary>
	///  インフルエンスバッファビュー
	/// </summary>
	/// <returns></returns>
	D3D12_VERTEX_BUFFER_VIEW* GetInfluenceView() { return &influenceView_; }

private:

	// 頂点バッファ
	Microsoft::WRL::ComPtr<ID3D12Resource> vertBuff_;
	// 頂点バッファマップ
	VertexData* vertMap = nullptr;
	// 頂点バッファビュー
	D3D12_VERTEX_BUFFER_VIEW vbView_{};

	// インフルエンスバッファ
	Microsoft::WRL::ComPtr<ID3D12Resource> influenceBuff_;
	// インフルエンスバッファマップ
	VertexInfluence* influenceMap = nullptr;
	// インフルエンスバッファビュー
	D3D12_VERTEX_BUFFER_VIEW influenceView_{};

};

