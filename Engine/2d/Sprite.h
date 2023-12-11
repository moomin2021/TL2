#pragma once
#include "Vector2.h"
#include "Vector3.h"
#include "float4.h"
#include "Matrix4.h"
#include "Camera.h"

#include <d3d12.h>
#include <wrl.h>
#include <vector>

class Sprite {
private:
	// エイリアステンプレート
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

#pragma region 構造体
	// 頂点データ
	struct Vertex {
		Vector2 pos;// 座標(XYZ)
		Vector2 uv;	// UV座標(XY)
	};

	// 定数バッファ用データ構造体
	struct ConstBufferData {
		Matrix4 mat;	// 行列
		float4 color;	// 色(RGBA)
	};
#pragma endregion

#pragma region メンバ変数
private:
	// カメラ
	static Camera* sCamera_;

	// スプライトデータ
	Vector2 position_ = { 0.0f, 0.0f };				// 座標(XY)
	float rotation_ = 0.0f;						// 回転(Z)
	Vector2 size_ = { 100.0f, 100.0f };			// サイズ(XY)
	float4 color_ = { 1.0f, 1.0f, 1.0f, 1.0f };	// 色(RGBA)
	Vector2 anchorPoint_ = { 0.0f, 0.0f };				// アンカーポイント座標(XY)
	bool isFlipX_ = false;					// 左右反転
	bool isFlipY_ = false;					// 上下反転

	// ワールド行列
	Matrix4 matWorld_ = {};

	// 定数バッファ
	ComPtr<ID3D12Resource> constBuff_ = nullptr;	// 定数バッファ
	ConstBufferData* constMap_ = nullptr;	// マッピング処理用

	// 頂点データ
	std::vector<Vertex> vertex_ = {};		// 頂点データ
	ComPtr<ID3D12Resource> vertexBuff_ = nullptr;	// 頂点バッファ
	Vertex* vertexMap_ = nullptr;	// マッピング処理用
	D3D12_VERTEX_BUFFER_VIEW vertexView_ = {};		// 頂点バッファビュー

	// インデックスデータ
	std::vector<uint16_t> index_ = {};		// インデックスデータ
	ComPtr<ID3D12Resource> indexBuff_ = nullptr;	// インデックスバッファ
	D3D12_INDEX_BUFFER_VIEW indexView_ = {};		// インデックスバッファビュー
#pragma endregion

#pragma region メンバ関数
public:
	// コンストラクタ
	Sprite();

	// 行列更新処理
	void MatUpdate();

	/// <summary>
	/// 描画処理
	/// </summary>
	/// <param name="textureHandle"> テクスチャハンドル </param>
	void Draw(int textureHandle = 0);
#pragma endregion

#pragma region セッター関数
public:
	// カメラを設定
	static void SetCamera(Camera* camera) { sCamera_ = camera; }

	// 座標(XY)を設定
	inline void SetPosition(const Vector2& position) { position_ = position; }

	// 回転(Z)(度数法)を設定
	inline void SetRotation(float rotation) { rotation_ = rotation; }

	// サイズ(XY)を設定
	inline void SetSize(const Vector2& size) { size_ = size; }

	// 色(RGBA)を設定
	inline void SetColor(const float4& color) { color_ = color; }

	// アンカーポイント(XY)を設定
	inline void SetAnchorPoint(const Vector2& anchorPoint) { anchorPoint_ = anchorPoint; }

	// 左右判定を設定
	inline void SetFlipX(bool flag) { isFlipX_ = flag; }

	// 上下判定を設定
	inline void SetFlipY(bool flag) { isFlipY_ = flag; }
#pragma endregion

#pragma region ゲッター関数
public:
	// 座標(XY)を取得
	inline const Vector2& GetPosition() { return position_; }

	// 回転(Z)(度数法)を取得
	inline float GetRotation() { return rotation_; }

	// サイズ(XY)を取得
	inline const Vector2& GetSize() { return size_; }

	// 色(RGBA)を取得
	inline const float4& GetColor() { return color_; }

	// アンカーポイント(XY)を取得
	inline const Vector2& GetAnchorPoint() { return anchorPoint_; }

	// 左右判定を取得
	inline bool GetFlipX() { return isFlipX_; }

	// 上下判定を取得
	inline bool GetFlipY() { return isFlipY_; }
#pragma endregion
};