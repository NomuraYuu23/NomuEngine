#pragma once

#include "Model.h"

class ModelLoader
{

public:

	//objファイルを読む
	static Model::ModelData LoadObjFile(const std::string& directoryPath, const std::string& filename);

	static Model::MaterialData LoadMaterialTemplateFile(const std::string& directoryPath, const std::string& filename);

};

