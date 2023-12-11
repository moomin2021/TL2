#pragma once

enum CollisionShapeType {
	SHAPE_UNKNOWN = -1,	// 未設定
	SHAPE_SPHERE,		// 球
	SHAPE_MESH,			// メッシュ
	SHAPE_RAY,			// レイ
	SHAPE_POINT,		// 点
	SHAPE_BOX,			// 矩形
	SHAPE_CIRCLE,		// 円
	SHAPE_AABB,			// AABB
};
