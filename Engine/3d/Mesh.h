#pragma once
#include "Vector2.h"
#include "Vector3.h"

#include <d3d12.h>
#include <wrl.h>
#include <string>
#include <vector>

class Mesh
{
#pragma region エイリアステンプレート
private:
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
#pragma endregion

#pragma region 構造体
public:
	// 頂点データ
	struct Vertex {
		Vector3 pos;		// 座標(XYZ)
		Vector3 normal;	// 法線(XYZ)
		Vector2 uv;		// UV座標(XY)
	};
#pragma endregion

#pragma region メンバ変数
private:
	// 頂点データ
	std::vector<Vertex> vertices_;			// 頂点データ
	D3D12_VERTEX_BUFFER_VIEW vertexView_;	// 頂点バッファービュー
	ComPtr<ID3D12Resource> vertexBuff_;		// 頂点バッファ

	// インデックスデータ
	std::vector<uint16_t> indexes_;		// インデックスデータ
	D3D12_INDEX_BUFFER_VIEW indexView_;	// インデックスバッファビュー
	ComPtr<ID3D12Resource> indexBuff_;	// インデックスバッファ

	// 適用されるマテリアルの名前
	std::string materialName_;
#pragma endregion

#pragma region メンバ関数
public:
	/// <summary>
	/// コンストラクタ
	/// </summary>
	Mesh();

	/// <summary>
	/// 描画処理
	/// </summary>
	void Draw();

	/// <summary>
	/// 頂点データを追加
	/// </summary>
	/// <param name="vertex"> 頂点データ </param>
	void AddVertex(const Vertex& vertex) { vertices_.emplace_back(vertex); }

	/// <summary>
	/// インデックスデータを追加
	/// </summary>
	/// <param name="index"> インデックスデータ </param>
	void AddIndex(uint16_t index) { indexes_.emplace_back(index); }

	/// <summary>
	/// 頂点バッファ作成
	/// </summary>
	void CreateVertexBuff();

	/// <summary>
	/// インデックスバッファ作成
	/// </summary>
	void CreateIndexBuff();
#pragma endregion

#pragma region セッター関数
public:
	/// <summary>
	/// マテリアルの名前を設定
	/// </summary>
	/// <param name="name"> マテリアルの名前 </param>
	void SetMaterialName(const std::string& name) { materialName_ = name; }
#pragma endregion

#pragma region ゲッター関数
	/// <summary>
	/// マテリアルの名前を取得
	/// </summary>
	/// <returns> マテリアルの名前 </returns>
	const std::string& GetMaterialName() { return materialName_; }

	/// <summary>
	/// 頂点データコンテナを取得
	/// </summary>
	/// <returns> 頂点データ </returns>
	inline const std::vector<Vertex>& GetVertex() { return vertices_; }

	/// <summary>
	/// インデックスデータコンテナを取得
	/// </summary>
	/// <returns> インデックスデータコンテナ </returns>
	inline const std::vector<uint16_t>& GetIndex() { return indexes_; }
#pragma endregion
};
