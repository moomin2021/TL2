#include "Matrix4.h"
#include "Vector3.h"
#include <cmath>

// --単位行列を求める-- //
Matrix4 Matrix4Identity() {
	static const Matrix4 result{
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	};

	return result;
}

Matrix4 Matrix4Inverse(const Matrix4& m)
{
	float sweepMat[4][8] = {};
	float tmepNum = 0;

	// 掃き出し行列の初期化
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			// 引数でもらった行列（左 4 * 4）
			sweepMat[i][j] = m.m[i][j];

			// 単位行列（右 4 * 4）
			sweepMat[i][j + 4] = (i == j) ? 1.0f : 0.0f;
		}
	}

	// 掃き出し法
	for (int i = 0; i < 4; i++)
	{
		// 最大成分を探索する
		float max = fabsf(sweepMat[i][i]);
		int maxIndex = i;
		for (int j = i + 1; j < 4; j++)
		{
			if (max < fabsf(sweepMat[i][i]))
			{
				max = fabsf(sweepMat[i][i]);
				maxIndex = j;
			}
		}
		// 逆行列求めるかどうか
		if (fabsf(sweepMat[maxIndex][i]) <= 1.e-50)
		{
			// 求めれない場合は単位行列を返す
			Matrix4 identity = Matrix4Identity();
			return identity;
		}

		// 対象となる行列の対角成分を1にする
		tmepNum = 1 / sweepMat[i][i];
		for (int j = 0; j < 8; j++)
		{
			sweepMat[i][j] *= tmepNum;
		}

		// 対象となる行列の対角成分以外を0にするため
		for (int j = 0; j < 4; j++)
		{
			if (i == j) continue;

			tmepNum = -sweepMat[j][i];
			for (int k = 0; k < 8; k++)
			{
				sweepMat[j][k] += sweepMat[i][k] * tmepNum;
			}
		}
	}

	// 逆行列を返す
	Matrix4 inverseMat = Matrix4Identity();
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			inverseMat.m[i][j] = sweepMat[i][j + 4];
		}
	}

	return inverseMat;
}

// --拡大縮小行列を求める-- //
Matrix4 Matrix4Scale(const Vector3& s) {
	Matrix4 result{
		s.x, 0.0f, 0.0f, 0.0f,
		0.0f, s.y, 0.0f, 0.0f,
		0.0f, 0.0f, s.z, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	};

	return result;
}

// --X軸まわりの回転行列を求める-- //
Matrix4 Matrix4RotateX(float angle) {
	float sin = std::sin(angle);
	float cos = std::cos(angle);

	Matrix4 result{
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, cos, sin, 0.0f,
		0.0f, -sin, cos, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	};

	return result;
}

// --Y軸まわりの回転行列を求める-- //
Matrix4 Matrix4RotateY(float angle) {
	float sin = std::sin(angle);
	float cos = std::cos(angle);

	Matrix4 result{
		cos, 0.0f, -sin, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		sin, 0.0f, cos, 0.0f,
		0.0f, 0.0, 0.0f, 1.0f
	};

	return result;
}

// --Z軸まわりの回転行列を求める-- //
Matrix4 Matrix4RotateZ(float angle) {
	float sin = std::sin(angle);
	float cos = std::cos(angle);

	Matrix4 result{
		cos, sin, 0.0f, 0.0f,
		-sin, cos, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0, 0.0f, 1.0f
	};

	return result;
}

// --平行移動行列を求める-- //
Matrix4 Matrix4Translate(const Vector3& t) {
	Matrix4 result{
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		t.x, t.y, t.z, 1.0f
	};

	return result;
}

// --座標変換（ベクトルと行列の掛け算）を行うtransform関数を作成する。（透視変換にも対応している）-- //
Vector3 Matrix4Transform(const Vector3& v, const Matrix4& m) {
	float w = v.x * m.m[0][3] + v.y * m.m[1][3] + v.z * m.m[2][3] + m.m[3][3];

	Vector3 result{
		(v.x * m.m[0][0] + v.y * m.m[1][0] + v.z * m.m[2][0] + m.m[3][0]) / w,
		(v.x * m.m[0][1] + v.y * m.m[1][1] + v.z * m.m[2][1] + m.m[3][1]) / w,
		(v.x * m.m[0][2] + v.y * m.m[1][2] + v.z * m.m[2][2] + m.m[3][2]) / w
	};

	return result;
}

// --代入演算子 *= のオーバーロード関数（行列と行列の積）-- //
Matrix4& operator*=(Matrix4& m1, const Matrix4& m2) {
	Matrix4 result{ 0 };

	for (size_t i = 0; i < 4; i++) {
		for (size_t j = 0; j < 4; j++) {
			for (size_t k = 0; k < 4; k++) {
				result.m[i][j] += m1.m[i][k] * m2.m[k][j];
			}
		}
	}
	m1 = result;
	return m1;
}

// --2項演算子*のオーバーロード関数（行列と行列の積）-- //
const Matrix4 operator*(const Matrix4& m1, const Matrix4& m2) {
	Matrix4 result = m1;
	return result *= m2;
}

// --2項演算子*のオーバーロード関数（ベクトルと行列の積）-- //
const Vector3 operator*(const Vector3& v, const Matrix4& m) {
	return Matrix4Transform(v, m);
}
