#pragma once
#include <d3dx12.h>
#include <wrl.h>

#include <vector>

// シェーダーの種類
enum ShaderType {
	VS,
	PS,
	GS
};

class PipelineObj
{
private:
	// エイリアステンプレート
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

public:
	enum BLENDMODE {
		NONE,
		ALPHA,
		ADD,
		SUB
	};

#pragma region メンバ変数
private:
	// シェーダーオブジェクト
	ComPtr<ID3DBlob> vsBlob_ = nullptr;	// 頂点シェーダオブジェクト
	ComPtr<ID3DBlob> psBlob_ = nullptr;	// ピクセルシェーダオブジェクト
	ComPtr<ID3DBlob> gsBlob_ = nullptr;	// ジオメトリシェーダオブジェクト
	ComPtr<ID3DBlob> errorBlob_;	// エラーオブジェクト

	// 頂点レイアウト
	std::vector<D3D12_INPUT_ELEMENT_DESC> inputLayout_;

	// ルートパラメータ
	std::vector<CD3DX12_ROOT_PARAMETER> rootParams_;

	// ルートシグネチャ
	ComPtr<ID3D12RootSignature> rootSignature_;

	// パイプラインステート
	ComPtr<ID3D12PipelineState> pipelineState_;

	// デスクリプタレンジ
	std::vector <CD3DX12_DESCRIPTOR_RANGE> descRangeSRV_;
#pragma endregion

#pragma region メンバ関数
public:
	/// <summary>
	/// コンストラクタ
	/// </summary>
	PipelineObj();

	/// <summary>
	/// シェーダー読み込み
	/// </summary>
	/// <param name="fileName"> シェーダーのファイル名 </param>
	/// <param name="shaderType"> シェーダーの種類 </param>
	void LoadShader(std::string fileName, ShaderType shaderType);

	/// <summary>
	/// インプットレイアウトに新規設定を追加
	/// </summary>
	/// <param name="semanticName"> セマンティック名 </param>
	/// <param name="format"> リソースデータ形式(要素数とビット数を表す) </param>
	/// <param name="index"> 同じセマンティック名が複数あるとき使うインデックス </param>
	void AddInputLayout(const char* semanticName, DXGI_FORMAT format, uint16_t index = 0);

	/// <summary>
	/// ルートパラメータ作成
	/// </summary>
	/// <param name="texRegisterNum"> テクスチャレジスタ数 </param>
	/// <param name="constBuffNum"> 定数バッファの数 </param>
	void CreateRootParams(uint16_t texRegisterNum, uint16_t constBuffNum);

	/// <summary>
	/// パイプライン作成
	/// </summary>
	/// <param name="renderTargetNum"> レンダーターゲットの数 </param>
	void CreatePipeline(uint16_t renderTargetNum,BLENDMODE blendMode = NONE, D3D12_PRIMITIVE_TOPOLOGY_TYPE primitiveType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE, bool isDepth = true);

private:
	/// <summary>
	/// シェーダーの読み込みとコンパイル
	/// </summary>
	/// <param name="fileName"> シェーダーのファイル名 </param>
	/// <param name="target"> シェーダーモデル指定 </param>
	/// <param name="shaderBlob"> シェーダーオブジェクト </param>
	void ShaderCompileFromFile(std::string fileName, LPCSTR target, ID3D10Blob** shaderBlob);
#pragma endregion

#pragma region ゲッター
public:
	/// <summary>
	/// パイプラインステート取得
	/// </summary>
	/// <returns> パイプラインステート </returns>
	ID3D12PipelineState* GetPipelineState() { return pipelineState_.Get(); }

	/// <summary>
	/// ルートシグネチャを取得
	/// </summary>
	/// <returns> ルートシグネチャ </returns>
	ID3D12RootSignature* GetRootSignature() { return rootSignature_.Get(); }
#pragma endregion
};
