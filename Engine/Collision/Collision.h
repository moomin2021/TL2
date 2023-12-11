#pragma once
#include "CollisionPrimitive.h"

class Collision
{
public:
	/// <summary>
	/// 球と球の当たり判定
	/// </summary>
	/// <param name="sphere0"> 球0 </param>
	/// <param name="sphere1"> 球1 </param>
	/// <param name="inter">  </param>
	/// <returns></returns>
	static bool CheckSphere2Sphere(const Sphere& sphere0, const Sphere& sphere1);

	/// <summary>
	/// 球と平面の当たり判定
	/// </summary>
	/// <param name="sphere"> 球 </param>
	/// <param name="plane"> 平面 </param>
	/// <param name="inter"> 交点(平面上の最近接点) </param>
	/// <returns> 交差しているか否か </returns>
	static bool CheckSphere2Plane(const Sphere& sphere, const Plane& plane, Vector3* inter = nullptr);

	/// <summary>
	/// 点と三角形の最近接点を求める
	/// </summary>
	/// <param name="point"> 点 </param>
	/// <param name="traiangle"> 三角形 </param>
	/// <param name="closest"> 最近接点(出力用) </param>
	/// <returns>  </returns>
	static void ClosestPtPoint2Triangle(const Vector3& point, const Triangle& triangle, Vector3* closest);

	/// <summary>
	/// 球と法線付き三角形の当たり判定チェック
	/// </summary>
	/// <param name="sphere"> 球 </param>
	/// <param name="triangle"> 三角形 </param>
	/// <param name="inter"> 交点(三角形上の最近接点) </param>
	/// <returns> 交差しているか否か </returns>
	static bool CheckSphere2Triangle(const Sphere& sphere, const Triangle& triangle, Vector3* inter = nullptr, Vector3* reject = nullptr);

	/// <summary>
	/// レイと平面の当たり判定
	/// </summary>
	/// <param name="ray"> レイ </param>
	/// <param name="plane"> 平面 </param>
	/// <param name="distance"> 距離(出力用) </param>
	/// <param name="inter"> 交点(出力用) </param>
	/// <returns> 交差しているか否か </returns>
	static bool CheckRay2Plane(const Ray& ray, const Plane& plane, float* distance = nullptr, Vector3* inter = nullptr);

	/// <summary>
	/// レイと法線付き三角形の当たり判定
	/// </summary>
	/// <param name="ray"> レイ </param>
	/// <param name="triangle"> 三角形 </param>
	/// <param name="distance"> 距離(出力用) </param>
	/// <param name="inter"> 交点(出力用) </param>
	/// <returns> 交差しているか否か </returns>
	static bool CheckRay2Triangle(const Ray& ray, const Triangle& triangle, float* distance = nullptr, Vector3* inter = nullptr);

	/// <summary>
	/// レイと球の当たり判定
	/// </summary>
	/// <param name="ray"> レイ </param>
	/// <param name="sphere"> 球 </param>
	/// <param name="distance"> 距離(出力用) </param>
	/// <param name="inter"> 交点(出力用) </param>
	/// <returns> 交差しているか否か </returns>
	static bool CheckRay2Sphere(const Ray& ray, const Sphere& sphere, float* distance = nullptr, Vector3* inter = nullptr);

	/// <summary>
	/// 円と円の当たり判定
	/// </summary>
	/// <param name="circle0"> 円0 </param>
	/// <param name="circle1"> 円1 </param>
	/// <returns></returns>
	static bool CheckCircle2Circle(const Circle& circle0, const Circle& circle1);

	/// <summary>
	/// 四角形と四角形の当たり判定
	/// </summary>
	/// <param name="box0"> 四角形0 </param>
	/// <param name="box1"> 四角形1 </param>
	/// <returns></returns>
	static bool CheckBox2Box(const Box& box0, const Box& box1);

	/// <summary>
	/// 円と四角形の当たり判定
	/// </summary>
	/// <param name="circle"> 円 </param>
	/// <param name="box"> 四角形 </param>
	/// <returns></returns>
	static bool CheckCircle2Box(const Circle& circle, const Box& box);

	/// <summary>
	/// 点と四角形の当たり判定
	/// </summary>
	/// <param name="point"> 点 </param>
	/// <param name="box"> 四角形 </param>
	/// <returns></returns>
	static bool CheckPoint2Box(const Point& point, const Box& box);

	/// <summary>
	/// 点と円の当たり判定
	/// </summary>
	/// <param name="point"> 点 </param>
	/// <param name="circle"> 円 </param>
	/// <returns></returns>
	static bool CheckPoint2Circle(const Point& point, const Circle& circle);

	/// <summary>
	/// レイとAABBの当たり判定
	/// </summary>
	/// <param name="ray"> レイ </param>
	/// <param name="aabb"> AABB </param>
	/// <param name="inter"> 最近接点 </param>
	/// <returns></returns>
	static bool CheckRay2AABB(const Ray& ray, const AABB& aabb, float* pDistance = nullptr, Vector3* pInter = nullptr);

	/// <summary>
	/// AABB同士の当たり判定
	/// </summary>
	/// <param name="aabb0"> AABB </param>
	/// <param name="aabb1"> AABB </param>
	/// <param name="pInter"> 接点 </param>
	/// <param name="pReject"> 押し返しベクトル </param>
	/// <returns></returns>
	static bool CheckAABB2AABB(const AABB& aabb0, const AABB& aabb1, Vector3* pReject = nullptr);
};
