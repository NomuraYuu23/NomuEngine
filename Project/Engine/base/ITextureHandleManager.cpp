#include "ITextureHandleManager.h"
#include "TextureManager.h"

void ITextureHandleManager::Initialize()
{
}

void ITextureHandleManager::SetTextureHandle(uint32_t textureHandle)
{

	textureHandles_[indexNextTextureHandle_] = textureHandle;
	indexNextTextureHandle_++;

}

void ITextureHandleManager::ResetTextureHandles()
{

	//std::vector<uint32_t> textureHandles;

	//for (uint32_t i = 0; i < indexNextTextureHandle_; ++i) {
	//	textureHandles.push_back(textureHandles_[i]);
	//}

	//TextureManager::GetInstance()->ResetTexture(textureHandles);
	//indexNextTextureHandle_ = 0u;

}
