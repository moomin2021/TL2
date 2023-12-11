#pragma once
#include "Mesh.h"
#include "Material.h"

#include <vector>
#include <map>
#include <d3d12.h>
#include <wrl.h>

class Model {
#pragma region エイリアステンプレート
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
#pragma endregion

#pragma region メンバ変数
private:
	// メッシュデータ
	std::vector<Mesh> meshes_;

	// マテリアルデータ
	std::map<std::string, Material> materials_;
#pragma endregion

#pragma region メンバ関数
public:
	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="fileName"> モデルファイル名 </param>
	Model(std::string fileName);

	/// <summary>
	/// 描画処理
	/// </summary>
	void Draw();

private:
	/// <summary>
	/// モデル読み込み
	/// </summary>
	/// <param name="name"> ファイル名 </param>
	void LoadModel(std::string name);

	/// <summary>
	/// マテリアル読み込み
	/// </summary>
	/// <param name="directoryPath"> ファイル名 </param>
	/// <param name="fileName"> ファイル名 </param>
	void LoadMaterial(const std::string& directoryPath, const std::string& fileName);
#pragma endregion

#pragma region ゲッター関数
public:
	/// <summary>
	/// メッシュコンテナを取得
	/// </summary>
	/// <returns> メッシュコンテナ </returns>
	inline const std::vector<Mesh>& GetMesh() { return meshes_; }
#pragma endregion
};
