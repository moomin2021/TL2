#pragma once
#include "BaseScene.h"// -> 各シーンのベース
#include "Key.h"// -> キーボード入力
#include "PostEffect.h"

#include <memory>

// シーン列挙型
enum SCENE {
	GAME,
	TRANSITION,
};

enum PostEffectType {
	NORMAL,
	BLUR,
	BLOOM
};

class SceneManager {
	// --メンバ変数-- //
public:

private:
	// キーボード入力クラス
	Key* key_;

	// 現在のシーン
	std::unique_ptr<BaseScene> nowScene_;

	SCENE nextScene_ = GAME;

	// ポストエフェクト
	std::unique_ptr<PostEffect> gaussianPostEffect_;
	std::unique_ptr<PostEffect> highLumiPostEffect_;
	std::unique_ptr<PostEffect> bloomPostEffect_;

	// ポストエフェクトの種類
	PostEffectType postEffectType_;

	// シーンを変更中か
	bool isChangeScene_ = false;

	// ゲームを終了
	bool isEnd_ = false;

	// シーン遷移フラグ
	bool isTransition_ = false;

	// --メンバ関数-- //
public:
	// インスタンス取得
	static SceneManager* GetInstance();

	// 更新処理
	void Update();

	// 描画処理
	void Draw();

	// シーン切り替え
	void ChangeScene(SCENE changeSceneNum);

	// シーン遷移
	void SceneTransition(SCENE scene);

	// 次のシーンを取得
	SCENE GetNextScene() { return nextScene_; }

private:
	// コンストラクタ
	SceneManager();

	// デストラクタ
	~SceneManager();

	// --禁止-- //
public:
	// コピーコンストラクタ
	SceneManager(const SceneManager& instance) = delete;

	// 代入演算子
	SceneManager& operator=(const SceneManager& instance) = delete;

	void SetIsEnd(bool frag) { isEnd_ = frag; }
	bool GetIsEnd() { return isEnd_; }
};
