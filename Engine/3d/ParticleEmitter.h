#pragma once
#include "Camera.h"
#include "Vector3.h"
#include "Matrix4.h"

#include <d3d12.h>
#include <wrl.h>
#include <deque>

enum BILLBOARD {
	NONE,
	ALL,
	X,
	Y,
	Z
};

class ParticleEmitter
{
private:
	// エイリアステンプレート
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

#pragma region 構造体
	// 頂点データ
	struct Vertex {
		Vector3 pos;
		float scale;
	};

	// 定数バッファ用
	struct ConstBufferData {
		Matrix4 mat;
	};

	// パーティクル1粒
	struct Particle {
		Vector3 position = {};	// 座標
		Vector3 velocity = {};	// 速度
		Vector3 accel = {};		// 加速度
		uint16_t frame = 0;		// 現在フレーム
		uint16_t num_frame = 0;	// 終了フレーム
		float scale = 1.0f;		// スケール
		float startScale = 1.0f;// 初期値
		float endScale = 0.0f;	// 最終値
	};
#pragma endregion

#pragma region メンバ変数
	// パーティクルコンテナ
	std::deque<Particle> particles_;

	// 最大頂点数
	const uint16_t MAX_VERTEX = 1024;

	// 発生位置
	Vector3 position_ = { 0.0f, 0.0f, 0.0f };

	// 定数バッファ
	ComPtr<ID3D12Resource> constBuff_ = nullptr;
	ConstBufferData* constMap_ = nullptr;

	// 頂点データ
	D3D12_VERTEX_BUFFER_VIEW vertexView_{};	// 頂点バッファービュー
	ComPtr<ID3D12Resource> vertexBuff_ = nullptr;		// 頂点バッファ

	// インデックスデータ
	D3D12_INDEX_BUFFER_VIEW indexView_{};	// インデックスバッファビュー
	ComPtr<ID3D12Resource> indexBuff_ = nullptr;	// インデックスバッファ

	// カメラ
	static Camera* sCamera_;
#pragma endregion

#pragma region メンバ関数
public:
	// コンストラクタ
	ParticleEmitter();

	// デストラクタ
	~ParticleEmitter();

	// 更新処理
	void Update(BILLBOARD billBoard = BILLBOARD::NONE);

	// 描画処理
	void Draw(uint16_t handle = 0);

	/// <summary>
	/// パーティクル追加
	/// </summary>
	/// <param name="life"> 生存時間 </param>
	/// <param name="pos"> 初期座標 </param>
	/// <param name="velocity"> 速度 </param>
	/// <param name="accel"> 加速度 </param>
	void Add(uint16_t life, Vector3 pos, Vector3 velocity, Vector3 accel, float startScale, float endScale);

private:
	// 定数バッファ生成
	void CreateConstBuff();

	// 頂点バッファ生成
	void CreateVertexBuff();

	// インデックスバッファ生成
	void CreateIndexBuff();
#pragma endregion

#pragma region セッター関数
public:
	// 生成位置を設定
	void SetSpawnPos(const Vector3& pos) { position_ = pos; }

	// カメラを設定
	static void SetCamera(Camera* camera) { sCamera_ = camera; }
#pragma endregion
};
