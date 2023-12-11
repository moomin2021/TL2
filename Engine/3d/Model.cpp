#include "Model.h"
#include "Texture.h"
#include "DX12Cmd.h"

#include <fstream>
#include <sstream>

using namespace std;

Model::Model(string fileName) :
#pragma region 初期化リスト
	// メッシュデータ
	meshes_{},

	// マテリアルデータ
	materials_{}
#pragma endregion
{
	// モデル読み込み
	LoadModel(fileName);
}

void Model::Draw() {
	for (auto& i : meshes_) {
		materials_[i.GetMaterialName()].Draw();
		i.Draw();
	}
}

void Model::LoadModel(string name)
{
	// ファイルストリーム
	std::ifstream file;

	// .objファイルを開く
	const string modelName = name;
	const string fileName = modelName + ".obj";
	const string directoryPath = "Model/" + modelName + "/";
	file.open("Resources/" + directoryPath + fileName);

	// ファイルオープン失敗をチェック
	assert(!file.fail());

	vector<Vector3> positions;	// 頂点座標
	vector<Vector3> normals;		// 法線ベクトル
	vector<Vector2> texcoords;	// テクスチャUV

	// メッシュを生成したらカウント
	uint16_t meshCount = 0;
	uint16_t loopCount = 0;

	// 1行ずつ読み込む
	string line;
	while (getline(file, line)) {

		// 1行分の文字列をストリームに変換して解析しやすくする
		std::istringstream line_stream(line);

		// 半角スペース区切りで行の先頭文字列を取得
		string key;
		getline(line_stream, key, ' ');

		// 先頭文字が[o]なら頂点データをリセット
		if (key == "o") {
			// メッシュ生成
			meshes_.emplace_back();
			meshCount++;
			loopCount = 0;
		}

		// 先頭文字列が[v]なら頂点座標
		else if (key == "v") {
			// X, Y, Z座標読み込み
			Vector3 position{};
			line_stream >> position.x;
			line_stream >> position.y;
			line_stream >> position.z;

			// 座標データに追加
			//position.x = -position.x;
			positions.emplace_back(position);
		}

		// 先頭文字列が[vt]ならテクスチャ
		else if (key == "vt") {
			// U, V成分読み込み
			Vector2 texcoord{};
			line_stream >> texcoord.x;
			line_stream >> texcoord.y;

			// V方向反転
			texcoord.y = 1.0f - texcoord.y;

			// テクスチャ座標データ追加
			texcoords.emplace_back(texcoord);
		}

		// 先頭文字列が[vn]なら法線ベクトル
		else if (key == "vn") {
			// X, Y, Z成分読み込み
			Vector3 normal{};
			line_stream >> normal.x;
			line_stream >> normal.y;
			line_stream >> normal.z;

			// 法線ベクトルデータに追加
			normals.emplace_back(normal);
		}

		// 先頭文字列が[f]ならポリゴン(三角形)
		else if (key == "f") {
			// 半角スペース区切りで行の続きを読み込む
			string index_string;
			while (getline(line_stream, index_string, ' ')) {

				// 頂点インデックス1個分の文字列をストリームに変換して解析しやすくする
				std::istringstream index_stream(index_string);
				unsigned short indexPosition, indexNormal, indexTexcoord;
				index_stream >> indexPosition;
				index_stream.seekg(1, ios_base::cur);// -> スラッシュを飛ばす
				index_stream >> indexTexcoord;
				index_stream.seekg(1, ios_base::cur);// -> スラッシュを飛ばす
				index_stream >> indexNormal;

				// 頂点データの追加
				Mesh::Vertex vertex{};
				vertex.pos = positions[indexPosition - 1];
				vertex.normal = normals[indexNormal - 1];
				vertex.uv = texcoords[indexTexcoord - 1];
				meshes_[meshCount - 1].AddVertex(vertex);

				// 頂点インデックスに追加
				meshes_[meshCount - 1].AddIndex(loopCount);
				loopCount++;
			}
		}

		// 先頭文字列が[usemtl]ならマテリアル
		else if (key == "usemtl") {
			std::string materialName;
			line_stream >> materialName;

			// マテリアルの名前をメッシュに設定
			meshes_[meshCount - 1].SetMaterialName(materialName);
		}

		// 先頭文字列が[mtllib]ならマテリアル
		else if (key == "mtllib") {
			string filename;
			line_stream >> filename;

			// マテリアル読み込み
			LoadMaterial(directoryPath, filename);
		}
	}

	// 生成したすべてのメッシュの頂点バッファとインデックスデータの生成
	for (size_t i = 0; i < meshes_.size(); i++) {
		meshes_[i].CreateVertexBuff();
		meshes_[i].CreateIndexBuff();
	}

	// ファイルを閉じる
	file.close();
}

void Model::LoadMaterial(const string& directoryPath, const string& fileName) {
	// ファイルストリーム
	ifstream file;

	// マテリアルファイルを開く
	file.open("Resources/" + directoryPath + fileName);

	// ファイルオープン失敗をチェック
	if (file.fail()) assert(0);

	std::string materialName;

	// 1行ずつ読み込む
	string line;
	while (getline(file, line)) {
		// 1行分の文字列をストリームに変換
		istringstream line_stream(line);

		// 半角スペース区切りで行の先頭文字列を取得
		string key;
		getline(line_stream, key, ' ');

		// 先頭のタブ文字は無視する
		if (key[0] == '\t') {
			key.erase(key.begin());
		}

		// 先頭文字列が[newmtl]ならマテリアル名
		if (key == "newmtl") {
			// マテリアル名読み込み
			line_stream >> materialName;

			// マテリアル追加
			materials_.emplace(materialName, Material());
		}

		// 先頭文字列が[Ka]ならアンビエント色
		if (key == "Ka") {
			line_stream >> materials_[materialName].ambient_.x;
			line_stream >> materials_[materialName].ambient_.y;
			line_stream >> materials_[materialName].ambient_.z;
		}

		// 先頭文字列が[Kd]ならディフェーズ職
		if (key == "Kd") {
			line_stream >> materials_[materialName].diffuse_.x;
			line_stream >> materials_[materialName].diffuse_.y;
			line_stream >> materials_[materialName].diffuse_.z;
		}

		// 先頭文字列が[Ks]ならスペキュラー色
		if (key == "Ks") {
			line_stream >> materials_[materialName].specular_.x;
			line_stream >> materials_[materialName].specular_.y;
			line_stream >> materials_[materialName].specular_.z;
		}

		// 先頭文字列が[map_Kd]ならテクスチャファイル名
		if (key == "map_Kd") {
			// テクスチャのファイル名読み込み
			std::string texName;
			line_stream >> texName;

			// テクスチャ読み込み
			materials_[materialName].texHandle_ = LoadTexture(directoryPath + texName);
		}
	}

	// 全てのマテリアルデータのバッファを生成
	for (auto it = materials_.begin(); it != materials_.end(); ++it) {
		materials_[it->first].CreateMaterialBuff();
	}

	// ファイルを閉じる
	file.close();
}
