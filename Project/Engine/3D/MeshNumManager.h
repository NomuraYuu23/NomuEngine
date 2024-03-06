#pragma once
#include <wrl.h>
#include <d3d12.h>
#include <cstdint>

class MeshNumManager
{

public:

	struct MeshNumData{

		uint32_t incrementMeshNum[4];
		uint32_t incrementMeshNumMax;
		//uint32_t padding[3];
	};

public:

	void Initialize();

public:

	//WVP用のリソースを作る。Matrix4x4 1つ分のサイズを用意する
	Microsoft::WRL::ComPtr<ID3D12Resource> meshNumDataBuff_;
	//書き込むためのアドレスを取得
	MeshNumData* meshNumDataMap_{};

};

