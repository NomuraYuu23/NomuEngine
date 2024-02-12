#pragma once
#include "../../externals/nlohmann/json.hpp"

struct Vector2 {

	float x;
	float y;

};

inline void to_json(nlohmann::json& json, const Vector2& value) {
	json = nlohmann::json::array({ value.x, value.y });
}

inline void from_json(const nlohmann::json& json, Vector2& value) {
	if (json.is_array() && json.size() == 2) {
		// float型のjson配列登録
		value = { json.at(0), json.at(1) };
	}
}
