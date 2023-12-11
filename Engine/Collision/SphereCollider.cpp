#include "SphereCollider.h"

SphereCollider::SphereCollider(Vector3 offset, float radius)
{
	// 形状タイプを球に設定
	shapeType_ = SHAPE_SPHERE;

	// オフセットを設定
	offset_ = offset;

	// 半径を設定
	Sphere::radius = radius;
}

void SphereCollider::Update()
{
	// 衝突フラグを初期化
	isHit_ = false;

	// 衝突したときの情報を初期化
	inter_ = { 0.0f, 0.0f, 0.0f };
	reject_ = { 0.0f, 0.0f, 0.0f };

	// オブジェクト3Dが紐づけられていたら
	if (object_) {
		Sphere::center = object_->GetPosition() + offset_;
	}

	else {
		Sphere::center = offset_;
	}
}
