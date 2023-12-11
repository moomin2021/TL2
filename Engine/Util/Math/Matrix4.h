#pragma once

class Vector3;

// --Matrix4構造体-- //
struct Matrix4 {
	float m[4][4];
};

// --単位行列を求める-- //
Matrix4 Matrix4Identity();

// 逆行列
Matrix4 Matrix4Inverse(const Matrix4& m);

// --拡大縮小行列の設定-- //
Matrix4 Matrix4Scale(const Vector3& s);

// --回転行列の設定-- //
Matrix4 Matrix4RotateX(float angle);
Matrix4 Matrix4RotateY(float angle);
Matrix4 Matrix4RotateZ(float angle);

// --平行移動行列の作成-- //
Matrix4 Matrix4Translate(const Vector3& t);

// --座標変換（ベクトルと行列の掛け算をする）-- //
Vector3 Matrix4Transform(const Vector3& v, const Matrix4& m);

// --代入演算子オーバーロード-- //
Matrix4& operator*=(Matrix4& m1, const Matrix4& m2);

// --2項演算子オーバーロード-- //
const Matrix4 operator*(const Matrix4& m1, const Matrix4& m2);
const Vector3 operator*(const Vector3& v, const Matrix4& m);
