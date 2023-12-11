#pragma once
#include <xaudio2.h>
#include <fstream>
#include <memory>
#include <wrl.h>
#include <map>
#include <string>
#include <vector>

// チャンクヘッダ
struct ChunkHeader {
	char id[4];		// チャンク毎のID
	uint32_t size;	// チャンクサイズ
};

// RIFFヘッダチャンク
struct RiffHeader {
	ChunkHeader chunk;	// RIFF
	char type[4];		// WAVE
};

// FMTチャンク
struct FormatChunk {
	ChunkHeader chunk;	// fmt
	WAVEFORMATEX fmt;	// 波形フォーマット
};

// 音声データ
struct SoundData {
	WAVEFORMATEX wfex;	// 波形フォーマット
	BYTE* pBuffer;		// バッファの先頭アドレス
	uint32_t bufferSize;// バッファサイズ
	float volume = 0;
};

class Sound {
private:
	// エイリアステンプレート
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
#pragma region メンバ変数
private:
	// XAudio2エンジンのインスタンス
	ComPtr<IXAudio2> pXAudio2_ = nullptr;

	// マスタリング音声
	IXAudio2MasteringVoice* pMasterVoice_ = nullptr;

	// 読み込んだ音声データ
	std::map<uint16_t, SoundData> soundDatas_ = {};

	// 音声ハンドルを保存
	std::map<std::string, uint16_t> soundHandles_ = {};

	// 再生中の音
	std::map<uint16_t, IXAudio2SourceVoice*> isPlaySounds_ = {};

	// 音声読み込みカウンター
	uint16_t soundCounter_ = 0;
#pragma endregion

#pragma region メンバ関数
public:
	// インスタンス取得
	static Sound* GetInstance();

	// 初期化処理
	void Initialize();

	// 更新処理
	void Update();

	// サウンド読み込み
	// volume = 0.0f ~ 1.0f
	uint16_t LoadWave(std::string fileName, float volume = 1.0f);

	// 再生
	void Play(uint16_t handle, bool isLoop = false);

	// 停止
	void Stop(uint16_t handle);

	// 解放
	void Release();
#pragma endregion
};
