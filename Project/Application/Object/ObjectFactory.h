#pragma once
#include "../../Engine/Object/AbstractObjectFactory.h"
#include "../../Engine/Level/LevelData.h"

class ObjectFactory :
    public AbstractObjectFactory
{

public: // 静的メンバ関数

	/// <summary>
	/// インスタンス取得
	/// </summary>
	/// <returns></returns>
	static ObjectFactory* GetInstance();

public: //メンバ関数
	
	/// <summary>
	/// オブジェクト作成
	/// </summary>
	/// <param name="objectData">オブジェクトデータ/param>
	/// <returns>オブジェクト</returns>
	IObject* CreateObject(LevelData::ObjectData& objectData) override;

private:
	ObjectFactory() = default;
	~ObjectFactory() = default;
	ObjectFactory(const ObjectFactory&) = delete;
	const ObjectFactory& operator=(const ObjectFactory&) = delete;

};

