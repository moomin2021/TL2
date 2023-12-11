#pragma once
#include "BaseCollider.h"
#include "CollisionPrimitive.h"

class MeshCollider : public BaseCollider
{
#pragma region メンバ変数
private:
	// 三角形
	std::vector<Triangle> triangles_;

	// 衝突したときの情報
	Vector3 inter_	= { 0.0f, 0.0f, 0.0f };// 交点
	Vector3 reject_ = { 0.0f, 0.0f, 0.0f };// 押し出しベクトル
#pragma endregion

#pragma region メンバ関数
public:
	// コンストラクタ
	MeshCollider(Object3D* object);

	// 更新処理
	void Update() override;

	/// <summary>
	/// 球との当たり判定
	/// </summary>
	/// <param name="sphere"> 球 </param>
	/// <param name="inter"> 交点(出力用) </param>
	/// <returns> 交差しているか否か </returns>
	bool CheckCollisionSphere(const Sphere& sphere, Vector3* inter = nullptr, Vector3* reject = nullptr);

	/// <summary>
	/// レイとの当たり判定
	/// </summary>
	/// <param name="ray"> レイ </param>
	/// <param name="distance"> 距離(出力用) </param>
	/// <param name="inter"> 交点(出力用) </param>
	/// <returns> 交差しているか否か </returns>
	bool CheckCollisionRay(const Ray& ray, float* distance = nullptr, Vector3* inter = nullptr);

private:
	/// <summary>
	/// 三角形の配列を構築する
	/// </summary>
	/// <param name="model"> モデル </param>
	void ConstructTriangles(Model* model);
#pragma endregion

#pragma region セッター関数
public:
	// 交点を設定
	inline void SetInter(const Vector3& inter) { inter_ = inter; }

	// 押し出しベクトルを加算
	inline void AddReject(const Vector3& reject) { reject_ += reject; }
#pragma endregion

#pragma region ゲッター関数
	// 交点を取得
	inline const Vector3& GetInter() { return inter_; }

	// 押し出しベクトルを取得
	inline const Vector3& GetReject() { return reject_; }
#pragma endregion
};
