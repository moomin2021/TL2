#pragma once
#include "Camera.h"
#include "Vector3.h"
#include "Matrix4.h"
#include "float4.h"

#include <d3d12.h>
#include <deque>
#include <wrl.h>

class Line3D
{
private:
	// エイリアステンプレート
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

#pragma region 構造体
	// 頂点データ
	struct Vertex {
		Vector3 pos;
	};

	// 定数バッファ
	struct ConstBufferData {
		Matrix4 mat;
		float4 color;
	};
#pragma endregion

#pragma region メンバ変数
private:
	static Camera* sCamera_;

private:
	// 最大頂点数
	uint16_t maxVertex_ = 0;

	// ラインコンテナ
	std::deque<Vector3> lines_ = {};

	// 定数バッファ
	ComPtr<ID3D12Resource> constBuff_ = nullptr;
	ConstBufferData* constMap_ = nullptr;

	// 頂点データ
	D3D12_VERTEX_BUFFER_VIEW vertexView_ = {};
	ComPtr<ID3D12Resource> vertexBuff_ = nullptr;

	// インデックスデータ
	D3D12_INDEX_BUFFER_VIEW indexView_ = {};
	ComPtr<ID3D12Resource> indexBuff_ = nullptr;

	// 色
	float4 color_ = { 1.0f, 1.0f, 1.0f, 1.0f };
#pragma endregion

#pragma region メンバ関数
public:
	// 初期化処理
	void Initialize(uint16_t inMaxVertex);

	// 行列更新処理
	void MatUpdate();

	// 描画処理
	void Draw();

	// 点の追加
	void AddPoint(const Vector3& inPoint0, const Vector3& inPoint1);

	// 点の全削除
	void ClearPoint() { lines_.clear(); }

	Line3D() {}
	~Line3D() { lines_.clear(); }

private:
	// 定数バッファ生成
	void CreateConstBuffer();

	// 頂点バッファ生成
	void CreateVertexBuffer();

	// インデックスバッファ生成
	void CreateIndexBuffer();
#pragma endregion

#pragma region セッター関数
public:
	static void SetCamera(Camera* inCamera) { sCamera_ = inCamera; }
	void SetColor(const float4& inColor) { color_ = inColor; }
#pragma endregion
};