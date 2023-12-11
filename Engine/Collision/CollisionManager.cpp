#include "CollisionManager.h"
#include "Collision.h"
#include "SphereCollider.h"
#include "RayCollider.h"
#include "MeshCollider.h"
#include "CollisionAttribute.h"

CollisionManager::~CollisionManager()
{
	// コライダー削除
	colliders_.clear();
}

void CollisionManager::CheckAllCollision()
{
	// コライダー更新
	for (auto& it : colliders_) {
		it->Update();
	}

	for (auto& it : blockCollider_) it->Update();

	// 総当たりするために用意
	std::forward_list<BaseCollider*>::iterator itA = colliders_.begin();
	std::forward_list<BaseCollider*>::iterator itB;

	uint32_t count = 0;

	// 総当たりチェック
	for (; itA != colliders_.end(); ++itA) {
		itB = itA;
		++itB;

		// ブロックと属性が合ったら
		if ((*itA)->attribute_ & COL_STAGE_OBJ)
		{
			if ((*itA)->shapeType_ == SHAPE_RAY)
			{
				// 衝突フラグ
				bool result = false;

				// 最も近いコライダーの情報
				float distance = FLT_MAX;				// 最も最小の距離
				Vector3 inter = { 0.0f, 0.0f, 0.0f };	// 最も近い距離にいるコライダーとの交点
				AABBCollider* it_hit = nullptr;			// 最も近いコライダー

				// 衝突判定の引数のためにレイに変換
				Ray* ray = dynamic_cast<Ray*>(*itA);

				for (auto& it : blockCollider_)
				{
					count++;

					// 判定したときのデータ
					float tempDistance = FLT_MAX;
					Vector3 tempInter = { 0.0f, 0.0f, 0.0f };

					// 衝突していなかったら除外
					if (!Collision::CheckRay2AABB(*ray, *it, &tempDistance, &tempInter)) continue;

					// 距離が最小でなければ除外
					if (tempDistance >= distance) continue;

					// 最も近いコライダーなので情報を保存
					result = true;
					distance = tempDistance;
					inter = tempInter;
					it_hit = it;
				}

				if (result)
				{
					// レイ
					RayCollider* rayCol = dynamic_cast<RayCollider*>(*itA);
					rayCol->SetIsHit(true);
					rayCol->SetInter(inter);
					rayCol->SetDistance(distance);
					rayCol->SetHitCollider(it_hit);

					AABBCollider* aabbCol = dynamic_cast<AABBCollider*>(it_hit);
					aabbCol->SetIsHit(true);
					aabbCol->SetHitCollider(*itA);
				}
			}

			else if ((*itA)->shapeType_ == SHAPE_AABB)
			{
				for (auto& it : blockCollider_)
				{
					count++;

					AABBCollider* aabb0 = dynamic_cast<AABBCollider*>(*itA);
					AABBCollider* aabb1 = dynamic_cast<AABBCollider*>(it);
					Vector3 reject = { 0.0f, 0.0f, 0.0f };

					if (!Collision::CheckAABB2AABB(*aabb0, *aabb1, &reject)) continue;

					aabb0->SetIsHit(true);
					aabb1->SetIsHit(true);
					aabb0->PushBack(reject);
					aabb0->SetHitCollider(it);
					aabb1->SetHitCollider(*itA);
				}
			}
		}

		// レイだったら
		if ((*itA)->GetShapeType() == SHAPE_RAY) {
			// 衝突フラグ
			bool result = false;

			// 最も近いコライダーの情報
			float distance = FLT_MAX;				// 最も最小の距離
			Vector3 inter = { 0.0f, 0.0f, 0.0f };	// 最も近い距離にいるコライダーとの交点
			BaseCollider* it_hit = nullptr;			// 最も近いコライダー

			// 走査用のイテレーター
			std::forward_list<BaseCollider*>::iterator it = colliders_.begin();

			// 衝突判定の引数のためにレイに変換
			Ray* ray = dynamic_cast<Ray*>(*itA);

			// 総当たりチェック
			for (; it != colliders_.end(); ++it) {

				count++;

				// 属性が合わなければスキップ
				if (!((*itA)->attribute_ & (*it)->attribute_)) continue;

				// 球の場合
				if ((*it)->GetShapeType() == SHAPE_SPHERE) {
					// 衝突判定の引数のために球に変換
					Sphere* sphere = dynamic_cast<Sphere*>(*it);

					// 判定したときのデータ
					float tempDistance = FLT_MAX;
					Vector3 tempInter = { 0.0f, 0.0f, 0.0f };

					// 衝突していなかったら除外
					if (!Collision::CheckRay2Sphere(*ray, *sphere, &tempDistance, &tempInter)) continue;

					// 距離が最小でなければ除外
					if (tempDistance >= distance) continue;

					// 最も近いコライダーなので情報を保存
					result = true;
					distance = tempDistance;
					inter = tempInter;
					it_hit = *it;
				}

				else if ((*it)->GetShapeType() == SHAPE_MESH) {
					// 衝突判定の引数のためにメッシュコライダーに変換
					MeshCollider* meshCol = dynamic_cast<MeshCollider*>(*it);

					// 判定したときのデータ
					float tempDistance = FLT_MAX;
					Vector3 tempInter = { 0.0f, 0.0f, 0.0f };

					// 衝突していなかったら除外
					if (!meshCol->CheckCollisionRay(*ray, &tempDistance, &tempInter)) continue;

					// 距離が最小でなければ除外
					if (tempDistance >= distance) continue;

					// 最も近いコライダーなので情報を保存
					result = true;
					distance = tempDistance;
					inter = tempInter;
					it_hit = *it;
				}

				else if ((*it)->shapeType_ == SHAPE_AABB)
				{
					// 衝突判定を行うためにAABBコライダーに変換
					AABBCollider* aabbCol = dynamic_cast<AABBCollider*>(*it);

					// 判定したときのデータ
					float tempDistance = FLT_MAX;
					Vector3 tempInter = { 0.0f, 0.0f, 0.0f };

					// 衝突していなかったら除外
					if (!Collision::CheckRay2AABB(*ray, *aabbCol, &tempDistance, &tempInter)) continue;

					// 距離が最小でなければ除外
					if (tempDistance >= distance) continue;

					// 最も近いコライダーなので情報を保存
					result = true;
					distance = tempDistance;
					inter = tempInter;
					it_hit = *it;
				}
			}

			// 衝突していたら
			if (result) {
				// レイ
				RayCollider* rayCol = dynamic_cast<RayCollider*>(*itA);
				rayCol->SetIsHit(true);
				rayCol->SetInter(inter);
				rayCol->SetDistance(distance);
				rayCol->SetHitCollider(it_hit);

				// 球
				if (it_hit->GetShapeType() == SHAPE_SPHERE) {
					SphereCollider* sphereCol = dynamic_cast<SphereCollider*>(it_hit);
					sphereCol->SetIsHit(true);
					sphereCol->SetInter(inter);
					sphereCol->SetHitCollider(*itA);
				}

				// メッシュ
				else if (it_hit->GetShapeType() == SHAPE_MESH) {
					MeshCollider* meshCol = dynamic_cast<MeshCollider*>(it_hit);
					meshCol->SetIsHit(true);
					meshCol->SetInter(inter);
					meshCol->SetHitCollider(*itA);
				}

				// AABB
				else if (it_hit->shapeType_ == SHAPE_AABB)
				{
					AABBCollider* aabbCol = dynamic_cast<AABBCollider*>(it_hit);
					aabbCol->SetIsHit(true);
					aabbCol->SetHitCollider(*itA);
				}
			}
		}

		// レイ以外だったら
		else {
			for (; itB != colliders_.end(); ++itB) {

				count++;
				// 属性が合わなければ除外
				if (!((*itA)->attribute_ & (*itB)->attribute_)) continue;

				// ともに球
				if ((*itA)->GetShapeType() == SHAPE_SPHERE && (*itB)->GetShapeType() == SHAPE_SPHERE) {
					// 衝突判定の引数のために球に変換
					Sphere* sphereA = dynamic_cast<Sphere*>(*itA);
					Sphere* sphereB = dynamic_cast<Sphere*>(*itB);

					// 衝突判定
					if (Collision::CheckSphere2Sphere(*sphereA, *sphereB)) {
						SphereCollider* sphereColA = dynamic_cast<SphereCollider*>(*itA);
						sphereColA->SetIsHit(true);
						sphereColA->SetHitCollider(*itB);

						SphereCollider* sphereColB = dynamic_cast<SphereCollider*>(*itB);
						sphereColB->SetIsHit(true);
						sphereColB->SetHitCollider(*itA);
					}
				}

				// メッシュと球
				else if ((*itA)->GetShapeType() == SHAPE_MESH && (*itB)->GetShapeType() == SHAPE_SPHERE) {
					// 衝突判定の引数のためにメッシュコライダーと球に変換
					MeshCollider* meshCol = dynamic_cast<MeshCollider*>(*itA);
					Sphere* sphere = dynamic_cast<Sphere*>(*itB);

					// 判定したときのデータ
					Vector3 inter = { 0.0f, 0.0f, 0.0f };
					Vector3 reject = { 0.0f, 0.0f, 0.0f };

					if (meshCol->CheckCollisionSphere(*sphere, &inter, &reject)) {
						meshCol->SetIsHit(true);
						meshCol->SetInter(inter);
						meshCol->AddReject(-reject);
						meshCol->SetHitCollider(*itB);

						// 衝突判定の引数のために球コライダーに変換
						SphereCollider* sphereCol = dynamic_cast<SphereCollider*>(*itB);
						sphereCol->SetIsHit(true);
						sphereCol->SetInter(inter);
						sphereCol->AddReject(reject);
						sphereCol->SetHitCollider(*itA);
					}
				}

				// 球とメッシュ
				else if ((*itA)->GetShapeType() == SHAPE_SPHERE && (*itB)->GetShapeType() == SHAPE_MESH) {
					// 衝突判定の引数のためにメッシュコライダーと球に変換
					Sphere* sphere = dynamic_cast<Sphere*>(*itA);
					MeshCollider* meshCol = dynamic_cast<MeshCollider*>(*itB);

					// 判定したときのデータ
					Vector3 inter = { 0.0f, 0.0f, 0.0f };
					Vector3 reject = { 0.0f, 0.0f, 0.0f };

					if (meshCol->CheckCollisionSphere(*sphere, &inter, &reject)) {

						// 衝突判定の引数のために球コライダーに変換
						SphereCollider* sphereCol = dynamic_cast<SphereCollider*>(*itA);
						sphereCol->SetIsHit(true);
						sphereCol->SetInter(inter);
						sphereCol->AddReject(reject);
						sphereCol->SetHitCollider(*itB);

						meshCol->SetIsHit(true);
						meshCol->SetInter(inter);
						meshCol->AddReject(-reject);
						meshCol->SetHitCollider(*itA);
					}
				}

				// AABB同士
				else if ((*itA)->GetShapeType() == SHAPE_AABB && (*itB)->GetShapeType() == SHAPE_AABB)
				{
					AABBCollider* aabb0 = dynamic_cast<AABBCollider*>(*itA);
					AABBCollider* aabb1 = dynamic_cast<AABBCollider*>(*itB);
					Vector3 reject = { 0.0f, 0.0f, 0.0f };

					if (Collision::CheckAABB2AABB(*aabb0, *aabb1, &reject))
					{
						aabb0->SetIsHit(true);
						aabb1->SetIsHit(true);
						aabb0->AddReject(reject);
						aabb0->PushBack(reject * 0.5f);
						aabb1->PushBack(-reject * 0.5f);
						aabb0->SetHitCollider(*itB);
						aabb1->SetHitCollider(*itA);
					}
				}

			}
		}
	}

	count = count;
}

CollisionManager* CollisionManager::GetInstance()
{
	// インスタンス生成
	static CollisionManager inst;

	// インスタンスを返す
	return &inst;
}
