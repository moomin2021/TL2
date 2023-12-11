#pragma once
// DirectXInput
#include <dinput.h>

// ComPtr用
#include <wrl.h>

// Vector2
#include "Vector2.h"

enum MouseButton {
	M_LEFT,
	M_RIGHT,
	M_CENTER
};

class Mouse {
private:
	// エイリアステンプレート
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

#pragma region メンバ変数
private:
	// デバイス
	ComPtr<IDirectInputDevice8> device_;

	// マウスデータ
	DIMOUSESTATE nowMouse_;// 現在のマウスの入力状態
	DIMOUSESTATE oldMouse_;// 前フレームのマウスの入力状態

	// マウスカーソル座標
	POINT p_;
#pragma endregion

#pragma region メンバ関数
public:
	/// <summary>
	/// インスタンス取得
	/// </summary>
	/// <returns> インスタンス </returns>
	static Mouse* GetInstance();

	/// <summary>
	/// 更新処理
	/// </summary>
	void Update();

	/// <summary>
	/// 指定されたマウスのボタンが押されていたら、1を返し、そうでなかった場合0を返す
	/// </summary>
	/// <param name="button"> 判定したいマウスボタン </param>
	/// <returns></returns>
	bool PushMouseButton(MouseButton button) { return nowMouse_.rgbButtons[button] == (0x80); }

	/// <summary>
	/// 指定されたマウスのボタンが押された瞬間だったら、1を返し、そうでなかった場合0を返す
	/// </summary>
	/// <param name="button"> 判定したいマウスボタン </param>
	/// <returns></returns>
	bool TriggerMouseButton(MouseButton button) { return nowMouse_.rgbButtons[button] == (0x80) && !(oldMouse_.rgbButtons[button] == (0x80)); }

	/// <summary>
	/// マウスの位置座標
	/// </summary>
	/// <returns> マウスの位置座標 </returns>
	Vector2 MousePos() { return Vector2(static_cast<float>(p_.x), static_cast<float>(p_.y)); }

	/// <summary>
	/// マウスの移動量
	/// </summary>
	/// <returns> マウスの移動量 </returns>
	Vector2 GetMouseVelosity() { return Vector2((float)nowMouse_.lX, (float)nowMouse_.lY); }

private:
	/// <summary>
	/// コンストラクタ
	/// </summary>
	Mouse();

	/// <summary>
	/// デストラクタ
	/// </summary>
	~Mouse();
#pragma endregion

#pragma region 特殊関数
	// 禁止
	Mouse(const Mouse&) = delete;				// コピーコンストラクタ禁止
	Mouse& operator = (const Mouse&) = delete;	// コピー代入演算子禁止
#pragma endregion
};
