#pragma once
#include <array>

class ITextureHandleManager
{

public: // 静的メンバ変数

	static const uint32_t kNumTextureHandle = 256;

public: // メンバ関数

	~ITextureHandleManager() = default;

	void Initialize();

	void SetTextureHandle(uint32_t textureHandle);

	void ResetTextureHandles();

protected: // メンバ変数

	// テクスチャハンドル
	std::array<uint32_t, kNumTextureHandle> textureHandles_;

	// 次の格納場所
	uint32_t indexNextTextureHandle_ = 0u;

};

