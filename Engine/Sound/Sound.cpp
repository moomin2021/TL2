#include "Sound.h"

#include <cassert>

Sound* Sound::GetInstance()
{
	static Sound inst;

	return &inst;
}

void Sound::Initialize()
{
	HRESULT result;

	// XAudioエンジンのインスタンスを作成
	result = XAudio2Create(&pXAudio2_, 0, XAUDIO2_DEFAULT_PROCESSOR);
	assert(SUCCEEDED(result));

	// マスタリング音声を作成
	result = pXAudio2_->CreateMasteringVoice(&pMasterVoice_);
	assert(SUCCEEDED(result));
}

void Sound::Update()
{
	for (auto it = isPlaySounds_.begin(); it != isPlaySounds_.end();) {
		XAUDIO2_VOICE_STATE state;
		it->second->GetState(&state);

		if (state.BuffersQueued <= 0) {
			it->second->Stop(0);
			it = isPlaySounds_.erase(it);
		}

		else {
			++it;
		}
	}
}

uint16_t Sound::LoadWave(std::string fileName, float volume)
{
	if (soundHandles_.count(fileName) > 0) {
		return soundHandles_[fileName];
	}

	soundCounter_++;

	// ファイル入力ストリームのインスタンス
	std::ifstream file;

	// .wavファイルをバイナリモードで開く
	file.open(fileName, std::ios_base::binary);

	// ファイルオープン失敗を検出する
	assert(file.is_open());

	// RIFFヘッダーの読み込み
	RiffHeader riff;
	file.read((char*)&riff, sizeof(riff));

	// ファイルがRIFFかチェック
	if (strncmp(riff.chunk.id, "RIFF", 4) != 0) {
		assert(0);
	}

	// タイプがWAVEかチェック
	if (strncmp(riff.type, "WAVE", 4) != 0) {
		assert(0);
	}

	// Formatチェックの読み込み
	FormatChunk format = {};

	// チャンクヘッダーの確認
	file.read((char*)&format, sizeof(ChunkHeader));
	if (strncmp(format.chunk.id, "fmt ", 4) != 0) {
		assert(0);
	}

	// チャンク本体の読み込み
	assert(format.chunk.size <= sizeof(format.fmt));
	file.read((char*)&format.fmt, format.chunk.size);

	// Dataチャンクの読み込み
	ChunkHeader data{};
	file.read((char*)&data, sizeof(data));

	// JUNKチャンクを検出した場合
	if (strncmp(data.id, "JUNK", 4) == 0)
	{
		// 読み取り位置をJUNKチャンクの終わりまで進める
		file.seekg(data.size, std::ios_base::cur);

		// 再読み込み
		file.read((char*)&data, sizeof(data));
	}

	if (strncmp(data.id, "data", 4) != 0) {
		assert(0);
	}

	// Dataチャンクのデータ部（波形データ）の読み込み
	char* pBuffer = new char[data.size];
	file.read(pBuffer, data.size);

	// ファイルを閉じる
	file.close();

	// returnする為の音声データ
	SoundData soundData = {};
	soundData.wfex = format.fmt;
	soundData.pBuffer = reinterpret_cast<BYTE*>(pBuffer);
	soundData.bufferSize = data.size;
	soundData.volume = volume;

	// 音声データ保存
	soundDatas_.emplace(soundCounter_, soundData);
	soundHandles_.emplace(fileName, soundCounter_);

	return soundCounter_;
}

void Sound::Play(uint16_t handle, bool isLoop)
{
	HRESULT result;

	if (isPlaySounds_.count(handle) > 0) {
		for (auto it = isPlaySounds_.begin(); it != isPlaySounds_.end(); it++) {
			if (it->first == handle) {
				it->second->Stop(0);
				it = isPlaySounds_.erase(it);
				break;
			}
		}
	}

	// ソース音声を作成
	IXAudio2SourceVoice* sourceVoices = nullptr;
	result = pXAudio2_->CreateSourceVoice(&sourceVoices, &soundDatas_[handle].wfex);
	assert(SUCCEEDED(result));

	// ソース音声にバッファを送信
	XAUDIO2_BUFFER buf{};
	buf.pAudioData = soundDatas_[handle].pBuffer;
	buf.AudioBytes = soundDatas_[handle].bufferSize;
	buf.Flags = XAUDIO2_END_OF_STREAM;
	if (isLoop) buf.LoopCount = XAUDIO2_LOOP_INFINITE;
	result = sourceVoices->SubmitSourceBuffer(&buf);

	// 再生
	result = sourceVoices->SetVolume(soundDatas_[handle].volume);
	result = sourceVoices->Start(0);
	assert(SUCCEEDED(result));

	isPlaySounds_.emplace(handle, sourceVoices);
}

void Sound::Stop(uint16_t handle)
{
	for (auto it = isPlaySounds_.begin(); it != isPlaySounds_.end(); it++) {
		if (it->first == handle) {
			it->second->Stop(0);
			it = isPlaySounds_.erase(it);
			break;
		}
	}
}

void Sound::Release()
{
	for (auto& it : isPlaySounds_) {
		it.second->Stop(0);
	}

	isPlaySounds_.clear();
	soundDatas_.clear();
	soundHandles_.clear();
	soundCounter_ = 0;
}
