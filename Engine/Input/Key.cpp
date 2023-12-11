#include "Key.h"
#include "WinAPI.h"
#include <cassert>

Key* Key::GetInstance() {
	// インスタンス生成
	static Key inst;

	// インスタンスを返す
	return &inst;
}

void Key::Update() {
	// 前フレームのキーの状態を保存
	for (size_t i = 0; i < 256; i++) oldKeys_[i] = keys_[i];

	// マウスデバイス制御開始
	device_->Acquire();

	// 全キーの入力状態を取得する
	device_->GetDeviceState(static_cast<DWORD>(keys_.size()), keys_.data());
}

Key::Key() :
#pragma region 初期化リスト
	// 入力情報
	keys_(256),		// 現在のキーボードの情報
	oldKeys_(256),	// 前フレームのキーボードの情報

	// デバイス
	device_(nullptr)
#pragma endregion
{
	// インスタンス取得
	WinAPI* win = WinAPI::GetInstance();

	// 関数が成功したかどうかを判別する用変数
	HRESULT result;

#pragma region DirectInputの初期化

	ComPtr<IDirectInput8> directInput = nullptr;
	result = DirectInput8Create(
		win->GetWinClass().hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8,
		(void**)&directInput, nullptr
	);
	assert(SUCCEEDED(result));

#pragma endregion

#pragma region デバイスの生成

	// デバイスの生成
	result = directInput->CreateDevice(GUID_SysKeyboard, &device_, NULL);
	assert(SUCCEEDED(result));

	// キーボード入力データの形式のセット
	result = device_->SetDataFormat(&c_dfDIKeyboard);// 標準形式
	assert(SUCCEEDED(result));

	// 排他制御レベルのセット
	result = device_->SetCooperativeLevel(
		win->GetHWND(), DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY
	);
	assert(SUCCEEDED(result));

	// デバイス制御開始
	device_->Acquire();

#pragma endregion
}

Key::~Key() {
	// キーボードデバイスの制御制御
	device_->Unacquire();

	// キーボードデバイスの解放
	device_->Release();
}
