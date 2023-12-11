#include "AABBCollider.h"

AABBCollider::AABBCollider()
{
	// 形状タイプを球に設定
	shapeType_ = SHAPE_AABB;
}

AABBCollider::AABBCollider(const Vector3& inOffset, const Vector3& inRadius)
{
	// 形状タイプを球に設定
	shapeType_ = SHAPE_AABB;

	// オフセットの設定
	offset_ = inOffset;

	// 半径の設定
	AABB::radius = inRadius;
}

void AABBCollider::Update()
{
	// 衝突フラグを初期化
	isHit_ = false;

	// 衝突したコライダーをリセット
	hitCol_ = nullptr;

	reject_ = { 0.0f, 0.0f, 0.0f };

	// オブジェクト3Dが紐づけられていたら
	if (object_)
	{
		AABB::center = object_->GetPosition() + offset_;
	}

	else
	{
		AABB::center = offset_;
	}
}

void AABBCollider::PushBack(const Vector3& reject)
{
	// オブジェクト3Dが紐づけられていたら
	if (object_)
	{
		object_->SetPosition(object_->GetPosition() + reject);
		AABB::center = object_->GetPosition() + offset_;
	}
}