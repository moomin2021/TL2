#include "RayCollider.h"

RayCollider::RayCollider(Vector3 offset, Vector3 dir)
{
	shapeType_ = SHAPE_RAY;

	offset_ = offset;

	Ray::dir = dir;
}

void RayCollider::Update()
{
	// 衝突フラグを初期化
	isHit_ = false;

	// 衝突したときの情報を初期化
	inter_ = { 0.0f, 0.0f, 0.0f };
	minDistance_ = FLT_MAX;

	// オブジェクト3Dが紐づけられていたら
	if (object_) {
		// ワールド行列からワールド座標を抽出
		const Vector3 objPos = object_->GetPosition();
		Ray::start = objPos + offset_;
	}

	else {
		Ray::start = offset_;
	}
}
