#include "Util.h"

#include <Windows.h>
#include <chrono>
#include <random>

using namespace std::chrono;

int Util::Clamp(int value, int max, int min)
{
	// 値が最大値を上回っていたら最大値を返す
	if (value >= max) return max;

	// 値が最小値を下回っていたら最小値を返す
	if (value <= min) return min;

	// どちらにも当てはまらなかったら値をそのまま返す
	return value;
}

float Util::Clamp(float value, float max, float min)
{
	// 値が最大値を上回っていたら最大値を返す
	if (value >= max) return max;

	// 値が最小値を下回っていたら最小値を返す
	if (value <= min) return min;

	// どちらにも当てはまらなかったら値をそのまま返す
	return value;
}

float Util::Max(float value0, float value1)
{
	if (value0 > value1) return value0;
	return value1;
}

float Util::Min(float value0, float value1)
{
	if (value0 < value1) return value0;
	return value1;
}

float Util::Radian2Degree(float radian)
{
	return radian * 180.0f / PI;
}

float Util::Degree2Radian(float degree)
{
	return degree * PI / 180.0f;
}

std::vector<wchar_t> Util::StringToWideChar(const std::string& str)
{
	// 最終的に返す用変数
	std::vector<wchar_t> wchar;
	wchar.resize(128);// -> サイズを設定

	// 変換
	MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, wchar.data(), static_cast<int>(wchar.size()));

	// 戻り値を返す
	return wchar;
}

uint16_t Util::GetRandomInt(uint16_t min, uint16_t max)
{
	std::random_device seed_gen;
	std::mt19937_64 engine(seed_gen());
	std::uniform_int_distribution<uint16_t> dist(min, max);
	return dist(engine);
}

float Util::GetRandomFloat(float min, float max) {
	std::random_device seed_gen;
	std::mt19937_64 engine(seed_gen());
	std::uniform_real_distribution<float> dist(min, max);
	return dist(engine);
}

uint64_t Util::GetTimeSec()
{
	return duration_cast<seconds>(system_clock::now().time_since_epoch()).count();
}

uint64_t Util::GetTimrMSec()
{
	return duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
}
