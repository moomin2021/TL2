#include "float4.h"
#include "Vector3.h"

// 引数なしのコンストラクタ
float4::float4() : x(0.0f), y(0.0f), z(0.0f), w(0.0f) {}

// 引数ありのコンストラクタ
float4::float4(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {}

float4::float4(const Vector3& vec, float a) : x(vec.x), y(vec.y), z(vec.z), w(a) {}

float4 float4::operator+() const
{
	return *this;
}

float4 float4::operator-() const
{
	return float4(-x, -y, -z, -w);
}

float4& float4::operator+=(const float4& f)
{
	x += f.x;
	y += f.y;
	z += f.z;
	w += f.w;
	return *this;
}

float4& float4::operator-=(const float4& f)
{
	x -= f.x;
	y -= f.y;
	z -= f.z;
	w -= f.w;
	return *this;
}

float4& float4::operator*=(float s)
{
	x *= s;
	y *= s;
	z *= s;
	w *= s;
	return *this;
}

float4& float4::operator/=(float s)
{
	x /= s;
	y /= s;
	z /= s;
	w /= s;
	return *this;
}

const float4 operator+(const float4& f1, const float4& f2)
{
	float4 tmp(f1);
	return tmp += f2;
}

const float4 operator-(const float4& f1, const float4& f2)
{
	float4 tmp(f1);
	return tmp += f2;
}

const float4 operator*(const float4& f, float s)
{
	float4 tmp(f);
	return tmp *= s;
}

const float4 operator*(float s, const float4& f)
{
	return f * s;
}

const float4 operator/(const float4& f, float s)
{
	float4 tmp(f);
	return tmp /= s;
}
