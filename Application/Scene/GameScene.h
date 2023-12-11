#pragma once
#include "BaseScene.h"
#include "Camera.h"
#include "Sprite.h"

#include <memory>

class GameScene : public BaseScene
{
#pragma region メンバ変数
private:
	// カメラ
	std::unique_ptr<Camera> camera_ = nullptr;

	// スプライト
	std::unique_ptr<Sprite> sprite_ = nullptr;

	// テクスチャ
	uint16_t texture_ = 0;
#pragma endregion

#pragma region メンバ関数
public:
	// 初期化
	void Initialize();

	// 更新処理
	void Update();

	// 描画処理
	void Draw();

	GameScene() {}
	~GameScene();

private:
	// 衝突時処理
	void OnCollision();

	// 行列更新処理
	void MatUpdate();
#pragma endregion
};