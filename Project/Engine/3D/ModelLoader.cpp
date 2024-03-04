#include "ModelLoader.h"
#include <cassert>
#include <fstream>
#include <sstream>
#include <format>
#include "../base/BufferResource.h"

using namespace DirectX;
using namespace Microsoft::WRL;

//objファイルを読む
Model::ModelData ModelLoader::LoadModelFile(const std::string& directoryPath, const std::string& filename) {

	// 変数宣言
	Model::ModelData modelData; // 構築するModelData

	// シーン構築
	Assimp::Importer importer;
	std::string filePath = directoryPath + "/" + filename;
	// 読み込み時のオプション
	const aiScene* scene = importer.ReadFile(filePath.c_str(), aiProcess_FlipWindingOrder | aiProcess_FlipUVs);
	assert(scene->HasMeshes()); // メッシュがないのは対応しない

	// ノード解析
	modelData.rootNode = ReadNode(scene->mRootNode);

	uint32_t vertexCount = 0;
	modelData.meshNumManager.Initialize();

	// メッシュ多いよ
	assert(scene->mNumMeshes <= 4);

	// メッシュ解析
	for (uint32_t meshIndex = 0; meshIndex < scene->mNumMeshes; ++meshIndex) {
		aiMesh* mesh = scene->mMeshes[meshIndex];
		assert(mesh->HasNormals()); // 法線がないMeshは今回は非対応
		assert(mesh->HasTextureCoords(0)); // TexcoordがないMeshは今回は非対応

		// フェイス解析
		for (uint32_t faceIndex = 0; faceIndex < mesh->mNumFaces; ++faceIndex) {
			aiFace& face = mesh->mFaces[faceIndex];
			assert(face.mNumIndices == 3);// 三角形のみサポート

			// 頂点解析
			for (uint32_t element = 0; element < face.mNumIndices; ++element) {
				uint32_t vertexIndex = face.mIndices[element];
				aiVector3D& position = mesh->mVertices[vertexIndex];
				aiVector3D& normal = mesh->mNormals[vertexIndex];
				aiVector3D& texcoord = mesh->mTextureCoords[0][vertexIndex];
				VertexData vertex;
				vertex.position = { position.x, position.y, position.z, 1.0f };
				vertex.normal = { normal.x, normal.y, normal.z };
				vertex.texcoord = { texcoord.x, texcoord.y };
				// aiProcess_MakeLeftHandedはx*=-1で、右手->左手に変換するので手動で対処
				vertex.position.x *= -1.0f;
				vertex.normal.x *= -1.0f;
				modelData.vertices.push_back(vertex);

				vertexCount++;

			}

		}

		// メッシュ番号
		modelData.meshNumManager.meshNumDataMap_->incrementMeshNum[meshIndex] = vertexCount;
		modelData.meshNumManager.meshNumDataMap_->incrementMeshNumMax++;

	}


	// マテリアル解析
	for (uint32_t materialIndex = 0; materialIndex < scene->mNumMaterials; ++materialIndex) {
		aiMaterial* material = scene->mMaterials[materialIndex];
		if (material->GetTextureCount(aiTextureType_DIFFUSE) != 0) {
			aiString textureFilePath;
			material->GetTexture(aiTextureType_DIFFUSE, 0, &textureFilePath);
			modelData.material.textureFilePaths.push_back(directoryPath + "/" + textureFilePath.C_Str());
		}
	}

	// ノードアニメーション解析
	if (scene->HasAnimations()) {
		// アニメーション配列一個目のみ確認
		aiAnimation** animation = scene->mAnimations;
		for (uint32_t i = 0; i < animation[0]->mNumChannels; ++i) {
			NodeAnimationData nodeAnimationData;
			assert(animation[0]->mChannels[i]);
			nodeAnimationData.Initialize(*animation[0]->mChannels[i]);
			modelData.nodeAnimations.push_back(nodeAnimationData);
		}
		modelData.nodeAnimationNum = static_cast<uint32_t>(modelData.nodeAnimations.size());
	}
	else {
		modelData.nodeAnimationNum = 0;
	}

	return modelData;

}

ModelNode ModelLoader::ReadNode(aiNode* node)
{

	ModelNode result;

	aiMatrix4x4 aiLocalMatrix = node->mTransformation; // nodeのlocalMatrixを取得
	aiLocalMatrix.Transpose(); // 列ベクトル形式を行ベクトル形式に転置
	for (uint32_t y = 0; y < 4; ++y) {
		for (uint32_t x = 0; x < 4; ++x) {
			result.localMatrix.m[y][x] = aiLocalMatrix[y][x];
		}
	}

	result.name = node->mName.C_Str(); // Node名を格納
	result.children.resize(node->mNumChildren); // 子供の数だけ確保
	for (uint32_t childIndex = 0; childIndex < node->mNumChildren; ++childIndex) {
		// 再帰的に読んで階層構造を作る
		result.children[childIndex] = ReadNode(node->mChildren[childIndex]);
	}

	if (node->mMeshes) {
		result.meshNum = *node->mMeshes;
	}
	else {
		result.meshNum = -1;
	}

	return result;

}
