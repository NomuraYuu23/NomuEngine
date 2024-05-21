#pragma once
#include <DirectXMath.h>
#include <Windows.h>
#include <d3d12.h>
#include <string>
#include <wrl.h>
#include <dxcapi.h>

#pragma comment(lib, "dxcompiler.lib")

#include "../Math/Vector4.h"
#include "../3D/TransformationMatrix.h"
#include "../3D/Material.h"
#include "../Camera/BaseCamera.h"

class SkyBox
{

public: // サブクラス

	struct VertexData {
		Vector4 position;
	};

public: // 関数

	void Initialize(uint32_t textureHandle);

	void Draw(ID3D12GraphicsCommandList* commandList, BaseCamera* camera);

private: // 関数

	void VertMapping();

	void IndexMapping();

private: // 定数

	// 頂点数
	static const size_t kVertNum;
	// index数
	static const size_t kIndexNum;

private: // 変数

	// 頂点バッファ
	Microsoft::WRL::ComPtr<ID3D12Resource> vertBuff_;
	// 頂点バッファマップ
	SkyBox::VertexData* vertMap_ = nullptr;
	// 頂点バッファビュー
	D3D12_VERTEX_BUFFER_VIEW vbView_{};

	// インデックスバッファ
	Microsoft::WRL::ComPtr<ID3D12Resource> indexBuff_;
	//インデックスマップ
	uint32_t* indexMap_ = nullptr;
	//インデックスバッファビュー
	D3D12_INDEX_BUFFER_VIEW ibView_{};

	// トランスフォームバッファ
	Microsoft::WRL::ComPtr<ID3D12Resource> transformationBuff_;
	// トランスフォーム行列マップ
	TransformationMatrix* transformationMatrixMap_;

	// マテリアル
	std::unique_ptr<Material> material_;

	// テクスチャハンドル
	uint32_t textureHandle_;

};

