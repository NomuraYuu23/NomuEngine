#pragma once
#include <vector>
#include "../../externals/nlohmann/json.hpp"

//3次元ベクトル
struct Vector3
{
	float x;
	float y;
	float z;

};

inline void to_json(nlohmann::json& json, const Vector3& value) {
	json = nlohmann::json::array({ value.x, value.y, value.z });
}

inline void from_json(const nlohmann::json& json, Vector3& value) {
	if (json.is_array() && json.size() == 3) {
		// float型のjson配列登録
		value = { json.at(0), json.at(1), json.at(2) };
	}
}

class Vector3Calc
{
public:

	/// <summary>
	/// シングルトンインスタンスの取得
	/// </summary>
	/// <returns></returns>
	static Vector3Calc* GetInstance();

	//加算
	static Vector3 Add(const Vector3& v1, const Vector3& v2);
	//減算
	static Vector3 Subtract(const Vector3& v1, const Vector3& v2);
	//スカラー倍
	static Vector3 Multiply(float scalar, const Vector3& v);
	//内積
	static float Dot(const Vector3& v1, const Vector3& v2);
	//長さ（ノルム）
	static float Length(const Vector3& v);
	//正規化
	static Vector3 Normalize(const Vector3& v);
	//クロス積
	static Vector3 Cross(const Vector3& v1, const Vector3& v2);

	// 線形補間
	static Vector3 Lerp(const Vector3& v1, const Vector3& v2, float t);

	// 3次スプライン曲線
	static Vector3 CatmullRomSpline(
		const std::vector<Vector3>& controlPoints, const float& t);

	// 反射ベクトル
	static Vector3 Reflect(const Vector3& input, const Vector3& normal);

private:
	Vector3Calc() = default;
	~Vector3Calc() = default;
	Vector3Calc(const Vector3Calc&) = delete;
	Vector3Calc& operator=(const Vector3Calc&) = delete;

};
