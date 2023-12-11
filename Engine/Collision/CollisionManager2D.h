#pragma once
#include "BaseCollider2D.h"

#include <forward_list>

class CollisionManager2D
{
#pragma region メンバ変数
	// コライダー
	std::forward_list<BaseCollider2D*> colliders_;
#pragma endregion

#pragma region メンバ関数
public:
	// コンストラクタ
	CollisionManager2D() = default;

	// デストラクタ
	~CollisionManager2D();

	// 全ての衝突チェック
	void CheckAllCollision();

	/// <summary>
	/// コライダーの追加
	/// </summary>
	/// <param name="collider"> コライダー </param>
	inline void AddCollider(BaseCollider2D* collider) { colliders_.push_front(collider); }

	/// <summary>
	/// コライダーの削除
	/// </summary>
	/// <param name="collider"></param>
	inline void RemoveCollider(BaseCollider2D* collider) { colliders_.remove(collider); }
#pragma endregion

#pragma region ゲッター関数
public:
	// インスタンス取得
	static CollisionManager2D* GetInstance();
#pragma endregion

#pragma region 特殊関数
	CollisionManager2D(const CollisionManager2D&) = delete;				// コピーコンストラクタ禁止
	CollisionManager2D& operator = (const CollisionManager2D&) = delete;	// コピー代入演算子禁止
#pragma endregion
};
