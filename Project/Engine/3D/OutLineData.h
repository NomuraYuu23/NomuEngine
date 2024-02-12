#pragma once
#include "../Math/Vector4.h"
#include "../Math/Matrix4x4.h"
#include <wrl.h>
#include <d3d12.h>
struct OutLineData
{
	struct forVS {
		Matrix4x4 scaleMatrix;
	};
	struct forPS {
		Vector4 color;
	};

	~OutLineData() {};
	//初期化
	void Initialize();
	//データ転送
	void TransfarData();
	//マップ(初期化時自動実行)
	void Map();

	//Scale用のリソースを作る。Matrix4x4 1つ分のサイズを用意する
	Microsoft::WRL::ComPtr<ID3D12Resource> forVSResource_;
	forVS* forVSData_;
	//Color用のリソースを作る。Matrix4x4 1つ分のサイズを用意する
	Microsoft::WRL::ComPtr<ID3D12Resource> forPSResource_;
	forPS* forPSData_;

	float width_;
	Vector4 color_;
};
