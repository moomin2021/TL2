#include "CollisionManager2D.h"
#include "Collision.h"

CollisionManager2D::~CollisionManager2D()
{
	// コライダー削除
	colliders_.clear();
}

void CollisionManager2D::CheckAllCollision()
{
	// コライダー更新
	for (auto& it : colliders_) {
		it->Update();
	}

	// 総当たりするために用意
	std::forward_list<BaseCollider2D*>::iterator itA = colliders_.begin();;
	std::forward_list<BaseCollider2D*>::iterator itB;

	// 総当たりチェック
	for (; itA != colliders_.end(); ++itA) {
		itB = itA;
		++itB;

		if ((*itA)->GetIsOnCol() == false) continue;

		for (; itB != colliders_.end(); ++itB) {
			// 属性が合わなければ除外
			if (!((*itA)->attribute_ & (*itB)->attribute_)) continue;

			if ((*itB)->GetIsOnCol() == false) continue;

			// ともに矩形
			if ((*itA)->GetShapeType() == SHAPE_BOX && (*itB)->GetShapeType() == SHAPE_BOX) {
				// 衝突判定の引数のために球に変換
				Box* boxA = dynamic_cast<Box*>(*itA);
				Box* boxB = dynamic_cast<Box*>(*itB);

				// 衝突判定
				if (Collision::CheckBox2Box(*boxA, *boxB)) {
					(*itA)->SetIsHit(true);
					(*itA)->SetHitCollider(*itB);

					(*itB)->SetIsHit(true);
					(*itB)->SetHitCollider(*itA);
				}
			}

			// ともに円
			else if ((*itA)->GetShapeType() == SHAPE_CIRCLE && (*itB)->GetShapeType() == SHAPE_CIRCLE) {
				// 衝突判定の引数のために球に変換
				Circle* cirA = dynamic_cast<Circle*>(*itA);
				Circle* cirB = dynamic_cast<Circle*>(*itB);

				// 衝突判定
				if (Collision::CheckCircle2Circle(*cirA, *cirB)) {
					(*itA)->SetIsHit(true);
					(*itA)->SetHitCollider(*itB);

					(*itB)->SetIsHit(true);
					(*itB)->SetHitCollider(*itA);
				}
			}

			// 点と矩形
			else if ((*itA)->GetShapeType() == SHAPE_POINT && (*itB)->GetShapeType() == SHAPE_BOX) {
				// 衝突判定の引数のために球に変換
				Point* point = dynamic_cast<Point*>(*itA);
				Box* box = dynamic_cast<Box*>(*itB);

				// 衝突判定
				if (Collision::CheckPoint2Box(*point, *box)) {
					(*itA)->SetIsHit(true);
					(*itA)->SetHitCollider(*itB);

					(*itB)->SetIsHit(true);
					(*itB)->SetHitCollider(*itA);
				}
			}

			// 矩形と点
			else if ((*itA)->GetShapeType() == SHAPE_BOX && (*itB)->GetShapeType() == SHAPE_POINT) {
				// 衝突判定の引数のために球に変換
				Box* box = dynamic_cast<Box*>(*itA);
				Point* point = dynamic_cast<Point*>(*itB);

				// 衝突判定
				if (Collision::CheckPoint2Box(*point, *box)) {
					(*itA)->SetIsHit(true);
					(*itA)->SetHitCollider(*itB);

					(*itB)->SetIsHit(true);
					(*itB)->SetHitCollider(*itA);
				}
			}

			// 点と円
			else if ((*itA)->GetShapeType() == SHAPE_POINT && (*itB)->GetShapeType() == SHAPE_CIRCLE) {
				// 衝突判定の引数のために球に変換
				Point* point = dynamic_cast<Point*>(*itA);
				Circle* cir = dynamic_cast<Circle*>(*itB);

				// 衝突判定
				if (Collision::CheckPoint2Circle(*point, *cir)) {
					(*itA)->SetIsHit(true);
					(*itA)->SetHitCollider(*itB);

					(*itB)->SetIsHit(true);
					(*itB)->SetHitCollider(*itA);
				}
			}

			// 円と点
			else if ((*itA)->GetShapeType() == SHAPE_CIRCLE && (*itB)->GetShapeType() == SHAPE_POINT) {
				// 衝突判定の引数のために球に変換
				Circle* cir = dynamic_cast<Circle*>(*itA);
				Point* point = dynamic_cast<Point*>(*itB);

				// 衝突判定
				if (Collision::CheckPoint2Circle(*point, *cir)) {
					(*itA)->SetIsHit(true);
					(*itA)->SetHitCollider(*itB);

					(*itB)->SetIsHit(true);
					(*itB)->SetHitCollider(*itA);
				}
			}

			// 矩形と円
			else if ((*itA)->GetShapeType() == SHAPE_BOX && (*itB)->GetShapeType() == SHAPE_CIRCLE) {
				// 衝突判定の引数のために球に変換
				Box* box = dynamic_cast<Box*>(*itA);
				Circle* cir = dynamic_cast<Circle*>(*itB);

				// 衝突判定
				if (Collision::CheckCircle2Box(*cir, *box)) {
					(*itA)->SetIsHit(true);
					(*itA)->SetHitCollider(*itB);

					(*itB)->SetIsHit(true);
					(*itB)->SetHitCollider(*itA);
				}
			}

			// 円と矩形
			else if ((*itA)->GetShapeType() == SHAPE_CIRCLE && (*itB)->GetShapeType() == SHAPE_BOX) {
				// 衝突判定の引数のために球に変換
				Circle* cir = dynamic_cast<Circle*>(*itA);
				Box* box = dynamic_cast<Box*>(*itB);

				// 衝突判定
				if (Collision::CheckCircle2Box(*cir, *box)) {
					(*itA)->SetIsHit(true);
					(*itA)->SetHitCollider(*itB);

					(*itB)->SetIsHit(true);
					(*itB)->SetHitCollider(*itA);
				}
				}
		}
	}
}

CollisionManager2D* CollisionManager2D::GetInstance()
{
	// インスタンス生成
	static CollisionManager2D inst;

	// インスタンスを返す
	return &inst;
}
