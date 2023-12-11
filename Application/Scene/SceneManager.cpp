#include "SceneManager.h"
#include "DX12Cmd.h"
#include "PipelineManager.h"
#include "ImGuiManager.h"

#include "GameScene.h"

// インスタンス取得
SceneManager* SceneManager::GetInstance()
{
	// 関数内static変数として宣言
	static SceneManager instance;
	return &instance;
}

// コンストラクタ
SceneManager::SceneManager() :
#pragma region 初期化リスト
	// キーボード入力クラス
	key_(nullptr),

	// 現在のシーン
	nowScene_(nullptr),

	// ポストエフェクト
	gaussianPostEffect_(nullptr),
	highLumiPostEffect_(nullptr),
	bloomPostEffect_(nullptr),

	// ポストエフェクトの種類
	postEffectType_(PostEffectType::NORMAL)
#pragma endregion
{
	// インスタンス取得
	key_ = Key::GetInstance();

	// 最初のシーン
	nowScene_ = std::make_unique<GameScene>();
	nowScene_->Initialize();

	gaussianPostEffect_ = std::make_unique<PostEffect>();
	highLumiPostEffect_ = std::make_unique<PostEffect>();
	bloomPostEffect_ = std::make_unique<PostEffect>();

	PipelineManager::GetInstance();
}

// デストラクタ
SceneManager::~SceneManager() {

}

void SceneManager::ChangeScene(SCENE changeSceneNum)
{
	isChangeScene_ = true;

	switch (changeSceneNum)
	{
	case SCENE::GAME:
		nowScene_ = std::make_unique<GameScene>();
		nowScene_->Initialize();
		break;
	}
}

void SceneManager::SceneTransition(SCENE scene)
{
	nextScene_ = scene;

	isTransition_ = true;
}

// 更新処理
void SceneManager::Update() {

	//if (key_->TriggerKey(DIK_1)) postEffectType_ = PostEffectType::NORMAL;
	//if (key_->TriggerKey(DIK_2)) postEffectType_ = PostEffectType::BLUR;
	//if (key_->TriggerKey(DIK_3)) postEffectType_ = PostEffectType::BLOOM;

	ImGuiManager::GetInstance()->Begin();

	nowScene_->Update();

	if (isTransition_)
	{
		isTransition_ = false;
		ChangeScene(TRANSITION);
	}
}

// 描画処理
void SceneManager::Draw()
{
	if (PostEffectType::NORMAL == postEffectType_) {
		// --描画前処理-- //
		DX12Cmd::GetInstance()->PreDraw();

		nowScene_->Draw();

		ImGuiManager::GetInstance()->End();
		ImGuiManager::GetInstance()->Draw();

		// --描画後処理-- //
		DX12Cmd::GetInstance()->PostDraw();
	}

	else if (PostEffectType::BLUR == postEffectType_) {
		gaussianPostEffect_->PreDraw();

		nowScene_->Draw();

		gaussianPostEffect_->PostDraw();

		// --描画前処理-- //
		DX12Cmd::GetInstance()->PreDraw();

		PipelineManager::GetInstance()->PreDraw("Gaussian");

		gaussianPostEffect_->Draw();

		// --描画後処理-- //
		DX12Cmd::GetInstance()->PostDraw();
	}

	else if (PostEffectType::BLOOM == postEffectType_) {
		highLumiPostEffect_->PreDraw();

		nowScene_->Draw();

		highLumiPostEffect_->PostDraw();

		bloomPostEffect_->PreDraw();

		PipelineManager::GetInstance()->PreDraw("HighLumi");
		highLumiPostEffect_->Draw();

		bloomPostEffect_->PostDraw();

		// --描画前処理-- //
		DX12Cmd::GetInstance()->PreDraw();

		PipelineManager::GetInstance()->PreDraw("Bloom");

		bloomPostEffect_->Draw();

		// --描画後処理-- //
		DX12Cmd::GetInstance()->PostDraw();
	}
}
