#pragma once
#include "WorldTransform.h"
#include "../Camera/BaseCamera.h"
#include "Material.h"
#include "../Animation/LocalMatrixManager.h"
#include "Model.h"
class ModelDraw
{

public: // サブクラス

	struct AnimObjectDesc
	{
		Model* model; //モデル
		WorldTransform& worldTransform; // ワールドトランスフォーム
		LocalMatrixManager& localMatrixManager;// ローカル行列マネージャー
		BaseCamera& camera; // カメラ
		Material* material; // マテリアル(なくてもいい)
		std::vector<UINT> textureHandles; // テクスチャハンドル(なくてもいい)
	};

public:

	static void PreAnimObjectDraw();

	static void AnimObjectDraw(AnimObjectDesc& desc);

	static void PostDraw();

};

