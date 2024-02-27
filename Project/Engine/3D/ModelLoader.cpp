#include "ModelLoader.h"
#include <cassert>
#include <fstream>
#include <sstream>
#include <format>
#include "../base/BufferResource.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

using namespace DirectX;
using namespace Microsoft::WRL;

Model::MaterialData ModelLoader::LoadMaterialTemplateFile(const std::string& directoryPath, const std::string& filename) {

	//1. 中で必要となる変数
	Model::MaterialData materialData;//構築するデータMaterialData
	std::string line;//ファイルから読んだ1行を格納するもの

	//2. ファイルを開く
	std::ifstream file(directoryPath + "/" + filename);//ファイルを開く
	assert(file.is_open());//開けなっかたら止める

	//3. 実際にファイルを読み、MaterialDataを構築していく
	while (std::getline(file, line)) {
		std::string identifier;
		std::istringstream s(line);
		s >> identifier;

		//identiferに応じた処理
		if (identifier == "map_Kd") {
			std::string texturefFilename;
			s >> texturefFilename;
			//連結してファイルパスにする
			materialData.textureFilePath = directoryPath + "/" + texturefFilename;
		}
	}
	//4. MaterialData を返す
	return materialData;

}

//objファイルを読む
Model::ModelData ModelLoader::LoadObjFile(const std::string& directoryPath, const std::string& filename) {

	////1. 変数宣言
	//Model::ModelData modelData; // 構築するModelData
	//std::vector<Vector4> positions; //位置
	//std::vector<Vector3> normals; // 法線
	//std::vector<Vector2> texcoords;  //テクスチャ座標
	//std::string line; // ファイルから読んだ1行を格納するもの

	////2. ファイルを開く
	//std::ifstream file(directoryPath + "/" + filename); //ファイルを開く
	//assert(file.is_open()); //開けなかったら止める

	////3. ファイルを読んでモデルデータを構築
	//while (std::getline(file, line)) {
	//	std::string identifier;
	//	std::istringstream s(line);
	//	s >> identifier;//先頭の識別子を読む

	//	//identifierに応じた処理
	//	if (identifier == "v") {
	//		Vector4 position;
	//		s >> position.x >> position.y >> position.z;
	//		position.x *= -1.0f;
	//		position.w = 1.0f;
	//		positions.push_back(position);
	//	}
	//	else if (identifier == "vt") {
	//		Vector2 texcoord;
	//		s >> texcoord.x >> texcoord.y;
	//		texcoord.y = 1.0f - texcoord.y;
	//		texcoords.push_back(texcoord);
	//	}
	//	else if (identifier == "vn") {
	//		Vector3 normal;
	//		s >> normal.x >> normal.y >> normal.z;
	//		normal.x *= -1.0f;
	//		normals.push_back(normal);
	//	}
	//	else if (identifier == "f") {
	//		VertexData triangle[3];
	//		//面は三角形限定。その他は未対応
	//		for (int32_t faceVertex = 0; faceVertex < 3; ++faceVertex) {
	//			std::string vertexDefintion;
	//			s >> vertexDefintion;
	//			//頂点の要素へのIndexは「位置/UV/法線」で格納されているので、分かいしてIndexを取得する
	//			std::istringstream v(vertexDefintion);
	//			uint32_t elementIndices[3];
	//			for (int32_t element = 0; element < 3; ++element) {
	//				std::string index;
	//				std::getline(v, index, '/');//区切りでインデックスを読んでいく
	//				elementIndices[element] = std::stoi(index);
	//			}
	//			//要素へのIndexから、実際の要素の値を取得して、頂点を構築する
	//			Vector4 position = positions[elementIndices[0] - 1];
	//			Vector2 texcoord = texcoords[elementIndices[1] - 1];
	//			Vector3 normal = normals[elementIndices[2] - 1];
	//			//VertexData vertex = { position, texcoord, normal };
	//			//modelData.vertices.push_back(vertex);
	//			triangle[faceVertex] = { position, texcoord, normal };
	//		}
	//		//頂点を逆順で登録することで、回り順を逆にする
	//		modelData.vertices.push_back(triangle[2]);
	//		modelData.vertices.push_back(triangle[1]);
	//		modelData.vertices.push_back(triangle[0]);
	//	}
	//	else if (identifier == "mtllib") {
	//		//materialTemplateLibrayファイルの名前を取得する
	//		std::string materialFilename;
	//		s >> materialFilename;
	//		//基本的にobjファイルを同一階層にmtlは存在させるので、ディレクトリ名とファイル名を渡す
	//		modelData.material = LoadMaterialTemplateFile(directoryPath, materialFilename);
	//	}
	//}

	//return modelData;


	// 変数宣言
	Model::ModelData modelData; // 構築するModelData

	// シーン構築
	Assimp::Importer importer;
	std::string filePath = directoryPath + "/" + filename;
	// 読み込み時のオプション
	const aiScene* scene = importer.ReadFile(filePath.c_str(), aiProcess_FlipWindingOrder | aiProcess_FlipUVs);
	assert(scene->HasMeshes()); // メッシュがないのは対応しない

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
				// aiProcess_MakeLeftHandedはz*=-1で、右手->左手に変換するので手動で対処
				vertex.position.x *= -1.0f;
				vertex.normal.x *= -1.0f;
				modelData.vertices.push_back(vertex);
			}

		}

	}


	// マテリアル解析 (最後に見つかったものを使う)
	for (uint32_t materialIndex = 0; materialIndex < scene->mNumMaterials; ++materialIndex) {
		aiMaterial* material = scene->mMaterials[materialIndex];
		if (material->GetTextureCount(aiTextureType_DIFFUSE) != 0) {
			aiString textureFilePath;
			material->GetTexture(aiTextureType_DIFFUSE, 0, &textureFilePath);
			modelData.material.textureFilePath = directoryPath + "/" + textureFilePath.C_Str();
		}
	}

	return modelData;

}