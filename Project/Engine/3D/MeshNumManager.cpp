#include "MeshNumManager.h"
#include "../base/DirectXCommon.h"
#include "../base/BufferResource.h"

void MeshNumManager::Initialize()
{


	//WVP用のリソースを作る。Matrix4x4 1つ分のサイズを用意する
	meshNumDataBuff_ = BufferResource::CreateBufferResource(DirectXCommon::GetInstance()->GetDevice(), (sizeof(MeshNumData) + 0xff) & ~0xff);
	//書き込むためのアドレスを取得
	meshNumDataBuff_->Map(0, nullptr, reinterpret_cast<void**>(&meshNumDataMap_));

	for (int i = 0; i < 32; ++i) {
		meshNumDataMap_->incrementMeshNum[i] = 0;
	}

	meshNumDataMap_->incrementMeshNumMax = 0;

}
