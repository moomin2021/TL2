#pragma once
#include "float4.h"

class PointLight {
#pragma region 構造体
public:
	// 定数バッファ用データ構造体
	struct ConstBufferData {
		float4 color;	// ライトの色
		Vector3 pos;	// ライトの位置
		float intensity;// 輝度
		float radius;	// ライトの届く距離
		float decay;	// 減衰率
		bool active;	// アクティブ
		float pad;
	};
#pragma endregion

#pragma region メンバ変数
private:
	// ライト座標(ワールド座標系)
	Vector3 position_ = { 0.0f, 0.0f, 0.0f };

	// ライト色
	Vector3 color_ = { 1.0f, 1.0f, 1.0f };

	// 輝度
	float intensity_ = 1.0f;

	// 距離
	float radius_ = 1.0f;

	// 減衰率
	float decay_ = 1.0f;

	// 有効フラグ
	bool active_ = true;
#pragma endregion

#pragma region セッター関数
public:
	/// <summary>
	/// 点光源の座標
	/// </summary>
	/// <param name="inPos"></param>
	inline void SetPosition(const Vector3& inPos) { position_ = inPos; }

	/// <summary>
	/// 点光源の色
	/// </summary>
	/// <param name="inColor"></param>
	inline void SetColor(const Vector3& inColor) { color_ = inColor; }

	/// <summary>
	/// 点光源の輝度
	/// </summary>
	/// <param name="inIntensity"></param>
	inline void SetIntensity(float inIntensity) { intensity_ = inIntensity; }

	/// <summary>
	/// 点光源の最大距離(半径)
	/// </summary>
	/// <param name="inRadius"></param>
	inline void SetRadius(float inRadius) { radius_ = inRadius; }

	/// <summary>
	/// 点光源の減衰率
	/// </summary>
	/// <param name="inDecay"></param>
	inline void SetDecay(float inDecay) { decay_ = inDecay; }

	/// <summary>
	/// 点光源のアクティブフラグ
	/// </summary>
	/// <param name="inActive"></param>
	inline void SetActive(bool inActive) { active_ = inActive; }
#pragma endregion

#pragma region ゲッター関数
public:
	/// <summary>
	/// 点光源の座標
	/// </summary>
	/// <returns></returns>
	inline const Vector3& GetPosition() { return position_; }

	/// <summary>
	/// 点光源の色
	/// </summary>
	/// <returns></returns>
	inline const Vector3& GetColor() { return color_; }

	/// <summary>
	/// 点光源の輝度
	/// </summary>
	/// <returns></returns>
	inline float GetIntensity() { return intensity_; }

	/// <summary>
	/// 点光源の半径
	/// </summary>
	/// <returns></returns>
	inline float GetRadius() { return radius_; }

	/// <summary>
	/// 点光源の減衰率
	/// </summary>
	/// <returns></returns>
	inline float GetDecay() { return decay_; }

	/// <summary>
	/// 点光源のアクティブフラグ
	/// </summary>
	/// <returns></returns>
	inline bool GetActive() { return active_; }
#pragma endregion
};
