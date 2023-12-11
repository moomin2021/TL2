#pragma once
#include "Vector3.h"
#include "Matrix4.h"

struct Quaternion {
	float x;
	float y;
	float z;
	float w;

	Quaternion(float x, float y, float z, float w);

	// 単項演算子オーバーロード
	Quaternion operator+() const;
	Quaternion operator-() const;

	// 代入演算子オーバーロード
	Quaternion& operator+=(const Quaternion& q);
	Quaternion& operator-=(const Quaternion& q);
	Quaternion& operator*=(float s);
	Quaternion& operator/=(float s);
};

// 2項演算子オーバーロード
// ※いろんな引数のパターンに対応(引数の順序)するため、以下のように準備している
const Quaternion operator+(const Quaternion& q1, const Quaternion& q2);
const Quaternion operator-(const Quaternion& q1, const Quaternion& q2);
const Quaternion operator*(const Quaternion& q, float s);
const Quaternion operator*(float s, const Quaternion& q);
const Quaternion operator/(const Quaternion& q, float s);

// Quaternionの積
Quaternion Multiply(const Quaternion& lhs, const Quaternion& rhs);

// 単位Quaternionを返す
Quaternion IdentityQuaternion();

// 共役Quaternionを返す
Quaternion Conjugate(const Quaternion& q);

// Quaternionのnormを返す
float Norm(const Quaternion& q);

// 正規化したQuaternionを返す
Quaternion Normalise(const Quaternion& q);

// 逆Quaternionを返す
Quaternion Inverse(const Quaternion& q);

// 任意軸回転を表すQuaternionの生成
Quaternion MakeAxisAngle(const Vector3& axis, float angle);

// ベクトルをQuaternionで回転させた結果のベクトルを求める
Vector3 RotateVector(const Vector3& vector, const Quaternion& quaternion);

// Quaternionから回転行列を求める
Matrix4 MakeRotateMatrix(const Quaternion& q);

// 球面線形補間
Quaternion Slerp(const Quaternion& q0, const Quaternion& q1, float t);

// uからvへの回転の生成
Quaternion DirectionToDirection(const Vector3& u, const Vector3& v);
