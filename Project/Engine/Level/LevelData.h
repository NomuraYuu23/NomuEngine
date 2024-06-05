#pragma once
#include <string>
#include <variant>
#include <vector>

#include "../3D/TransformStructure.h"

class LevelData
{

public: // サブクラス

	/// <summary>
	/// オブジェクトのデータの種類
	/// </summary>
	enum ObjectDataIndex {
		kObjectDataIndexMesh, // メッシュ
		kObjectDataIndexCamaera, // カメラ
		kObjectDataIndexLight, // ライト
		kObjectDataIndexOfCount // 数数える用
	};

	/// <summary>
	/// メッシュタイプ
	/// </summary>
	struct MeshData
	{
		// 名前
		std::string name;
		// ファイルの名前
		std::string flieName;
		// トランスフォーム
		EulerTransform transform;
	};

	/// <summary>
	/// カメラタイプ
	/// </summary>
	struct CamaeraData
	{
		// 名前
		std::string name;
		// トランスフォーム
		EulerTransform transform;
	};


	/// <summary>
	/// ライトタイプ
	/// </summary>
	struct LightData
	{
		// 名前
		std::string name;
		// トランスフォーム
		EulerTransform transform;
	};

	using ObjectData = std::variant<MeshData, CamaeraData, LightData>;

public: // 変数

	// オブジェクトの情報
	std::vector<ObjectData> objectsData_;


};

