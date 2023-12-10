#pragma once
#include <string>
#include <DirectXTex.h>

class TextureConverter
{
#pragma region メンバ変数
	// 画像の情報
	DirectX::TexMetadata metaData_ = {};

	// 画像イメージのコンテナ
	DirectX::ScratchImage scratchImage_ = {};

	// ディレクトリパス
	std::wstring directoryPath_ = {};

	// ファイル名
	std::wstring fileName_ = {};

	// ファイル拡張子
	std::wstring fileExt_ = {};
#pragma endregion

#pragma region メンバ関数
public:
	/// <summary>
	/// テクスチャをWICからDDSに変換する
	/// </summary>
	/// <param name="filePath"> ファイルパス </param>
	void ConvertTextureWICToDDS(const std::string& filePath);

private:
	/// <summary>
	/// テクスチャファイル読み込み
	/// </summary>
	/// <param name="filePath"> ファイルパス </param>
	void LoadWICTextureFromFile(const std::string& filePath);

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
	void SeparateFilePath(const std::wstring& filePath);

	/// <summary>
	/// DDSテクスチャとしてファイル書き出し
	/// </summary>
	void SaveDDSTextureToFile();
#pragma endregion
};