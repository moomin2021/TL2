#pragma once
#include <wrl.h>
#include <d3d12.h>

class ImGuiManager
{
private:
	// エイリアステンプレート
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

#pragma region メンバ変数
private:
	// SRV用デスクリプタヒープ
	ComPtr<ID3D12DescriptorHeap> srvHeap_ = nullptr;
#pragma endregion

#pragma region メンバ関数
public:
	// インスタンス取得
	static ImGuiManager* GetInstance();

	// 初期化処理
	void Initialize();

	// ImGui受付開始
	void Begin();

	// ImGui受付終了
	void End();

	// 画面への描画
	void Draw();

private:
	// コンストラクタ
	ImGuiManager();

	// デストラクタ
	~ImGuiManager();
#pragma endregion

#pragma region 特殊関数
	// 禁止
	ImGuiManager(const ImGuiManager&) = delete;				// コピーコンストラクタ禁止
	ImGuiManager& operator = (const ImGuiManager&) = delete;	// コピー代入演算子禁止
#pragma endregion
};
