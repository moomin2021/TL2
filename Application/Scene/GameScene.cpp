#include "GameScene.h"
#include "Texture.h"
#include "PipelineManager.h"

void GameScene::Initialize()
{
#pragma region カメラ
	camera_ = std::make_unique<Camera>();
	Sprite::SetCamera(camera_.get());
#pragma endregion

#pragma region スプライト
	sprite_ = std::make_unique<Sprite>();
#pragma endregion

#pragma region テクスチャ読み込み
	texture_ = LoadTexture("Sprite/GameOver.dds");
#pragma endregion
}

void GameScene::Update()
{
	// 衝突時処理
	OnCollision();

	// 行列更新処理
	MatUpdate();
}

void GameScene::Draw()
{
	PipelineManager::PreDraw("Sprite");

	sprite_->Draw(texture_);
}

GameScene::~GameScene()
{
}

void GameScene::OnCollision()
{
}

void GameScene::MatUpdate()
{
	sprite_->MatUpdate();
}