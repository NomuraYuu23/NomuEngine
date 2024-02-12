#include "AnimationFile.h"
#include "../2D/ImGuiManager.h"
#include "../../externals/nlohmann/json.hpp"
#include <fstream>

AnimationFile* AnimationFile::GetInstance()
{
	static AnimationFile instance;
	return &instance;
}

void AnimationFile::CreateObject(const std::string& objectName)
{

	datas_[objectName];

}

void AnimationFile::CreatePart(const std::string& objectName, const std::string& partName)
{

	std::map<std::string, Object>::iterator itObject = datas_.find(objectName);

	//未登録チェック
	if (itObject == datas_.end()) {
		CreateObject(objectName);
	}

	datas_[objectName][partName];

}

void AnimationFile::CreateMotion(const std::string& objectName, const std::string& partName, const std::string& motionName)
{

	std::map<std::string, Object>::iterator itObject = datas_.find(objectName);

	//未登録チェック
	if (itObject == datas_.end()) {
		CreateObject(objectName);
	}

	std::map<std::string, Part>::iterator itPart = datas_.find(objectName)->second.find(partName);

	//未登録チェック
	if (itPart == datas_.find(objectName)->second.end()) {
		CreatePart(objectName, partName);
	}

	datas_[objectName][partName][motionName];

}

void AnimationFile::SaveFile(const std::string& objectName, const std::string& partName)
{

	std::map<std::string, Object>::iterator itObject = datas_.find(objectName);

	//未登録チェック
	assert(itObject != datas_.end());

	//グループを検索
	std::map<std::string, Part>::iterator itPart = itObject->second.find(partName);

	//未登録チェック
	assert(itPart != itObject->second.end());

	//グループ登録確認
	nlohmann::json root;

	root = nlohmann::json::object();

	//各項目について
	for (std::map<std::string, Motion>::iterator itMotion = itPart->second.begin();
		itMotion != itPart->second.end(); ++itMotion) {
		//項目名を取得
		std::string motionName = itMotion->first;
		for (uint32_t i = 0; i < motionName.size(); ++i) {
			if (motionName[i] == '\u0000') {
				motionName.resize(i);
				break;
			}
		}
		//項目の参照を取得
		Motion& motion = itMotion->second;

		// jsonオブジェクト登録
		root[motionName] = nlohmann::json::object();

		for (size_t i = 0; i < motion.size(); i++)
		{

			const std::string& keyFrame = std::to_string(i);

			root[motionName][keyFrame] = nlohmann::json::array();

			root[motionName][keyFrame] = motion[i];

		}

		CreateAnimationFile(root, objectName, partName);

	}

}

void AnimationFile::SetValue(const std::string& objectName, const std::string& partName, const std::string& motionName, std::vector<BoneData> value)
{

	// グループの参照を取得
	Part& part = datas_[objectName][partName];
	// 新しい項目のデータを設定
	Motion newMotion{};
	newMotion = value;
	// 設定した項目をstd::mapに追加
	part[motionName] = newMotion;

}

void AnimationFile::Update()
{

#ifdef _DEBUG

	if (!ImGui::Begin("AnimationFile", nullptr, ImGuiWindowFlags_MenuBar)) {
		ImGui::End();
		return;
	}
	if (!ImGui::BeginMenuBar())
		return;

	for (std::map<std::string, Object>::iterator itObject = datas_.begin(); itObject != datas_.end();
		++itObject) {

		// オブジェクト名を取得
		const std::string& objectName = itObject->first;
		// オブジェクト参照を取得
		Object& object = itObject->second;

		if (!ImGui::BeginMenu(objectName.c_str()))
			continue;

		//各グループについて
		for (std::map<std::string, Part>::iterator itPart = object.begin(); itPart != object.end();
			++itPart) {
			// グループ名を取得
			const std::string& partName = itPart->first;
			// グループの参照を取得
			Part& part = itPart->second;

			if (ImGui::TreeNode(partName.c_str())) {

				//各項目について
				for (std::map<std::string, Motion>::iterator itMotion = part.begin();
					itMotion != part.end(); ++itMotion) {

					//項目名を取得
					const std::string& motionName = itMotion->first;
					//項目の参照を取得
					Motion& motion = itMotion->second;

					if (ImGui::TreeNode(motionName.c_str())) {

						BoneData* ptr = nullptr;
						int preEndFrame = 0;
						for (size_t i = 0; i < motion.size(); i++)
						{

							if (ImGui::TreeNode(std::to_string(i).c_str())) {

								ptr = &motion[i];

								int endFrame = static_cast<int>(ptr->endFrame_);
								ImGui::DragInt("endFrame", &endFrame);
								if (preEndFrame < endFrame) {
									ptr->endFrame_ = static_cast<uint32_t>(endFrame);
									preEndFrame = endFrame;
								}
								else {
									ptr->endFrame_ = static_cast<uint32_t>(preEndFrame);
								}

								ImGui::DragFloat3("translate", &ptr->transform_.translate.x, 0.01f);
								ImGui::DragFloat3("rotate", &ptr->transform_.rotate.x, 0.01f);
								ImGui::DragFloat3("scale", &ptr->transform_.scale.x, 0.01f);

								int easeType = static_cast<int>(ptr->easeType_);
								ImGui::DragInt("easeType", &easeType);
								ptr->easeType_ = static_cast<uint32_t>(easeType);

								ImGui::TreePop();

							}

						}

						//改行
						ImGui::Text("\n");

						// フレームを増やす
						if (ImGui::Button("AddFrame")) {
							BoneData newBoneData = {
								static_cast<uint32_t>(preEndFrame),
								1.0f, 1.0f, 1.0f,
								0.0f, 0.0f, 0.0f,
								0.0f, 0.0f, 0.0f,
								0
							};
							motion.push_back(newBoneData);
						}
						// 最後のフレームを消す
						if (ImGui::Button("DeleteLastFrame")) {
							if (motion.size() != 0) {
								motion.pop_back();
							}
						}

						ImGui::TreePop();
					
					}

				}

				//改行
				ImGui::Text("\n");

				ImGui::InputText("newMotionName", newMotionName.data(), 16);
				// 文字列
				if (newMotionName.size() < 16u) {
					newMotionName.resize(16u);
				}

				if (ImGui::Button("AddNewMotionName")) {
					CreateMotion(objectName, partName, newMotionName);
				}

				//改行
				ImGui::Text("\n");

				if (ImGui::Button("Save")) {
					std::string message = std::format("Do you want to save the {} animation file?", partName);
					int ans = MessageBoxA(nullptr, message.c_str(), "Save confirmation",
						MB_ICONQUESTION | MB_OKCANCEL);
					if (ans == 1) {
						SaveFile(objectName, partName);
					}
				}

				ImGui::TreePop();
			
			}

		}

		ImGui::EndMenu();

	}

	ImGui::EndMenuBar();
	ImGui::End();

#endif // _DEBUG

}

void AnimationFile::LoadFiles()
{

	// 保存先ディレクトリのパスをローカル変数で宣言する
	std::filesystem::path dir(kDirectoryPath);
	// ディレクトリがなければスキップする
	if (!std::filesystem::exists(dir)) {
		return;
	}

	//各ファイルの処理
	std::filesystem::directory_iterator objectDir_it(kDirectoryPath);
	for (const std::filesystem::directory_entry& objectEntry : objectDir_it) {

		std::filesystem::directory_iterator dir_it(objectEntry.path());
		for (const std::filesystem::directory_entry& entry : dir_it) {

			// ファイルパスを取得
			const std::filesystem::path& filePath = entry.path();

			// ファイル拡張子を取得
			std::string extension = filePath.extension().string();
			// .jsonファイル以外はスキップ
			if (extension.compare(".json") != 0) {
				continue;
			}

			// ファイル読み込み
			LoadFile(objectEntry.path().string(), filePath.stem().string());

		}

	}

}

void AnimationFile::LoadFile(const std::string& directoryPath, const std::string& partName)
{

	// 読み込むJSONファイルのフルパスを合成する
	std::string filePath = directoryPath + "/" + partName + ".json";
	// オブジェクトネーム
	std::string objectName = directoryPath.substr(kDirectoryPath.size());

	// 読み込み用ファイルストリーム
	std::ifstream ifs;
	// ファイルを読み込み用に聞く
	ifs.open(filePath);

	// ファイルオープン失敗?
	if (ifs.fail()) {
		std::string message = "Failed open data file for write.";
		MessageBoxA(nullptr, message.c_str(), "AnimationFile", 0);
		assert(0);
		return;
	}

	nlohmann::json root;

	// json文字列からjsonのデータ構造に展開
	ifs >> root;
	// ファイルを閉じる
	ifs.close();

	// 登録用データ
	uint32_t boneDataSize = (sizeof(BoneData) + 0xff) & ~0xff;

	// 各アイテムについて
	for (nlohmann::json::iterator itMotion = root.begin(); itMotion != root.end(); ++itMotion) {

		// アイテム名を取得
		const std::string& motionName = itMotion.key();

		// float型のjson配列登録
		std::vector<BoneData> boneDatas;

		for (nlohmann::json::iterator itFrameIndex = itMotion->begin(); itFrameIndex != itMotion->end(); ++itFrameIndex) {
			BoneData boneData = itFrameIndex->get<BoneData>();
			boneDatas.push_back(boneData);
		}

		SetValue(objectName, partName, motionName, boneDatas);

	}

}

void AnimationFile::AddMotion(const std::string& objectName, const std::string& partName, const std::string& motionName, std::vector<BoneData> value)
{

	// 項目が未登録なら
	if (datas_[objectName][partName].find(motionName) == datas_[objectName][partName].end()) {
		SetValue(objectName, partName, motionName, value);
	}

}

std::vector<BoneData> AnimationFile::GetValue(const std::string& objectName, const std::string& partName, const std::string& motionName)
{

	assert(datas_.find(objectName) != datas_.end());
	// グループの参照を取得
	Part& part = datas_[objectName][partName];

	assert(part.find(motionName) != part.end());

	return part[motionName];
}

void AnimationFile::CreateAnimationFile(const nlohmann::json& root, const std::string& objectName, const std::string& partName)
{

	//ディレクトリがなければ作成する
	std::filesystem::path objectDir_it(kDirectoryPath);
	if (!std::filesystem::exists(objectDir_it)) {
		std::filesystem::create_directory(objectDir_it);
	}
	//ディレクトリがなければ作成する
	std::filesystem::path dir(kDirectoryPath + objectName + "/");
	if (!std::filesystem::exists(dir)) {
		std::filesystem::create_directory(dir);
	}

	// 書き込むJSONファイルのフルパスを合成する
	std::string filePath = kDirectoryPath + objectName + "/" + partName + ".json";
	// 書き込む用ファイルストリーム
	std::ofstream ofs;
	// ファイルを書き込み用に開く
	ofs.open(filePath);
	// ファイルオープン失敗
	if (ofs.fail()) {
		std::string message = "Failed open data file for write.";
		MessageBoxA(nullptr, message.c_str(), "AnimationFile", 0);
		assert(0);
		return;
	}

	// ファイルにjson文字列を書き込む(インデント幅4)
	ofs << std::setw(4) << root << std::endl;
	// ファイルを閉じる
	ofs.close();

}
