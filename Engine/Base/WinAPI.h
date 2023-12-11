#pragma once
#include <Windows.h>
#include <stdint.h>

class WinAPI {
#pragma region メンバ変数
private:
	// ウィンドウサイズ
	uint16_t winWidth_;	// 横幅
	uint16_t winHeight_;// 縦幅

	WNDCLASSEX	winClass_;	// ウィンドウクラス
	RECT		winRect_;	// ウィンドウサイズ
	HWND		hwnd_;		// ウィンドウハンドル(オブジェクト)
	MSG			msg_;		// メッセージ
#pragma endregion

#pragma region メンバ関数
public:
	/// <summary>
	/// インスタンス取得
	/// </summary>
	/// <returns> インスタンス </returns>
	static WinAPI* GetInstance();

	/// <summary>
	/// 初期化処理
	/// </summary>
	void Initialize(uint16_t winWidth, uint16_t winHeight);

	/// <summary>
	/// ウィンドウを閉じるメッセージが来ているか
	/// </summary>
	/// <returns> 真偽結果 </returns>
	bool IsEndMessage();

	// カーソルの設定
	void DisplayCursor(bool isDisplay = true);

	// カーソルを画面の範囲外にいあかないようにする
	void SetClipCursor(bool isClip = false);

#pragma region ゲッター関数
	/// <summary>
	/// ウィンドウサイズの横幅を取得
	/// </summary>
	/// <returns> ウィンドウサイズの横幅 </returns>
	inline const uint16_t& GetWidth() { return winWidth_; }

	/// <summary>
	/// ウィンドウサイズの縦幅を取得
	/// </summary>
	/// <returns> ウィンドウサイズの縦幅 </returns>
	inline const uint16_t& GetHeight() { return winHeight_; }

	/// <summary>
	/// ウィンドウクラスを取得
	/// </summary>
	/// <returns> ウィンドウクラス </returns>
	inline const WNDCLASSEX& GetWinClass() { return winClass_; }

	/// <summary>
	/// ウィンドウハンドルを取得
	/// </summary>
	/// <returns> ウィンドウハンドル </returns>
	inline const HWND& GetHWND() { return hwnd_; }

#pragma endregion

private:
	/// <summary>
	/// コンストラクタ
	/// </summary>
	WinAPI();

	/// <summary>
	/// デストラクタ
	/// </summary>
	~WinAPI();

	/// <summary>
	/// ウィンドウオブジェクト作成
	/// </summary>
	void CreateWindowObj();

#pragma endregion

#pragma region 特殊関数
	// 禁止
	WinAPI(const WinAPI&) = delete;				// コピーコンストラクタ禁止
	WinAPI& operator = (const WinAPI&) = delete;// コピー代入演算子禁止
#pragma endregion
};
