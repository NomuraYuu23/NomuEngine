#include "LevelDataViewing.h"
#include "../2D/ImguiManager.h"

const std::array<const std::string, LevelIndex::kLevelIndexOfCount>* LevelDataViewing::fileNames_ = nullptr;

std::array<std::unique_ptr<LevelData>, LevelIndex::kLevelIndexOfCount>* LevelDataViewing::levelDatas_ = nullptr;

LevelIndex LevelDataViewing::viewingIndex_ = LevelIndex::kLevelIndexSample;

void LevelDataViewing::Initialize(std::array<std::unique_ptr<LevelData>, LevelIndex::kLevelIndexOfCount>* levelDatas)
{

	levelDatas_ = levelDatas;
	viewingIndex_ = LevelIndex::kLevelIndexSample;

}

void LevelDataViewing::ImGuiViewing()
{

	ImGui::Begin("EnemyEditor");

	// 閲覧するlevelを切り替える
	ViewingIndexChange();
	
	// 表示
	Viewing();

	ImGui::End();

}

void LevelDataViewing::ViewingIndexChange()
{

	int32_t index = static_cast<int32_t>(viewingIndex_);

	for (uint32_t i = 0; i < LevelIndex::kLevelIndexOfCount; ++i) {

		ImGui::RadioButton(fileNames_->at(i).c_str(), &index, i);
		ImGui::SameLine();

	}

	viewingIndex_ = static_cast<LevelIndex>(index);

}

void LevelDataViewing::Viewing()
{

	// 線
	ImGui::SeparatorText("Datas");

	// データ群の参照
	std::vector<LevelData::ObjectData>* objectDatas = &levelDatas_->at(viewingIndex_)->objectsData_;

	// オブジェクトの走査
	for (uint32_t i = 0; i < objectDatas->size(); ++i) {

		// オブジェクトの参照
		LevelData::ObjectData* objectData = &objectDatas->at(i);

		// 型にあわせてObjectViewing
		std::visit([](auto& objectData_) {
			ObjectViewing(&objectData_);
			}, *objectData);

	}


}

void LevelDataViewing::ObjectViewing(LevelData::MeshData* objectData)
{

	// 名前
	NamaViewing(objectData->name);

	// ファイルの名前
	FileNameViewing(objectData->flieName);

	// トランスフォーム
	TransformViewing(objectData->transform);

}

void LevelDataViewing::ObjectViewing(LevelData::CameraData* objectData)
{

	// 名前
	NamaViewing(objectData->name);

	// トランスフォーム
	TransformViewing(objectData->transform);

}

void LevelDataViewing::ObjectViewing(LevelData::LightData* objectData)
{

	// 名前
	NamaViewing(objectData->name);

	// トランスフォーム
	TransformViewing(objectData->transform);

}

void LevelDataViewing::NamaViewing(const std::string& name)
{

	std::string text = "NAME::" + name;
	ImGui::SeparatorText(text.c_str());

}

void LevelDataViewing::FileNameViewing(const std::string& fileName)
{

	std::string text = "FILE_NAME::" + fileName;
	ImGui::Text(text.c_str());

}

void LevelDataViewing::TransformViewing(EulerTransform& transform)
{

	// 名前
	std::string text = "TRANSFORM";
	ImGui::Text(text.c_str());

	ImGui::DragFloat3("translation", &transform.translate.x, 0.0f);
	ImGui::DragFloat3("rotation", &transform.rotate.x, 0.0f);
	ImGui::DragFloat3("scaling", &transform.scale.x, 0.0f);

}
