#pragma once
#include "BaseScene.h"
#include "Sprite.h"
#include "Camera.h"

#include <memory>

class TransitionScene : public BaseScene
{
#pragma region メンバ変数
private:
	std::unique_ptr<Camera> camera_ = nullptr;

	uint16_t maxTimr_ = 60;
	uint16_t time_ = 0;

	std::unique_ptr<Sprite> sLoad_ = nullptr;

	uint16_t loadHandle_ = 0;
#pragma endregion

#pragma region メンバ関数
public:
	// コンストラクタ
	TransitionScene();

	// デストラクタ
	~TransitionScene();

	// 初期化処理
	void Initialize();

	// 更新処理
	void Update();

	// 描画処理
	void Draw();

	// 衝突時処理
	void OnCollision();

	// 行列更新
	void MatUpdate();
#pragma endregion
};
