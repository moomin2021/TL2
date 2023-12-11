#include "PipelineObj.h"
#include "DX12Cmd.h"
#include "Util.h"

#include <d3dcompiler.h>
#include <d3d12.h>
#include <cassert>

PipelineObj::PipelineObj()
{
}

void PipelineObj::LoadShader(std::string fileName, ShaderType shaderType)
{
	switch (shaderType)
	{
	// 頂点シェーダー
	case VS : ShaderCompileFromFile(fileName, "vs_5_0", &vsBlob_);
		break;
	// ピクセルシェーダー
	case PS : ShaderCompileFromFile(fileName, "ps_5_0", &psBlob_);
		break;
		// ジオメトリシェーダー
	case GS: ShaderCompileFromFile(fileName, "gs_5_0", &gsBlob_);
		break;
	}
}

void PipelineObj::AddInputLayout(const char* semanticName, DXGI_FORMAT format, uint16_t index)
{
	// インプットレイアウトを設定＆追加
	inputLayout_.emplace_back(D3D12_INPUT_ELEMENT_DESC{
		semanticName,								// セマンティック名
		index,										// 同じセマンティック名が複数あるときに使うインデックス（0でよい）
		format,										// 要素数とビット数を表す
		0,											// 入力スロットインデックス（0でよい）
		D3D12_APPEND_ALIGNED_ELEMENT,				// データのオフセット値（D3D12_APPEND_ALIGNED_ELEMENTだと自動設定）
		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,	// 入力データの種別（標準はD3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA）
		0											// 一度に描画するインスタンス数（0でよい）
	});
}

void PipelineObj::CreateRootParams(uint16_t texRegisterNum, uint16_t constBuffNum)
{
	// ルートパラメータの生成カウンター
	uint16_t counter = 0;

	rootParams_.resize(texRegisterNum + constBuffNum);
	descRangeSRV_.resize(texRegisterNum);

	for (size_t i = 0; i < texRegisterNum; i++) {
		descRangeSRV_[i].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, static_cast<UINT>(i));// t i レジスタ

		// ルートパラメータ設定
		rootParams_[counter].InitAsDescriptorTable(1, &descRangeSRV_[i], D3D12_SHADER_VISIBILITY_ALL);

		// ルートパラメータを生成＆設定し終わったためインクリメント
		counter++;
	}

	for (size_t i = 0; i < constBuffNum; i++) {
		// ルートパラメータ設定
		rootParams_[counter].InitAsConstantBufferView(static_cast<UINT>(i), 0, D3D12_SHADER_VISIBILITY_ALL);

		// ルートパラメータを生成＆設定し終わったためインクリメント
		counter++;
	}
}

void PipelineObj::CreatePipeline(uint16_t renderTargetNum, BLENDMODE blendMode, D3D12_PRIMITIVE_TOPOLOGY_TYPE primitiveType, bool isDepth) {
	// 関数が成功したかどうかを判別する用変数
	HRESULT result;

	// デバイス取得
	ID3D12Device* device = DX12Cmd::GetInstance()->GetDevice();

	// グラフィックスパイプライン設定
	D3D12_GRAPHICS_PIPELINE_STATE_DESC pipelineDesc{};

	// シェーダーの設定
	pipelineDesc.VS.pShaderBytecode = vsBlob_->GetBufferPointer();
	pipelineDesc.VS.BytecodeLength = vsBlob_->GetBufferSize();
	pipelineDesc.PS.pShaderBytecode = psBlob_->GetBufferPointer();
	pipelineDesc.PS.BytecodeLength = psBlob_->GetBufferSize();

	if (gsBlob_ != nullptr) {
		pipelineDesc.GS.pShaderBytecode = gsBlob_->GetBufferPointer();
		pipelineDesc.GS.BytecodeLength = gsBlob_->GetBufferSize();
	}

	// サンプルマスクの設定
	pipelineDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK; // 標準設定

	// ラスタライザの設定
	pipelineDesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;	// カリングしない
	pipelineDesc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;	// ポリゴン内塗りつぶし
	pipelineDesc.RasterizerState.DepthClipEnable = true;			// 深度クリッピングを有効に

	// デプスステンシルステート
	pipelineDesc.DepthStencilState.DepthEnable = true;							// 深度テストを行う
	pipelineDesc.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
	if (isDepth) pipelineDesc.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;	// 書き込み許可
	pipelineDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS;		// 小さければ合格
	pipelineDesc.DSVFormat = DXGI_FORMAT_D32_FLOAT;								// 深度値フォーマット

	// レンダーターゲットのブレンド設定
	D3D12_RENDER_TARGET_BLEND_DESC blendDesc{};
	blendDesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;// RGBA全てのチャンネルを描画
	blendDesc.BlendEnable = true;
	blendDesc.BlendOpAlpha = D3D12_BLEND_OP_ADD;
	blendDesc.SrcBlendAlpha = D3D12_BLEND_ONE;
	blendDesc.DestBlendAlpha = D3D12_BLEND_ZERO;

	if (blendMode == NONE) {
		blendDesc.BlendOp = D3D12_BLEND_OP_ADD;
		blendDesc.SrcBlend = D3D12_BLEND_SRC_ALPHA;
		blendDesc.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
	}

	else if (blendMode == ALPHA) {
		blendDesc.BlendOp = D3D12_BLEND_OP_ADD;
		blendDesc.SrcBlend = D3D12_BLEND_SRC_ALPHA;
		blendDesc.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
	}

	else if (blendMode == ADD) {
		blendDesc.BlendOp = D3D12_BLEND_OP_ADD;
		blendDesc.SrcBlend = D3D12_BLEND_ONE;
		blendDesc.DestBlend = D3D12_BLEND_ONE;
	}

	else if (blendMode == SUB) {
		blendDesc.BlendOpAlpha = D3D12_BLEND_OP_REV_SUBTRACT;
		blendDesc.SrcBlendAlpha = D3D12_BLEND_ONE;
		blendDesc.DestBlendAlpha = D3D12_BLEND_ONE;
	}

	// ブレンドステートの設定
	for (size_t i = 0; i < renderTargetNum; i++) {
		pipelineDesc.BlendState.RenderTarget[i] = blendDesc;
	}

	// 頂点レイアウトの設定
	pipelineDesc.InputLayout.pInputElementDescs = inputLayout_.data();
	pipelineDesc.InputLayout.NumElements = static_cast<UINT>(inputLayout_.size());

	// 図形の形状設定
	pipelineDesc.PrimitiveTopologyType = primitiveType;

	// その他の設定
	pipelineDesc.NumRenderTargets = renderTargetNum;					// 描画対象は2つ
	for (size_t i = 0; i < renderTargetNum; i++) {
		pipelineDesc.RTVFormats[i] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;	// 0~255指定のRGBA
	}
	pipelineDesc.SampleDesc.Count = 1;									// 1ピクセルにつき1回サンプリング

	// テクスチャサンプラーの設定
	// スタティックサンプラー
	CD3DX12_STATIC_SAMPLER_DESC samplerDesc =
		CD3DX12_STATIC_SAMPLER_DESC(0, D3D12_FILTER_MIN_MAG_MIP_POINT);
	samplerDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
	samplerDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_BORDER;

	// ルートシグネチャの設定
	CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDesc;
	rootSignatureDesc.Init_1_0(
		static_cast<UINT>(rootParams_.size()), rootParams_.data(), 1, &samplerDesc,
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

	// ルートシグネチャのシリアライズ
	ComPtr<ID3DBlob> rootSigBlob = nullptr;
	result = D3DX12SerializeVersionedRootSignature(
		&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1_0,
		&rootSigBlob, &errorBlob_);
	assert(SUCCEEDED(result));
	result = device->CreateRootSignature(0, rootSigBlob->GetBufferPointer(), rootSigBlob->GetBufferSize(),
		IID_PPV_ARGS(&rootSignature_));
	assert(SUCCEEDED(result));

	// パイプラインにルートシグネチャをセット
	pipelineDesc.pRootSignature = rootSignature_.Get();

	// パイプランステートの生成
	result = device->CreateGraphicsPipelineState(&pipelineDesc, IID_PPV_ARGS(&pipelineState_));
	assert(SUCCEEDED(result));
}

void PipelineObj::ShaderCompileFromFile(std::string fileName, LPCSTR target, ID3D10Blob** shaderBlob)
{
	// 関数が成功したかどうかを判別する用変数
	HRESULT result;

	// 頂点シェーダの読み込みとコンパイル
	result = D3DCompileFromFile(
		Util::StringToWideChar(fileName).data(),		// シェーダファイル名
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,				// インクルード可能にする
		"main", target,									// エントリーポイント名、シェーダーモデル指定
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,// デバッグ用設定
		0,
		shaderBlob, errorBlob_.GetAddressOf());

	// エラーなら
	if (FAILED(result))
	{
		// errorBlobからエラー内容をstring型にコピー
		std::string error;
		error.resize(errorBlob_->GetBufferSize());
		std::copy_n((char*)errorBlob_->GetBufferPointer(),
			errorBlob_->GetBufferSize(),
			error.begin());
		error += "\n";

		// エラー内容を出力ウィンドウに表示
		OutputDebugStringA(error.c_str());
		assert(0);
	}
}
