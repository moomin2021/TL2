#include "BoxCollider.h"

BoxCollider::BoxCollider(const Vector2& offset, const Vector2& radius)
{
	// 形状タイプを球に設定
	shapeType_ = SHAPE_BOX;

	// オフセットを設定
	offset_ = offset;

	// 半径を設定
	Box::radius = radius;
}

void BoxCollider::Update()
{
	// 衝突フラグを初期化
	isHit_ = false;

	// 衝突しているコライダーをなくす
	hitCol_ = nullptr;

	// スプライトを紐づけていたら
	if (sprite_) {
		Box::center = offset_ + sprite_->GetPosition();
	}

	else {
		Box::center = offset_;
	}
}
