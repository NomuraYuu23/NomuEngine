#pragma once
#include <vector>
#include "BoneData.h"
#include <map>
#include <string>
class AnimationFile
{

public:

	/// <summary>
	/// インスタンスの取得
	/// </summary>
	/// <returns></returns>
	static AnimationFile* GetInstance();

	/// <summary>
	/// オブジェクトの作成
	/// </summary>
	/// <param name="partName">名</param>
	void CreateObject(const std::string& objectName);

	/// <summary>
	/// パーツの作成
	/// </summary>
	/// <param name="motionName">グループ名</param>
	void CreatePart(const std::string& objectName, const std::string& partName);

	/// <summary>
	/// モーションの作成
	/// </summary>
	/// <param name="motionName">グループ名</param>
	void CreateMotion(const std::string& objectName, const std::string& partName, const std::string& motionName);

	/// <summary>
	///  ファイルに書き出し
	/// </summary>
	/// <param name="objectName">オブジェクト名</param>
	/// <param name="partName">パーツ名</param>
	void SaveFile(const std::string& objectName, const std::string& partName);

	/// <summary>
	/// 値のセット
	/// </summary>
	/// <param name="objectName">オブジェクト名</param>
	/// <param name="partName">パーツ名</param>
	/// <param name="motionName">モーション名</param>
	/// <param name="value">値</param>
	void SetValue(const std::string& objectName, const std::string& partName, const std::string& motionName, std::vector<BoneData> value);

	/// <summary>
	/// 毎フレーム処理
	/// </summary>
	void Update();

	/// <summary>
	/// ディレクトリの全ファイル読み込み
	/// </summary>
	void LoadFiles();

	/// <summary>
	/// ファイルから読み込む
	/// </summary>
	/// <param name="directoryPath">ディレクトリパス</param>
	/// <param name="partName">パーツ名</param>
	void LoadFile(const std::string& directoryPath, const std::string& partName);

	/// <summary>
	/// モーションの追加
	/// </summary>
	/// <param name="objectName">オブジェクト名</param>
	/// <param name="partName">パーツ名</param>
	/// <param name="motionName">モーション名</param>
	/// <param name="value">値</param>
	void AddMotion(const std::string& objectName, const std::string& partName, const std::string& motionName, std::vector<BoneData> value);

	// <summary>
	/// 値の取得
	/// </summary>
	/// <param name="objectName">オブジェクト名</param>
	/// <param name="partName">パーツ名</param>
	/// <param name="motionName">モーション名</param>
	/// <returns>値</returns>
	std::vector<BoneData> GetValue(const std::string& objectName, const std::string& partName, const std::string& motionName);

	/// <summary>
	/// アニメーションファイルの作成
	/// </summary>
	/// <param name="root">jsonファイル</param>
	/// <param name="objectName">オブジェクト名</param>
	/// <param name="partName">パーツ名</param>
	void CreateAnimationFile(const nlohmann::json& root, const std::string& objectName, const std::string& partName);

private:
	AnimationFile() = default;
	~AnimationFile() = default;
	AnimationFile(const AnimationFile&) = delete;
	const AnimationFile& operator=(const AnimationFile&) = delete;

	//項目
	using Motion = std::vector<BoneData>;
	using Part = std::map<std::string, Motion>; // 歩きとか
	using Object = std::map<std::string, Part>; // 腕とか

	// 全データ
	std::map<std::string, Object> datas_; // Playerとか

	// グローバル変数の保存先ファイルパス
	const std::string kDirectoryPath = "Resources/AnimationFile/";

	// 新しいモーション名
	// 新しいモーション名
	std::string newMotionName = "";

};

