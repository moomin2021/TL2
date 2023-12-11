#pragma once
class Vector3;

struct float4
{
	// x成分、y成分、z成分、w成分
	float x, y, z, w;

	// コンストラクタ
	float4();// -----------------------------------> 引数なし
	float4(float x, float y, float z, float w);// -> 引数あり
	float4(const Vector3& vec, float a);

	// 単項演算子オーバーロード
	float4 operator+() const;
	float4 operator-() const;

	// 代入演算子オーバーロード
	float4& operator+=(const float4& f);
	float4& operator-=(const float4& f);
	float4& operator*=(float s);
	float4& operator/=(float s);
};

// 2項演算子オーバーロード
// ※いろんな引数のパターンに対応(引数の順序)するため、以下のように準備している
const float4 operator+(const float4& f1, const float4& f2);
const float4 operator-(const float4& f1, const float4& f2);
const float4 operator*(const float4& f, float s);
const float4 operator*(float s, const float4& f);
const float4 operator/(const float4& f, float s);
