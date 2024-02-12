#include "UI.h"
#include "../../Engine/GlobalVariables/GlobalVariables.h"

void UI::Initialize(uint32_t textureHandle, const std::string& groupName, const std::string& jsonName)
{

	Vector2 position = { 0.0f,0.0f };

	Vector4 color = { 1.0f,1.0f,1.0f,1.0f };

	sprite_.reset(Sprite::Create(textureHandle, position, color));

	position_ = position;

	rotate_ = 0.0f;

	size_ = sprite_->GetSize();

	jsonName_ = jsonName;

	groupName_ = groupName;

	RegisteringGlobalVariables();

	ApplyGlobalVariables();

}

void UI::Initialize(uint32_t textureHandle, const std::string& groupName, const Vector2& size, const Vector2& leftTop, const std::string& jsonName)
{

	UI::Initialize(textureHandle, groupName, jsonName);

	sprite_->SetTextureSize(size);
	sprite_->SetTextureLeftTop(leftTop);

}

void UI::Update()
{

//#ifdef _DEBUG
	ApplyGlobalVariables();
//#endif // _DEBUG

}

void UI::Update(const Vector2& leftTop)
{

	UI::Update();

	sprite_->SetTextureLeftTop(leftTop);

}

void UI::Draw()
{

	sprite_->Draw();

}

void UI::RegisteringGlobalVariables()
{

	GlobalVariables* globalVariables = GlobalVariables::GetInstance();

	globalVariables->AddItem(jsonName_, groupName_ + "position", position_);
	globalVariables->AddItem(jsonName_, groupName_ + "rotate", rotate_);
	globalVariables->AddItem(jsonName_, groupName_ + "size", size_);

}

void UI::ApplyGlobalVariables()
{

	GlobalVariables* globalVariables = GlobalVariables::GetInstance();

	position_ = globalVariables->GetVector2Value(jsonName_, groupName_ + "position");
	rotate_ = globalVariables->GetFloatValue(jsonName_, groupName_ + "rotate");
	size_ = globalVariables->GetVector2Value(jsonName_, groupName_ + "size");

	sprite_->SetPosition(position_);
	sprite_->SetRotate(rotate_);
	sprite_->SetSize(size_);

}

void UI::SetColor(const Vector4& color)
{

	sprite_->SetColor(color);
	
}

void UI::SetIsInvisible(bool isInvisible)
{

	sprite_->SetIsInvisible(isInvisible);

}

bool UI::GetIsInvisible()
{
	return sprite_->GetIsInvisible();
}

void UI::SetTextureHandle(uint32_t textureHandle)
{

	sprite_->SetTextureHandle(textureHandle);

}

void UI::SetRotate(float rotate)
{

	sprite_->SetRotate(rotate);

}

void UI::SetPosition(const Vector2& positon)
{

	sprite_->SetPosition(positon);

}

Vector2 UI::GetPosition()
{
	return sprite_->GetPosition();
}

void UI::SetSize(const Vector2& size)
{
	sprite_->SetSize(size);
}

Vector2 UI::GetSize()
{
	return sprite_->GetSize();
}

void UI::SetAnchorPoint(const Vector2& anchorPoint)
{

	sprite_->SetAnchorPoint(anchorPoint);

}
