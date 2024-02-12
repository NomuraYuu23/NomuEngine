#pragma once
#include "../3D/TransformStructure.h"

struct BoneData
{
	uint32_t endFrame_;
	TransformStructure transform_;
	uint32_t easeType_;

};

inline void to_json(nlohmann::json& json, const BoneData& value) {
	json = nlohmann::json{
		{"endFrame",value.endFrame_ },
		{"transform",value.transform_ },
		{"easeType",value.easeType_ },
	};
}

inline void from_json(const nlohmann::json& json, BoneData& value) {

	// キーを確認
	if (json.contains("endFrame") && json["endFrame"].is_number_integer() &&
		json.contains("transform") && json["transform"].is_object() &&
		json.contains("easeType") && json["easeType"].is_number_integer()) {

		value.endFrame_ = json["endFrame"];
		value.transform_ = json["transform"];
		value.easeType_ = json["easeType"];
	}

}
