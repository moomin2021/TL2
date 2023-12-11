#pragma once
#include "Vector3.h"
#include "Vector3.h"

class DirectionalLight {
#pragma region 構造体
public:
	// 定数バッファ用データ構造
	struct ConstBufferData {
		Vector3 lightVec;	// ライトへ方向を表す
		float pad1;
		Vector3 lightColor;	// ライトの色
		bool active;		// 有効フラグ
	};
#pragma endregion

#pragma region メンバ変数
private:
	// ライト光線方向
	Vector3 lightDir_ = { 0.0f, -1.0f, 0.0f };

	// ライト色
	Vector3 lightColor_ = { 1.0f, 1.0f, 1.0f };

	// 有効フラグ
	bool active_ = true;
#pragma endregion

#pragma region セッター関数
public:
	/// <summary>
	/// ライトの向き(XYZ)を設定
	/// </summary>
	/// <param name="lightDir"> ライトの向き(XYZ) </param>
	inline void SetLightDir(const Vector3& lightDir) { lightDir_ = Vector3Normalize(lightDir); }

	/// <summary>
	/// ライトの色(RGB)を設定
	/// </summary>
	/// <param name="color"> ライトの色(RGB) </param>
	inline void SetLightColor(const Vector3& color) { lightColor_ = color; }

	/// <summary>
	/// ライトの有効フラグを設定
	/// </summary>
	/// <param name="active"> 有効フラグ </param>
	inline void SetActive(bool active) { active_ = active; }
#pragma endregion

#pragma region ゲッター関数
public:
	/// <summary>
	/// ライトの方向(XYZ)を取得
	/// </summary>
	/// <returns> ライトの方向(XYZ) </returns>
	inline const Vector3& GetLightDir() { return lightDir_; }

	/// <summary>
	/// ライトの色(RGB)を取得
	/// </summary>
	/// <returns> ライトの色(RGB) </returns>
	inline const Vector3& GetLightColor() { return lightColor_; }

	/// <summary>
	/// 有効フラグを取得
	/// </summary>
	/// <returns> 有効フラグ </returns>
	inline bool GetActive() { return active_; }
#pragma endregion
};
