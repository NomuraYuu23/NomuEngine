#pragma once

#include "Model.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

class ModelLoader
{

public:

	//objファイルを読む
	static Model::ModelData LoadModelFile(const std::string& directoryPath, const std::string& filename);

	/// <summary>
	/// ノード読み込み
	/// </summary>
	static ModelNode ReadNode(aiNode* node);

};

