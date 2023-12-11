#include "WinAPI.h"

#include <imgui_impl_win32.h>

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

// ウィンドウプロシージャプロトタイプ宣言
// ※システムメッセージを処理するための関数
LRESULT WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	// ImGui用ウィンドウプロシージャ呼び出し
	if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, wparam, lparam)) return true;

	// メッセージに応じてゲーム固有の処理を行う
	switch (msg)
	{
		// ウィンドウが破棄された
	case WM_DESTROY:
		// OSに対して、アプリの終了を伝える
		PostQuitMessage(0);
		return 0;
	}

	// 標準のメッセージ処理を行う
	return DefWindowProc(hwnd, msg, wparam, lparam);
}

WinAPI* WinAPI::GetInstance()
{
	// インスタンス生成
	static WinAPI inst;

	// インスタンスを返す
	return &inst;
}

void WinAPI::Initialize(uint16_t winWidth, uint16_t winHeight)
{
	// ウィンドウサイズ保存
	winWidth_ = winWidth;
	winHeight_ = winHeight;

	// ウィンドウクラスの設定
	winClass_.cbSize = sizeof(WNDCLASSEX);// -> WNDCLASSEX構造体のサイズ
	winClass_.lpfnWndProc = (WNDPROC)WindowProc; // ウィンドウプロシージャを設定(アドレスを代入)
	winClass_.lpszClassName = L"DirectXGame"; // ウィンドウクラス名
	winClass_.hInstance = GetModuleHandle(nullptr); // ウィンドウハンドル
	winClass_.hCursor = LoadCursor(NULL, IDC_ARROW); // マウスカーソル指定

	// ウィンドウクラスをOSに登録する
	RegisterClassEx(&winClass_);

	// ウィンドウサイズ{ X座標 Y座標 横幅 縦幅 }
	winRect_ = { 0, 0, winWidth, winHeight };

	// 自動でサイズを補正する
	AdjustWindowRect(&winRect_, WS_POPUP, false);

	// ウィンドウオブジェクトの生成
	CreateWindowObj();

	// ウィンドウを表示状態にする
	ShowWindow(hwnd_, SW_SHOW);
}

bool WinAPI::IsEndMessage()
{
	// メッセージがあるか
	if (PeekMessage(&msg_, nullptr, 0, 0, PM_REMOVE))
	{
		// キー入力メッセージの処理
		TranslateMessage(&msg_);

		// プロシージャにメッセージを送る
		DispatchMessage(&msg_);
	}

	// ×ボタンで終了メッセージが来たら「真」を返す
	if (msg_.message == WM_QUIT)
	{
		return true;
	}

	// 「偽」を返す
	return false;
}

void WinAPI::DisplayCursor(bool isDisplay)
{
	ShowCursor(isDisplay);
}

void WinAPI::SetClipCursor(bool isClip)
{
	if (isClip)
	{
		RECT rect;

		GetWindowRect(hwnd_, &rect);

		// 指定範囲外にマウスがいかないように
		ClipCursor(&rect);
	}

	else
	{
		ClipCursor(nullptr);
	}
}

WinAPI::WinAPI() :
#pragma region 初期化リスト
	// ウィンドウサイズ
	winWidth_(0), winHeight_(0),

	// ウィンドウ設定関連
	winClass_{},// ウィンドウクラス
	winRect_{},	// ウィンドウサイズ
	hwnd_{},	// ウィンドウハンドル(オブジェクト)
	msg_{}		// メッセージ
#pragma endregion
{}

WinAPI::~WinAPI() {
	// ウィンドウクラス登録解除
	UnregisterClass(winClass_.lpszClassName, winClass_.hInstance);
}

void WinAPI::CreateWindowObj()
{
	// ウィンドウオブジェクトの生成
	hwnd_ = CreateWindow(
		winClass_.lpszClassName,// ------> クラス名
		L"DirectXGame",// -------> タイトルバーの文字
		WS_POPUP,// --> 標準的なウィンドウスタイル
		0,// --------> 表示X座標(OSに任せる)
		0,// --------> 表示Y座標(OSに任せる)
		winRect_.right - winRect_.left,// -> ウィンドウ横幅
		winRect_.bottom - winRect_.top,// -> ウィンドウ縦幅
		nullptr,// --------------> 親ウィンドウハンドル
		nullptr,// --------------> メニューハンドル
		winClass_.hInstance,// ----------> 呼び出しアプリケーションハンドル
		nullptr);// -------------> オプション
}
