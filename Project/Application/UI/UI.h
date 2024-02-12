#pragma once
#include <memory>
#include <cstdint>
#include "../../Engine/2D/Sprite.h"

class UI
{

public: // メンバ関数

	virtual void Initialize(uint32_t textureHandle, const std::string& groupName, const std::string& jsonName = "UI");

	virtual void Initialize(uint32_t textureHandle, const std::string& groupName, const Vector2& size, const Vector2& leftTop, const std::string& jsonName = "UI");

	virtual void Update();

	virtual void Update(const Vector2& leftTop);

	void Draw();

private: // メンバ関数

	/// <summary>
	/// 外部変数登録
	/// </summary>
	virtual void RegisteringGlobalVariables();

	/// <summary>
	/// 外部変数適用
	/// </summary>
	virtual void ApplyGlobalVariables();

protected: // メンバ変数

	std::unique_ptr<Sprite> sprite_;

	Vector2 position_;

	float rotate_;

	Vector2 size_;

	std::string jsonName_;

	std::string groupName_;

public:

	/// <summary>
	/// セットカラー
	/// </summary>
	/// <param name="color"></param>
	void SetColor(const Vector4& color);

	/// <summary>
	/// 表示するか
	/// </summary>
	/// <param name="isInvisible"></param>
	void SetIsInvisible(bool isInvisible);

	/// <summary>
	/// 表示するか
	/// </summary>
	bool GetIsInvisible();

	/// <summary>
	/// テクスチャハンドル
	/// </summary>
	/// <param name="textureHandle"></param>
	void SetTextureHandle(uint32_t textureHandle);

	/// <summary>
	/// 回転
	/// </summary>
	/// <param name="rotate"></param>
	void SetRotate(float rotate);

	void SetPosition(const Vector2& positon);
	Vector2 GetPosition();

	void SetSize(const Vector2& size);
	Vector2 GetSize();

	void SetAnchorPoint(const Vector2& anchorPoint);

};

