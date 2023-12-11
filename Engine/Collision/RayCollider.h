#pragma once
#include "BaseCollider.h"
#include "CollisionPrimitive.h"

class RayCollider : public BaseCollider, public Ray
{
#pragma region メンバ変数
private:
	// オブジェクト中心からのオフセット
	Vector3 offset_ = { 0.0f, 0.0f, 0.0f };

	// 衝突したときの情報
	Vector3 inter_ = { 0.0f, 0.0f, 0.0f };// 交点
	float minDistance_ = FLT_MAX;// 交点までの距離
#pragma endregion

#pragma region メンバ関数
public:
	// コンストラクタ
	RayCollider(Vector3 offset = { 0.0f, 0.0f, 0.0f }, Vector3 dir = { 0.0f, 0.0f, 0.0f });

	// 更新処理
	void Update() override;
#pragma endregion

#pragma region セッター関数
public:
	// オフセットを設定
	inline void SetOffSet(const Vector3& offset) { offset_ = offset; }

	// 方向を設定
	inline void SetDir(const Vector3& aDir) { Ray::dir = aDir; }

	// 交点を設定
	inline void SetInter(const Vector3& inter) { inter_ = inter; }

	// 交点までの距離を設定
	inline void SetDistance(float distance) { minDistance_ = distance; }
#pragma endregion

#pragma region ゲッター関数
	// 交点を取得
	inline const Vector3& GetInter() { return inter_; }

	// 交点までの距離を取得
	inline float GetDistance() { return minDistance_; }
#pragma endregion
};
