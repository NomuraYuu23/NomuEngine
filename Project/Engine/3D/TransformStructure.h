#pragma once
#include "../Math/Vector3.h"
#include "../../externals/nlohmann/json.hpp"

//Transform構造体
struct TransformStructure {
	Vector3 scale;
	Vector3 rotate;
	Vector3 translate;
};

inline void to_json(nlohmann::json& json, const TransformStructure& value) {
	json = nlohmann::json{
		{"scale",value.scale },
		{"rotate",value.rotate },
		{"translate",value.translate },
	};
}

inline void from_json(const nlohmann::json& json, TransformStructure& value) {

    // キーを確認
    if (json.contains("scale") && json["scale"].is_array() &&
        json.contains("rotate") && json["rotate"].is_array() &&
        json.contains("translate") && json["translate"].is_array()) {

        value.scale = json["scale"].get<Vector3>();
        value.rotate = json["rotate"].get<Vector3>();
        value.translate = json["translate"].get<Vector3>();
    }

}
