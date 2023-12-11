#pragma once
#include "PipelineObj.h"

#include <map>
#include <memory>
#include <string>

class PipelineManager
{
#pragma region メンバ変数
private:
	static std::map<std::string, std::unique_ptr<PipelineObj>> pipelineObj_;
#pragma endregion

#pragma region メンバ関数
public:
	/// <summary>
	/// インスタンス取得
	/// </summary>
	/// <returns> インスタンス </returns>
	static PipelineManager* GetInstance();


	/// <summary>
	/// 描画前処理
	/// </summary>
	/// <param name="pipelineName"> パイプライン名前 </param>
	static void PreDraw(std::string pipelineName, D3D_PRIMITIVE_TOPOLOGY primitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST, bool isDescHeap = true);

private:
	/// <summary>
	/// コンストラクタ
	/// </summary>
	PipelineManager();

	/// <summary>
	/// パイプラインを追加
	/// </summary>
	/// <param name="pipelineName"> パイプラインの名前 </param>
	void AddPipeline(std::string pipelineName);
#pragma endregion

#pragma region 特殊関数
	// 禁止
	PipelineManager(const PipelineManager&) = delete;				// コピーコンストラクタ禁止
	PipelineManager& operator = (const PipelineManager&) = delete;	// コピー代入演算子禁止
#pragma endregion
};
