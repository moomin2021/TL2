#include "PointCollider.h"

PointCollider::PointCollider(const Vector2& offset)
{
	// 形状タイプを球に設定
	shapeType_ = SHAPE_POINT;

	// オフセットを設定
	offset_ = offset;
}

void PointCollider::Update()
{
	// 衝突フラグを初期化
	isHit_ = false;

	// 衝突しているコライダーをなくす
	hitCol_ = nullptr;
	
	// スプタイトが紐づけられていたら
	if (sprite_) {
		Point::pos = offset_ + sprite_->GetPosition();
	}

	else {
		Point::pos = offset_;
	}
}
