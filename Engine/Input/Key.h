#pragma once
// --DirectXInput-- //
#define DIREXTINPUT_VERSION 0x0800// DirectInputのバージョン指定
#include <wrl.h>
#include <dinput.h>
#include <vector>

class Key {
	// namespaceの省略
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

#pragma region メンバ変数
private:
	// 入力情報
	std::vector<uint8_t> keys_;		// 現在のキーボードの情報
	std::vector<uint8_t> oldKeys_;	// 前フレームのキーボードの情報

	// デバイス
	ComPtr<IDirectInputDevice8> device_;
#pragma endregion

#pragma region メンバ関数
public:
	/// <summary>
	/// インスタンス取得
	/// </summary>
	/// <returns> インスタンス </returns>
	static Key* GetInstance();

	/// <summary>
	/// 更新処理
	/// </summary>
	void Update();

	/// <summary>
	/// 指定されたキーが押されていたら、1を返し、そうでなかったら0を返す
	/// </summary>
	/// <param name="key"> 判定したいキー </param>
	/// <returns></returns>
	bool PushKey(uint16_t key) { return keys_[key]; }

	/// <summary>
	/// 指定されたキーが押された瞬間だったら、1を返し、そうでなかったら0を返す
	/// </summary>
	/// <param name="key"> 判定したいキー </param>
	/// <returns></returns>
	bool TriggerKey(uint16_t key) { return keys_[key] && !oldKeys_[key]; }

private:
	/// <summary>
	/// コンストラクタ
	/// </summary>
	Key();

	/// <summary>
	/// デストラクタ
	/// </summary>
	~Key();
#pragma endregion

#pragma region 特殊関数
	// 禁止
	Key(const Key&) = delete;				// コピーコンストラクタ禁止
	Key& operator = (const Key&) = delete;	// コピー代入演算子禁止
#pragma endregion
};
