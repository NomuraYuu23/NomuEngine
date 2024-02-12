#include "UIManager.h"
#include "../../Engine/Math/Ease.h"

void UIManager::Initialize(const std::array<uint32_t, UITextureHandleIndex::kUITextureHandleIndexOfCount>& textureHandles)
{

	textureHandles_ = textureHandles;

	UIInitialize();

	for (uint32_t i = 0; i < UIIndex::kUIIndexOfCount; ++i) {
		UIInitPositions_[i] = UIs_[i]->GetPosition();
	}

}

void UIManager::Update(const UIManagerUpdateDesc& uiManagerUpdateDesc)
{

	Vector2 leftTop = { 0.0f, 0.0f };

	// フレーム
	UIs_[kUIIndexSample]->Update();

}

void UIManager::Draw()
{

	for (uint32_t i = 0;i < UIIndex::kUIIndexOfCount; ++i) {
		UIs_[i]->Draw();
	}

}

void UIManager::UIInitialize()
{

	Vector2 leftTop = { 0.0f, 0.0f };
	Vector2 sampleSize = { 128.0f, 128.0f };

	// フレーム
	leftTop = { 0.0f, 0.0f };
	UIs_[kUIIndexSample] = std::make_unique<UI>();
	UIs_[kUIIndexSample]->Initialize(textureHandles_[kUITextureHandleIndexSample], "UIIndexSample", sampleSize, leftTop);

}
