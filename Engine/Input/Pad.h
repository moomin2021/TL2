#pragma once
// DirectInputバージョン指定
#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>
#include <Xinput.h>
#include <wrl.h>
#include <stdint.h>

#include "Vector2.h"

// ボタンマスク
enum BUTTON {
	PAD_UP			= 0x0001,// 十字キー上
	PAD_DOWN		= 0x0002,// 十字キー下
	PAD_LEFT		= 0x0004,// 十字キー左
	PAD_RIGHT		= 0x0008,// 十字キー右
	PAD_START		= 0x0010,// Startボタン
	PAD_BACK		= 0x0020,// Backボタン
	PAD_LSTICK		= 0x0040,// Lスティック押し込み
	PAD_RSTICK		= 0x0080,// Rスティック押し込み
	PAD_LB			= 0x0100,// LBボタン
	PAD_RB			= 0x0200,// RBボタン
	PAD_A			= 0x1000,// Aボタン
	PAD_B			= 0x2000,// Bボタン
	PAD_X			= 0x4000,// Xボタン
	PAD_Y			= 0x8000,// Ｙボタン
};

class Pad
{
private:
	// エイリアステンプレート
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

#pragma region 構造体

#pragma endregion

#pragma region メンバ変数
private:
	XINPUT_STATE state_ = {};
	XINPUT_STATE oldState_ = {};
#pragma endregion

#pragma region メンバ関数
public:
	// インスタンス取得
	static Pad* GetInstance();

	// 初期化処理
	void Initialize();

	// 更新処理
	void Update();

	// 指定したボタンの入力状態を取得
	bool GetPushButton(BUTTON button) { return state_.Gamepad.wButtons & button; }

	// 指定したボタンを押した瞬間かどうか
	bool GetTriggerButton(BUTTON button) { return (state_.Gamepad.wButtons & button) && !(oldState_.Gamepad.wButtons & button); }

	bool GetReleasButton(BUTTON button) { return !(state_.Gamepad.wButtons & button) && (oldState_.Gamepad.wButtons & button); }

	// Lトリガーの現在の値を返す(倒せば倒すほど値が大きくなる0 ~ 255)
	uint8_t GetLTrigger() { return state_.Gamepad.bLeftTrigger; }

	// Rトリガーの現在の値を返す(倒せば倒すほど値が大きくなる0 ~ 255)
	uint8_t GetRTrigger() { return state_.Gamepad.bLeftTrigger; }

	// 左スティックの横軸(X)と縦軸(Y)を返す(-1.0f ~ 1.0f)
	Vector2 GetLStick(int16_t deadZone = 5000);

	// 右スティックの横軸(X)と縦軸(Y)を返す(-1.0f ~ 1.0f)
	Vector2 GetRStick(int16_t deadZone = 5000);
private:
	// コンストラクタ
	Pad();

	// デストラクタ
	~Pad();
#pragma endregion

#pragma region 特殊関数
	// 禁止
	Pad(const Pad&) = delete;				// コピーコンストラクタ禁止
	Pad& operator = (const Pad&) = delete;	// コピー代入演算子禁止
#pragma endregion
};
