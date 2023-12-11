#include "CircleCollider.h"

CircleCollider::CircleCollider(Vector2 offset, float radius)
{
	// 形状タイプを球に設定
	shapeType_ = SHAPE_CIRCLE;

	// オフセットを設定
	offset_ = offset;

	// 半径を設定
	Circle::radius = radius;
}

void CircleCollider::Update()
{
	// 衝突フラグを初期化
	isHit_ = false;

	// 衝突しているコライダーをなくす
	hitCol_ = nullptr;

	// スプライトが紐づけられていたら
	if (sprite_) {
		Circle::center = offset_ + sprite_->GetPosition();
	}

	else {
		Circle::center = offset_;
	}
}
