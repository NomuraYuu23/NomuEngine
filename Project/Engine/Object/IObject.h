#pragma once
#include <string>

#include "../3D/WorldTransform.h"

class IObject
{

public: // 関数

	/// <summary>
	/// デストラクタ
	/// </summary>
	virtual ~IObject() = default;

	/// <summary>
	/// 初期化
	/// </summary>
	//virtual void Initialize() = 0;
	
	/// <summary>
	/// 更新
	/// </summary>
	virtual void Update() = 0;

	/// <summary>
	/// 名前取得
	/// </summary>
	/// <returns></returns>
	std::string GetName() { return name_; }

protected: // 変数

	// 名前
	std::string name_;

	// トランスフォーム
	WorldTransform worldTransform_;

};

