#include "Mouse.h"
#include <cassert>
#define DIREXTINPUT_VERSION 0x0800// DirectInputのバージョン指定

#include "WinAPI.h"

Mouse* Mouse::GetInstance()
{
	// インスタンス生成
	static Mouse inst;

	// インスタンスを返す
	return &inst;
}

void Mouse::Update() {
	// インスタンス取得
	WinAPI* win = WinAPI::GetInstance();

	// 前フレームのマウスの入力状態を保存
	oldMouse_ = nowMouse_;

	// マウスの入力状態を取得
	device_->GetDeviceState(sizeof(DIMOUSESTATE), &nowMouse_);

	// マウスデバイス制御開始
	device_->Acquire();

	// マウスの座標を取得
	GetCursorPos(&p_);
	ScreenToClient(FindWindowW(win->GetWinClass().lpszClassName, nullptr), &p_);
}

Mouse::Mouse() :
#pragma region 初期化リスト
	// デバイス
	device_(nullptr),

	// マウスデータ
	nowMouse_{},// 現在のマウスの入力状態
	oldMouse_{},// 前フレームのマウスの入力状態
	
	// マウスカーソル座標
	p_{}
#pragma endregion
{
	// インスタンス取得
	WinAPI* win = WinAPI::GetInstance();

	// 関数実行の成否を判別用の変数
	HRESULT result;

	// DirectInputデバイス生成
	ComPtr<IDirectInput8> directInput = nullptr;
	result = DirectInput8Create(
		win->GetWinClass().hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8,
		(void**)&directInput, nullptr
	);
	assert(SUCCEEDED(result));

	// マウスデバイスの生成
	result = directInput->CreateDevice(GUID_SysMouse, &device_, NULL);
	assert(SUCCEEDED(result));

	// マウス入力データの形式のセット
	result = device_->SetDataFormat(&c_dfDIMouse);// 標準形式
	assert(SUCCEEDED(result));

	// 排他制御レベルのセット
	result = device_->SetCooperativeLevel(
		win->GetHWND(), DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY
	);
	assert(SUCCEEDED(result));

	// マウスデバイス制御開始
	device_->Acquire();

	// DirectInputデバイスの解放
	directInput->Release();
}

Mouse::~Mouse()
{
	// デバイス制御
	device_->Unacquire();

	// デバイスの解放
	device_->Release();
}
