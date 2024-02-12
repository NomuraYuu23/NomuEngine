#pragma once
#include <array>
#include <memory>
#include <cstdint>
#include "UI.h"
#include "../AudioManager/GameAudioManager.h"

enum UITextureHandleIndex {
	kUITextureHandleIndexSample, // サンプル
	kUITextureHandleIndexOfCount
};

enum UIIndex {
	kUIIndexSample, // サンプル
	kUIIndexOfCount
};

struct UIManagerUpdateDesc {

};

class UIManager
{

public: // メンバ関数

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(const std::array<uint32_t, UITextureHandleIndex::kUITextureHandleIndexOfCount>& textureHandles);

	/// <summary>
	/// 更新
	/// </summary>
	void Update(const UIManagerUpdateDesc& uiManagerUpdateDesc);

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

private: // メンバ関数

	/// <summary>
	/// UIごとの初期化
	/// </summary>
	void UIInitialize();

private: // メンバ変数

	// テクスチャハンドル
	std::array<uint32_t, UITextureHandleIndex::kUITextureHandleIndexOfCount> textureHandles_;

	// UI
	std::array<std::unique_ptr<UI>, UIIndex::kUIIndexOfCount> UIs_;

	// UI位置
	std::array<Vector2, UIIndex::kUIIndexOfCount> UIInitPositions_;

	// オーディオマネージャー
	GameAudioManager* audioManager_;

public:

	void SetAudioManager(GameAudioManager* audioManager) { audioManager_ = audioManager; }

};

