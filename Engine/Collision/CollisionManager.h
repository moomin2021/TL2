#pragma once
#include "BaseCollider.h"
#include "AABBCollider.h"

#include <forward_list>

class CollisionManager
{
#pragma region メンバ変数
	// コライダー
	std::forward_list<BaseCollider*> colliders_;

	// ブロックのコライダー
	std::forward_list<AABBCollider*> blockCollider_ = {};
#pragma endregion

#pragma region メンバ関数
public:
	// コンストラクタ
	CollisionManager() = default;

	// デストラクタ
	~CollisionManager();

	// 全ての衝突チェック
	void CheckAllCollision();

	/// <summary>
	/// コライダーの追加
	/// </summary>
	/// <param name="collider"> コライダー </param>
	inline void AddCollider(BaseCollider* collider) { colliders_.push_front(collider); }

	/// <summary>
	/// ブロックコライダーの追加
	/// </summary>
	/// <param name="collider"> AABBコライダー </param>
	inline void AddBlockCollider(AABBCollider* collider) { blockCollider_.emplace_front(collider); }

	/// <summary>
	/// コライダーの削除
	/// </summary>
	/// <param name="collider"></param>
	inline void RemoveCollider(BaseCollider* collider) { colliders_.remove(collider); }
#pragma endregion

#pragma region ゲッター関数
public:
	// インスタンス取得
	static CollisionManager* GetInstance();
#pragma endregion

#pragma region 特殊関数
	CollisionManager(const CollisionManager&) = delete;				// コピーコンストラクタ禁止
	CollisionManager& operator = (const CollisionManager&) = delete;	// コピー代入演算子禁止
#pragma endregion
};
