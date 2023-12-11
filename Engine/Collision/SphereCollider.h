#pragma once
#include "BaseCollider.h"
#include "CollisionPrimitive.h"

class SphereCollider : public BaseCollider, public Sphere
{
#pragma region メンバ変数
private:
	// オブジェクト中心からのオフセット
	Vector3 offset_ = { 0.0f, 0.0f, 0.0f };

	// 衝突したときの情報
	Vector3 inter_	= { 0.0f, 0.0f, 0.0f };// 交点
	Vector3 reject_	= { 0.0f, 0.0f, 0.0f };// 押し出しベクトル
#pragma endregion

#pragma region メンバ関数
public:
	// コンストラクタ
	SphereCollider(Vector3 offset = { 0.0f, 0.0f, 0.0f }, float radius = 1.0f);

	// 更新処理
	void Update() override;
#pragma endregion

#pragma region セッター関数
public:
	// オフセットを設定
	inline void SetOffset(const Vector3& offset) { offset_ = offset; }

	// 交点を設定
	inline void SetInter(const Vector3& inter) { inter_ = inter; }

	// 半径を設定
	inline void SetRadius(float aRadius) { Sphere::radius = aRadius; }

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
