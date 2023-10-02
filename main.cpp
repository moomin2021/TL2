#include <cstdio>
#include <cstdlib>
#include <cstdint>
#include <DirectXTex.h>
//#include <cassert>

#include "TextureConverter.h"

// コマンドライン引数
enum Argument {
	kApplicationPath,	// アプリケーションパス
	kFilePath,			// 渡されたファイルのパス

	NumArgument
};

int main(uint16_t argc, char* argv[]) {
	assert(argc >= NumArgument);

	// COMライブラリの初期化
	HRESULT hr = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
	assert(SUCCEEDED(hr));

	// テクスチャコンバータ
	TextureConverter converter;

	// テクスチャ変換
	converter.ConvertTextureWICToDDS(argv[kFilePath]);

	// COMライブラリの終了
	CoUninitialize();

	system("pause");
	return 0;
}