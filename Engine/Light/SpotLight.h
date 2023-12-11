#pragma once
#include "Vector2.h"
#include "Util.h"

class SpotLight {
#pragma region 構造体
public:
	// 定数バッファ用データ構造体
	struct ConstBufferData {
		Vector3 lightVec;			// ライトの方向(XYZ)
		Vector3 lightPos;			// ライトの座標(XYZ)
		float pad1;// パディング
		Vector3 lightColor;			// ライトの色(RGB)
		float pad2;// パディング
		Vector3 lightAtten;			// ライトの距離減衰係数(XYZ)
		float pad3;// パディング
		Vector2 lightFactorAngleCos;	// ライトの減衰角度(開始角度, 終了角度)
		bool active;				// 有効フラグ
		float pad4;// パディング
	};
#pragma endregion

#pragma region メンバ変数
private:
	// ライト方向(XYZ)
	Vector3 lightDir_ = { 1.0f, 0.0f, 0.0f };

	// ライト座標(XYZ)
	Vector3 lightPos_ = { 0.0f, 0.0f, 0.0f };

	// ライト色
	Vector3 lightColor_ = { 1.0f, 1.0f, 1.0f };

	// ライト距離減衰係数
	Vector3 lightAtten_ = { 1.0f, 1.0f, 1.0f };

	// ライト減衰角度(開始角度、終了角度)
	Vector2 lightFactorAngleCos_ = { 0.5f, 0.2f };

	// 有効フラグ
	bool active_ = true;
#pragma endregion

#pragma region セッター関数
public:
	/// <summary>
	/// ライトの方向(XYZ)を設定
	/// </summary>
	/// <param name="lightDir"> ライトの方向(XYZ) </param>
	inline void SetLightDir(const Vector3& lightDir) { lightDir_ = Vector3Normalize(lightDir); }

	/// <summary>
	/// ライトの座標(XYZ)を設定
	/// </summary>
	/// <param name="lightPos"> ライトの座標(XYZ) </param>
	inline void SetLightPos(const Vector3& lightPos) { lightPos_ = lightPos; }

	/// <summary>
	/// ライトの色(RGB)を設定
	/// </summary>
	/// <param name="lightColor"> ライトの色(RGB) </param>
	inline void SetLightColor(const Vector3& lightColor) { lightColor_ = lightColor; }

	/// <summary>
	/// ライトの距離減衰係数(XYZ)を設定
	/// </summary>
	/// <param name="lightAtten"> ライトの距離減衰係数(XYZ) </param>
	inline void SetLightAtten(const Vector3& lightAtten) { lightAtten_ = lightAtten; }

	/// <summary>
	/// ライト減衰角度(開始角度、終了角度)を設定
	/// </summary>
	/// <param name="lightFactorAngle"> ライトの減衰角度(開始角度, 終了角度) </param>
	inline void SetLightFactorAngle(const Vector2& lightFactorAngle) {
		lightFactorAngleCos_.x = cosf(Util::Degree2Radian(lightFactorAngle.x));
		lightFactorAngleCos_.y = cosf(Util::Degree2Radian(lightFactorAngle.y));
	}

	/// <summary>
	/// ライト有効フラグを設定
	/// </summary>
	/// <param name="active"> ライト有効フラグ </param>
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
	/// ライトの座標(XYZ)を取得
	/// </summary>
	/// <returns> ライトの座標(XYZ) </returns>
	inline const Vector3& GetLightPos() { return lightPos_; }

	/// <summary>
	/// ライトの色(RGB)を取得
	/// </summary>
	/// <returns> ライトの色(RGB) </returns>
	inline const Vector3& GetLightColor() { return lightColor_; }

	/// <summary>
	/// ライトの距離減衰係数(XYZ)を取得
	/// </summary>
	/// <returns> ライトの距離減衰係数(XYZ) </returns>
	inline const Vector3& GetLightAtten() { return lightAtten_; }

	/// <summary>
	/// ライト減衰角度(開始角度、終了角度)を取得
	/// </summary>
	/// <returns> ライト減衰角度(開始角度、終了角度) </returns>
	inline const Vector2& GetLightFactorAngle() { return lightFactorAngleCos_; }

	/// <summary>
	/// ライト有効フラグを取得
	/// </summary>
	/// <returns> ライト有効フラグ </returns>
	inline bool GetActive() { return active_; }
#pragma endregion
};
