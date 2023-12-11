#pragma once
#include "Vector3.h"

#include <d3d12.h>
#include <wrl.h>
#include <string>

class Material
{
#pragma region エイリアステンプレート
private:
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
#pragma endregion

#pragma region 構造体
private:
	struct MaterialBuffer {
		Vector3 ambient;	// アンビエント係数
		float pad1;		// パディング
		Vector3 diffuse;	// ディフェーズ係数
		float pad2;		// パディング
		Vector3 specular;// スペキュラー係数
		float alpha;	// アルファ
	};
#pragma endregion

#pragma region メンバ変数
public:
	// マテリアルデータ
	std::string name_;	// マテリアル名
	Vector3 ambient_;	// アンビエント影響度
	Vector3 diffuse_;	// ディフューズ影響度
	Vector3 specular_;	// スペキュラー影響度
	float alpha_;		// アルファ値
	uint16_t texHandle_;// テクスチャハンドル
	ComPtr<ID3D12Resource> materialBuff_;	// マテリアルバッファ
#pragma endregion

#pragma region メンバ関数
public:
	// コンストラクタ
	Material();

	// 描画処理
	void Draw();

	/// <summary>
	/// マテリアルバッファ作成
	/// </summary>
	void CreateMaterialBuff();
#pragma endregion
};
