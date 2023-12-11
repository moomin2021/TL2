#include "Quaternion.h"
#include <math.h>
#include <cmath>

Quaternion::Quaternion(float x, float y, float z, float w)
{
	this->x = x;
	this->y = y;
	this->z = z;
	this->w = w;
}

Quaternion Quaternion::operator+() const
{
	return *this;
}

Quaternion Quaternion::operator-() const
{
	return Quaternion(-x, -y, -z, -w);
}

Quaternion& Quaternion::operator+=(const Quaternion& q)
{
	x += q.x;
	y += q.y;
	z += q.z;
	w += q.w;
	return *this;
}

Quaternion& Quaternion::operator-=(const Quaternion& q)
{
	x -= q.x;
	y -= q.y;
	z -= q.z;
	w -= q.w;
	return *this;
}

Quaternion& Quaternion::operator*=(float s)
{
	x *= s;
	y *= s;
	z *= s;
	w *= s;
	return *this;
}

Quaternion& Quaternion::operator/=(float s)
{
	x /= s;
	y /= s;
	z /= s;
	w /= s;
	return *this;
}

const Quaternion operator+(const Quaternion& q1, const Quaternion& q2)
{
	Quaternion tmp(q1);
	return tmp += q2;
}

const Quaternion operator-(const Quaternion& q1, const Quaternion& q2)
{
	Quaternion tmp(q1);
	return tmp -= q2;
}

const Quaternion operator*(const Quaternion& q, float s)
{
	Quaternion tmp(q);
	return tmp *= s;
}

const Quaternion operator*(float s, const Quaternion& q)
{
	return q * s;
}

const Quaternion operator/(const Quaternion& q, float s)
{
	Quaternion tmp(q);
	return tmp /= s;
}

Quaternion Multiply(const Quaternion& lhs, const Quaternion& rhs)
{
	return Quaternion{
		lhs.x * rhs.w + lhs.w * rhs.x - lhs.z * rhs.y + lhs.y * rhs.z,
		lhs.y * rhs.w + lhs.z * rhs.x + lhs.w * rhs.y - lhs.x * rhs.z,
		lhs.z * rhs.w - lhs.y * rhs.x + lhs.x * rhs.y + lhs.w * rhs.z,
		lhs.w * rhs.w - lhs.x * rhs.x - lhs.y * rhs.y - lhs.z * rhs.z
	};
}

Quaternion IdentityQuaternion()
{
	return Quaternion{ 0.0f, 0.0f, 0.0f, 1.0f };
}

Quaternion Conjugate(const Quaternion& q)
{
	return Quaternion{ -q.x, -q.y, -q.z, q.w };
}

float Norm(const Quaternion& q)
{
	return sqrtf(q.x * q.x + q.y * q.y + q.z * q.z + q.w * q.w);
}

Quaternion Normalise(const Quaternion& q)
{
	Quaternion resultQ = q;
	float norm = Norm(q);
	if (norm != 0) {
		resultQ.x /= norm;
		resultQ.y /= norm;
		resultQ.z /= norm;
		resultQ.w /= norm;
	}
	return resultQ;
}

Quaternion Inverse(const Quaternion& q)
{
	Quaternion resultQ = q;
	resultQ = Conjugate(resultQ);
	resultQ = resultQ / (Norm(q) * Norm(q));
	return resultQ;
}

Quaternion MakeAxisAngle(const Vector3& axis, float angle)
{
	return Quaternion(sinf(angle / 2.0f) * axis.x, sinf(angle / 2.0f) * axis.y, sinf(angle / 2.0f) * axis.z, cosf(angle / 2.0f));
}

Vector3 RotateVector(const Vector3& vector, const Quaternion& quaternion)
{
	Quaternion r = { vector.x, vector.y, vector.z, 0.0f };

	Quaternion cQ = Conjugate(quaternion);

	Quaternion result = Multiply(quaternion, r);
	result = Multiply(result, cQ);
	return Vector3(result.x, result.y, result.z);
}

Matrix4 MakeRotateMatrix(const Quaternion& q)
{
	return Matrix4{ q.w * q.w + q.x * q.x - q.y * q.y - q.z * q.z, (q.x * q.y + q.w * q.z) * 2.0f, (q.x * q.z - q.w * q.y) * 2.0f, 0.0f,
		(q.x * q.y - q.w * q.z) * 2.0f, q.w * q.w - q.x * q.x + q.y * q.y - q.z * q.z, (q.y * q.z + q.w * q.x) * 2.0f, 0.0f,
		(q.x * q.z + q.w * q.y) * 2.0f, (q.y * q.z - q.w * q.x) * 2.0f, q.w * q.w - q.x * q.x - q.y * q.y + q.z * q.z, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f };
}

Quaternion Slerp(const Quaternion& q0, const Quaternion& q1, float t)
{
	Quaternion t0 = q0;
	Quaternion t1 = q1;

	float dot = t0.x * t1.x + t0.y * t1.y + t0.z * t1.z + t0.w * t1.w;

	if (dot < 0) {
		t0 = -t0;
		dot = -dot;
	}

	float theta = std::acos(dot);

	float scale0 = sinf((1 - t) * theta) / sinf(theta);
	float scale1 = sinf(t * theta) / sinf(theta);
	if (dot >= 1.0f - 0.0005f) {
		return (1.0f - t) * t0 + t * t1;
	}
	return scale0 * t0 + scale1 * t1;
}

Quaternion DirectionToDirection(const Vector3& u, const Vector3& v)
{
	Vector3 t0 = u;
	Vector3 t1 = v;

	t0 = t0.normalize();
	t1 = t1.normalize();

	// uとvを正規化して内積を求める
	float dot = t0.dot(t1);

	// u, vの外積をとる
	Vector3 cross = t0.cross(t1);

	// uとvが単位ベクトルであっても、外積が単位ベクトルとは限らないので正規化は必須
	Vector3 axis = cross.normalize();

	// 単位ベクトルで内積を取っているのでacosで角度を求める
	float theta = acosf(dot);

	// axisとthetaで任意軸回転を作って返す
	return MakeAxisAngle(axis, theta);
}
