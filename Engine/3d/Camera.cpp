#include "Camera.h"
#include "Util.h"
#include "WinAPI.h"

#include <DirectXMath.h>

using namespace DirectX;

Camera::Camera()
{
	// ウィンドウサイズ取得
	float winWidth	= static_cast<float>(WinAPI::GetInstance()->GetWidth());	// 横幅
	float winHeight	= static_cast<float>(WinAPI::GetInstance()->GetHeight());	// 縦幅

	// 透視投影行列の計算
	XMMATRIX mat = XMMatrixPerspectiveFovLH(
		Util::Degree2Radian(45.0f),	// 上下画角45度
		winWidth / winHeight,		// アスペクト比（画面横幅/画面縦幅）
		0.1f, 3000.0f);				// 前端、奥端

	for (size_t i = 0; i < 4; i++) {
		for (size_t j = 0; j < 4; j++) {
			matPerspectivePro_.m[i][j] = mat.r[i].m128_f32[j];
		}
	}

#pragma region 平行投影計算
	// 単位行列を代入
	matOrthoGraphicPro_ = Matrix4Identity();

	// 画面サイズに合わせて計算
	matOrthoGraphicPro_.m[0][0] = 2.0f / winWidth;
	matOrthoGraphicPro_.m[1][1] = -2.0f / winHeight;

	// 画面半分移動させて画面左上を原点にする
	matOrthoGraphicPro_.m[3][0] = -1.0f;
	matOrthoGraphicPro_.m[3][1] = 1.0f;
#pragma endregion
}

void Camera::Update() {
	// ビュー行列更新
	UpdateMatView();
}

void Camera::UpdateMatView()
{
	// Z軸を求める
	Vector3 zAxis = target_ - eye_;
	zAxis.normalize();// 正規化

	// X軸を求める
	Vector3 xAxis = up_.cross(zAxis);
	xAxis.normalize();// 正規化

	// Y軸を求める
	Vector3 yAxis = zAxis.cross(xAxis);
	yAxis.normalize();// 正規化

	// 回転行列を求める
	Matrix4 matRot = {
		{
			{xAxis.x, xAxis.y, xAxis.z, 0.0f},
			{yAxis.x, yAxis.y, yAxis.z, 0.0f},
			{zAxis.x, zAxis.y, zAxis.z, 0.0f},
			{   0.0f,    0.0f,    0.0f, 1.0f}
		}
	};

	// 逆行列にする
	matRot = Matrix4Inverse(matRot);

	// 平行移動成分を求める
	Vector3 parallelMove = {};

	// 逆カメラ座標
	Vector3 reverseEye = -eye_;

	// 計算
	parallelMove = {
		reverseEye.dot(xAxis),
		reverseEye.dot(yAxis),
		reverseEye.dot(zAxis)
	};

	// ビュー行列完成
	matView_ = {
		{
			{matRot.m[0][0], matRot.m[0][1], matRot.m[0][2], 0.0f},
			{matRot.m[1][0], matRot.m[1][1], matRot.m[1][2], 0.0f},
			{matRot.m[2][0], matRot.m[2][1], matRot.m[2][2], 0.0f},
			{parallelMove.x, parallelMove.y, parallelMove.z, 1.0f}
		}
	};
}

void Camera::SetFovAngleY(float fovAngleY)
{
	// ウィンドウサイズ取得
	float winWidth = static_cast<float>(WinAPI::GetInstance()->GetWidth());	// 横幅
	float winHeight = static_cast<float>(WinAPI::GetInstance()->GetHeight());	// 縦幅

	// 透視投影行列の計算
	XMMATRIX mat = XMMatrixPerspectiveFovLH(
		Util::Degree2Radian(fovAngleY),	// 上下画角45度
		winWidth / winHeight,		// アスペクト比（画面横幅/画面縦幅）
		0.1f, 3000.0f);				// 前端、奥端

	for (size_t i = 0; i < 4; i++) {
		for (size_t j = 0; j < 4; j++) {
			matPerspectivePro_.m[i][j] = mat.r[i].m128_f32[j];
		}
	}
}
