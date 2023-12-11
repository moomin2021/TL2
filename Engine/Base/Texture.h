#pragma once
#include <d3d12.h>
#include <DirectXTex.h>
#include <wrl.h>
#include <string>
#include <map>

struct FileName {
	// ディレクトリパス
	std::wstring directoryPath = {};

	// ファイル名
	std::wstring fileName = {};

	// ファイル拡張子
	std::wstring fileExt = {};
};

class Texture {
private:
	// エイリアステンプレート
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

#pragma region メンバ変数
private:
	// 読み込んだ回数
	uint16_t loadCounter_ = 0;

	// テクスチャバッファ
	std::map<const std::wstring, ComPtr<ID3D12Resource>> texBuff_;

	// テクスチャハンドル
	std::map<const std::wstring, uint16_t> texHandle_;

	// SRV用デスクリプタヒープ
	ComPtr<ID3D12DescriptorHeap> srvHeap_ = nullptr;

	// SRVヒープのハンドル
	D3D12_CPU_DESCRIPTOR_HANDLE srvHandle_{};

	std::vector<ID3D12Resource*> intermediateResources_;
#pragma endregion

#pragma region メンバ関数
public:
	/// <summary>
	/// インスタンス取得
	/// </summary>
	/// <returns> インスタンス </returns>
	static Texture* GetInstance();

	/// <summary>
	/// 初期化処理
	/// </summary>
	void Initialize();

	// 画像イメージを読み取る
	void LoadImageFile(const std::string filePath, DirectX::ScratchImage& scratchImage, DirectX::TexMetadata& metadata, FileName fileName);

	// テクスチャバッファを生成
	ID3D12Resource* CreateTextureResource(const DirectX::TexMetadata& metadata, D3D12_RESOURCE_DESC& texResourceDesc);

	// テクスチャバッファにデータを転送
	ID3D12Resource* UploadTextureData(ID3D12Resource* texture, const DirectX::ScratchImage& mipImages);

	ID3D12Resource* CreateBufferResource(size_t sizeInBytes);
	/// <summary>
	/// テクスチャを読み込み、ハンドルを取得する
	/// </summary>
	/// <param name="fileName"> 画像ファイル名 </param>
	/// <returns> 読み込んだ画像のハンドル </returns>
	uint16_t LoadTexture(const std::string fileName);

	void ReleaseIntermediateResources();

#pragma region ゲッター関数
	/// <summary>
	/// SRVデスクリプタヒープの取得
	/// </summary>
	/// <returns> SRVデスクリプタヒープ </returns>
	ID3D12DescriptorHeap* GetSRVHeap() { return srvHeap_.Get(); }
#pragma endregion

private:
	/// <summary>
	/// コンストラクタ
	/// </summary>
	Texture();

	/// <summary>
	/// SRV用でスクリプタヒープ生成
	/// </summary>
	void CreateDescriptorHeap();

	/// <summary>
	/// マルチバイト文字列をワイド文字列も変換
	/// </summary>
	/// <param name="mString"> マルチバイト文字列 </param>
	/// <returns> ワイド文字列 </returns>
	static std::wstring ConvertMultiByteStringToWideString(const std::string& mString);

	/// <summary>
	/// フォルダパスとファイル名を分離する
	/// </summary>
	/// <param name="filePath"> ファイルパス </param>
	FileName SeparateFilePath(const std::wstring& filePath);

#pragma endregion

#pragma region 特殊関数
	// 禁止
	Texture(const Texture&) = delete;				// コピーコンストラクタ禁止
	Texture& operator = (const Texture&) = delete;	// コピー代入演算子禁止
#pragma endregion
};

/// <summary>
/// テクスチャを読み込み、ハンドルを取得する
/// </summary>
/// <param name="fileName"> 画像ファイル名 </param>
/// <returns> 読み込んだ画像のハンドル </returns>
uint16_t LoadTexture(const std::string fileName);
