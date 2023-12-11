#pragma once
#include "BaseCollider2D.h"
#include "CollisionPrimitive.h"

class PointCollider : public BaseCollider2D, public Point
{
#pragma region メンバ変数
private:
	// オフセット
	Vector2 offset_ = { 0.0f, 0.0f };
#pragma endregion

#pragma region メンバ関数
public:
	// コンストラクタ
	PointCollider(const Vector2& offset = { 0.0f, 0.0f });

	// 更新処理
	void Update() override;
#pragma endregion

#pragma region セッター関数
public:
	// オフセットを設定
	inline void SetOffset(const Vector2& offset) { offset_ = offset; }
#pragma endregion

#pragma region ゲッター関数
	// オフセットを取得
	inline const Vector2& GetOffset() { return offset_; }
#pragma endregion
};
