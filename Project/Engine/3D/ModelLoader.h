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
	/// 
	/// </summary>
	static ModelNode ReadNode(aiNode* node);

	//gltfファイルを読む
	//static Model::ModelData LoadGlTFFile(const std::string& directoryPath, const std::string& filename);

};

