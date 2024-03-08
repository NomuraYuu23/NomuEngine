#pragma once
#include <cstdint>
#include "../Math/Matrix4x4.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

class SkinBone
{


public: // 関数

	/// <summary>
	/// 
	/// </summary>
	/// <param name="id"></param>
	/// <param name="nodeAnim"></param>
	void Initialize(uint32_t id, const aiSkeleton& nodeSkeleton);

	void ReadSkeleton(aiSkeleton* nodeSkeleton);

public: // 変数

	uint32_t id_; // 識別番号
	uint32_t childNum_; // 子の数

	SkinBone* children_; // 子

	Matrix4x4 offsetMatrix_; // 姿勢行列
	Matrix4x4 initMatrix_; // 初期姿勢行列
	Matrix4x4 boneMatrix_; // ローカル行列
	Matrix4x4* combMatrixArray_; // GPUに送るやつ

};

