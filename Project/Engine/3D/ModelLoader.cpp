#include "ModelLoader.h"
#include <cassert>
#include <fstream>
#include <sstream>
#include <format>
#include "../base/BufferResource.h"

using namespace DirectX;
using namespace Microsoft::WRL;

std::vector<std::pair<std::string, Matrix4x4>> ModelLoader::boneOffsetMatrixes_;

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

	uint32_t vertexCount = 0;
	modelData.meshNumManager.Initialize();

	// メッシュ多いよ
	assert(scene->mNumMeshes <= 4);

	// メッシュ解析
	for (uint32_t meshIndex = 0; meshIndex < scene->mNumMeshes; ++meshIndex) {
		aiMesh* mesh = scene->mMeshes[meshIndex];
		assert(mesh->HasNormals()); // 法線がないMeshは今回は非対応
		assert(mesh->HasTextureCoords(0)); // TexcoordがないMeshは今回は非対応

		// ボーン解析
		std::vector<std::vector<std::pair<uint32_t, float>>> boneDatas;
		boneOffsetMatrixes_.clear();
		if (mesh->HasBones()) {
			aiBone** bone = mesh->mBones;
			boneDatas.resize(mesh->mNumBones);
			for (uint32_t i = 0; i < mesh->mNumBones; ++i) {

				aiVertexWeight* weights = bone[i]->mWeights;
				for (uint32_t j = 0; j < bone[i]->mNumWeights; ++j) {
					
					std::pair<uint32_t, float> boneData;
					boneData.first = weights[j].mVertexId;
					boneData.second = weights[j].mWeight;
					boneDatas[i].push_back(boneData);

				}
				aiString name = bone[i]->mName;
				aiMatrix4x4 aiMatrix = bone[i]->mOffsetMatrix;
				aiMatrix.Transpose();

				std::pair<std::string, Matrix4x4> boneOffsetMatrix;
				boneOffsetMatrix.first = name.C_Str();
				for (uint32_t y = 0; y < 4; ++y) {
					for (uint32_t x = 0; x < 4; ++x) {
						boneOffsetMatrix.second.m[y][x] = aiMatrix[y][x];
					}
				}
				boneOffsetMatrixes_.push_back(boneOffsetMatrix);

			}
		}

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

				// ボーン情報取得
				if (mesh->HasBones()) {
					vertex.wegiht0 = 0.0f;
					vertex.wegiht1 = 0.0f;
					vertex.wegiht2 = 0.0f;
					vertex.matrixIndex0 = 1000;
					vertex.matrixIndex1 = 1000;
					vertex.matrixIndex2 = 1000;
					vertex.matrixIndex3 = 1000;
					// 重みデータ
					for (uint32_t i = 0; i < boneDatas.size(); ++i) {
						for (uint32_t j = 0; j < boneDatas[i].size(); ++j) {
							if (boneDatas[i][j].first == vertexIndex) {
								// 空いている場所
								if (vertex.matrixIndex0 == 1000) {
									vertex.matrixIndex0 = i + 2;
									vertex.wegiht0 = boneDatas[i][j].second;
									break;
								}
								else if (vertex.matrixIndex1 == 1000) {
									vertex.matrixIndex1 = i + 2;
									vertex.wegiht1 = boneDatas[i][j].second;
									break;
								}
								else if (vertex.matrixIndex2 == 1000) {
									vertex.matrixIndex2 = i + 2;
									vertex.wegiht2 = boneDatas[i][j].second;
									break;
								}
								else if (vertex.matrixIndex3 == 1000) {
									vertex.matrixIndex3 = i + 2;
									break;
								}
							}
						}
					}
				}
				else {
					vertex.wegiht0 = 1.0f;
					vertex.wegiht1 = 0.0f;
					vertex.wegiht2 = 0.0f;
					vertex.matrixIndex0 = meshIndex + 1; // 親ノード分＋1
					vertex.matrixIndex1 = 1000;
					vertex.matrixIndex2 = 1000;
					vertex.matrixIndex3 = 1000;
				}

				modelData.vertices.push_back(vertex);

				vertexCount++;

			}

		}

		// メッシュ番号
		modelData.meshNumManager.meshNumDataMap_->incrementMeshNum[meshIndex] = vertexCount;
		modelData.meshNumManager.meshNumDataMap_->incrementMeshNumMax++;

	}

	// ノード解析
	modelData.rootNode = ReadNode(scene->mRootNode);

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

	result.offsetMatrix = Matrix4x4::MakeIdentity4x4();
	if (!boneOffsetMatrixes_.empty()) {
		for (uint32_t i = 0; i < boneOffsetMatrixes_.size(); ++i) {
			if (boneOffsetMatrixes_[i].first == node->mName.C_Str()) {
				result.offsetMatrix = boneOffsetMatrixes_[i].second;
			}
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
		result.meshNum = 0;
	}

	return result;

}
