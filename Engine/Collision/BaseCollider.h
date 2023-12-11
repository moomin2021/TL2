#pragma once
#include "Object3D.h"
#include "CollisionShapeType.h"

class BaseCollider {
public:
	friend class CollisionManager;

#pragma region メンバ変数
protected:
	// 形状タイプ
	CollisionShapeType shapeType_ = SHAPE_UNKNOWN;

	// 衝突判定属性
	uint16_t attribute_ = 0;

	// 任意で紐付けるオブジェクト
	Object3D* object_ = nullptr;

	// 衝突フラグ
	bool isHit_ = false;

	// 衝突したコライダー
	BaseCollider* hitCol_ = nullptr;
#pragma endregion

#pragma region メンバ関数
public:
	// デストラクタ
	virtual ~BaseCollider() = default;

	// 更新処理
	virtual void Update() = 0;
#pragma endregion

#pragma region セッター関数
	// 形状タイプを設定
	inline void SetShapeType(CollisionShapeType shapeType) { shapeType_ = shapeType; }

	// 衝突判定属性を設定
	inline void SetAttribute(uint16_t attribute) { attribute_ = attribute; }

	// オブジェクトを設定
	inline void SetObject3D(Object3D* object) { object_ = object; }

	// 衝突フラグを設定
	inline void SetIsHit(bool flag) { isHit_ = flag; }

	// 衝突したコライダーを設定
	inline void SetHitCollider(BaseCollider* collider) { hitCol_ = collider; }
#pragma endregion

#pragma region ゲッター関数
	// 形状タイプを取得
	inline CollisionShapeType GetShapeType() { return shapeType_; }

	// 衝突判定属性を取得
	inline uint16_t GetAttribute() { return attribute_; }

	// 衝突フラグを取得
	inline bool GetIsHit() { return isHit_; }

	// 衝突したコライダーを取得
	inline BaseCollider* GetHitCollider() { return hitCol_; }
#pragma endregion
};
