#pragma once
#include "BaseCollider2D.h"
#include "CollisionPrimitive.h"

class CircleCollider : public BaseCollider2D, public Circle
{
#pragma region メンバ変数
private:
	// オフセット
	Vector2 offset_ = { 0.0f, 0.0f };
#pragma endregion

#pragma region メンバ関数
public:
	// コンストラクタ
	CircleCollider(Vector2 offset = { 0.0f, 0.0f }, float radius = 1.0f);

	// 更新処理
	void Update() override;
#pragma endregion

#pragma region セッター関数
public:
	// オフセットを設定
	inline void SetOffset(const Vector2& offset) { offset_ = offset; }

	// 半径を設定
	inline void SetRadius(float aRadius) { Circle::radius = aRadius; }
#pragma endregion

#pragma region ゲッター関数
	// オフセットを取得
	inline const Vector2& GetOffset() { return offset_; }

	// 半径を取得
	inline float GetRadius() { return Circle::radius; }
#pragma endregion
};
