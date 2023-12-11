#include "PostEffect.h"
#include "DX12Cmd.h"
#include "Texture.h"
#include "WinAPI.h"
#include "Util.h"

#include <DirectXMath.h>
#include <d3dx12.h>

using namespace DirectX;

const std::vector<float> PostEffect::clearColor_ = { 0.1f, 0.25, 0.5f, 0.0f };

PostEffect::PostEffect() :
#pragma region 初期化リスト
	// スプライトデータ
	scale_{ 1.0f, 1.0f },// 拡縮(画面の大きさに対して)

	// スプライトデータを変更したかどうか
	hasChanget_(true),

	// 定数バッファ
	constBuff_(nullptr),// 定数バッファ
	constMap_(nullptr),	// マッピング処理用

	// 頂点データ
	vertex_(4),
	vertexBuff_(nullptr),	// 頂点バッファ
	vertexMap_(nullptr),	// マッピング処理用
	vertexView_{},			// 頂点バッファビュー

	// インデックスデータ
	index_(6),
	indexBuff_(nullptr),// インデックスバッファ
	indexView_{},		// インデックスバッファビュー

	// テクスチャバッファ
	texBuff_(2),

	// SRV用デスクリプタヒープ
	descHeapSRV_(nullptr),

	// 深度バッファ
	depthBuff_(nullptr),

	// RTV用デスクリプタヒープ
	descHeapRTV_(nullptr),

	// DSV用デスクリプタヒープ
	descHeapDSV_(nullptr)
#pragma endregion
{
	// 頂点バッファの作成
	CreateVertexBuff();

	// インデックスバッファの作成
	CreateIndexBuff();

	// 定数バッファの作成
	CreateConstBuff();

	// テクスチャバッファ作成
	CreateTextureBuff();

	// SRV用デスクリプタヒープ作成
	CreateSRVDescHeap();

	// RTV用デスクリプタヒープ作成
	CreateRTVDescHeap();

	// 深度バッファ作成
	CreateDepthBuff();

	// DSV用デスクリプタヒープ作成
	CreateDSVDescHeap();
}

void PostEffect::Draw(bool isDescHeap)
{
	// デバイス取得
	ID3D12Device* device = DX12Cmd::GetInstance()->GetDevice();

	// コマンドリスト取得
	ID3D12GraphicsCommandList* cmdList = DX12Cmd::GetInstance()->GetCmdList();

	// スプライトデータの更新処理
	UpdateData();

	// デスクリプタヒープの配列をセットするコマンド
	if (isDescHeap) {
		std::vector<ID3D12DescriptorHeap*> ppHeaps = { descHeapSRV_.Get() };
		cmdList->SetDescriptorHeaps(1, ppHeaps.data());
	}

	// 指定されたSRVをルートパラメータ1番に設定
	CD3DX12_GPU_DESCRIPTOR_HANDLE descHandle0 = CD3DX12_GPU_DESCRIPTOR_HANDLE(
		descHeapSRV_->GetGPUDescriptorHandleForHeapStart(), 0,
		device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV));
	cmdList->SetGraphicsRootDescriptorTable(0, descHandle0);

	// 指定されたSRVをルートパラメータ2番に設定
	CD3DX12_GPU_DESCRIPTOR_HANDLE descHandle1 = CD3DX12_GPU_DESCRIPTOR_HANDLE(
		descHeapSRV_->GetGPUDescriptorHandleForHeapStart(), 1,
		device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV));
	cmdList->SetGraphicsRootDescriptorTable(1, descHandle1);

	// 定数バッファビュー（CBV）の設定コマンド
	cmdList->SetGraphicsRootConstantBufferView(2, constBuff_->GetGPUVirtualAddress());

	// 頂点バッファビューの設定コマンド
	cmdList->IASetVertexBuffers(0, 1, &vertexView_);

	// インデックスバッファビューの設定コマンド
	cmdList->IASetIndexBuffer(&indexView_);

	// 描画コマンド
	cmdList->DrawIndexedInstanced(static_cast<UINT>(index_.size()), 1, 0, 0, 0);
}

void PostEffect::PreDraw()
{
	// コマンドリスト取得
	ID3D12GraphicsCommandList* cmdList = DX12Cmd::GetInstance()->GetCmdList();

	// デバイス取得
	ID3D12Device* device = DX12Cmd::GetInstance()->GetDevice();

	// ウィンドウサイズを取得
	float winWidth = static_cast<float>(WinAPI::GetInstance()->GetWidth());
	float winHeight = static_cast<float>(WinAPI::GetInstance()->GetHeight());

	for (size_t i = 0; i < 2; i++) {
		CD3DX12_RESOURCE_BARRIER resourceBarrier =
			CD3DX12_RESOURCE_BARRIER::Transition(texBuff_[i].Get(),
				D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
				D3D12_RESOURCE_STATE_RENDER_TARGET);

		// リソースバリアを変更(シェーダーリソース->描画可能)
		cmdList->ResourceBarrier(1, &resourceBarrier);
	}

	// レンダーターゲットビュー用デスクリプタヒープのハンドルを取得
	std::vector<D3D12_CPU_DESCRIPTOR_HANDLE> rtvHs(2);

	for (size_t i = 0; i < 2; i++) {
		CD3DX12_CPU_DESCRIPTOR_HANDLE descHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(
			descHeapRTV_->GetCPUDescriptorHandleForHeapStart(), static_cast<INT>(i),
			device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV));

		rtvHs[i] = descHandle;
	}

	// 深度ステンシルビュー用デスクリプタヒープのハンドル取得
	D3D12_CPU_DESCRIPTOR_HANDLE dsvH =
		descHeapDSV_->GetCPUDescriptorHandleForHeapStart();

	// レンダーターゲットをセット
	cmdList->OMSetRenderTargets(2, rtvHs.data(), false, &dsvH);

	// ビューポート設定
	std::vector<CD3DX12_VIEWPORT> viewPorts(2);

	for (size_t i = 0; i < 2; i++) {
		viewPorts[i] = CD3DX12_VIEWPORT(0.0f, 0.0f, winWidth, winHeight);
	}

	cmdList->RSSetViewports(1, viewPorts.data());

	// シザリング矩形の設定
	std::vector<CD3DX12_RECT> rects(2);

	for (size_t i = 0; i < 2; i++) {
		rects[i] = CD3DX12_RECT(0, 0, static_cast<LONG>(winWidth), static_cast<LONG>(winHeight));
	}

	cmdList->RSSetScissorRects(
		2, rects.data());

	// 全画面クリア
	for (size_t i = 0; i < 2; i++) {
		cmdList->ClearRenderTargetView(rtvHs[i], clearColor_.data(), 0, nullptr);
	}

	// 深度バッファのクリア
	cmdList->ClearDepthStencilView(
		dsvH, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
}

void PostEffect::PostDraw()
{
	// コマンドリスト取得
	ID3D12GraphicsCommandList* cmdList = DX12Cmd::GetInstance()->GetCmdList();

	// リソースバリアを変更(描画可能->シェーダーリソース)
	for (size_t i = 0; i < 2; i++) {
		CD3DX12_RESOURCE_BARRIER resourceBarrier =
			CD3DX12_RESOURCE_BARRIER::Transition(texBuff_[i].Get(),
				D3D12_RESOURCE_STATE_RENDER_TARGET,
				D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

		cmdList->ResourceBarrier(1, &resourceBarrier);
	}
}

void PostEffect::UpdateData()
{
	// スプライトデータの変更がされていなかったら処理を飛ばす
	if (hasChanget_ == false) return;

#pragma region 頂点座標変更(画像のサイズを変更)

	// 頂点データ
	vertex_[0] = { { -scale_.x, -scale_.y }, {0.0f, 1.0f} };// 左下
	vertex_[1] = { { -scale_.x, +scale_.y }, {0.0f, 0.0f} };// 左上
	vertex_[2] = { { +scale_.x, -scale_.y }, {1.0f, 1.0f} };// 右下
	vertex_[3] = { { +scale_.x, +scale_.y }, {1.0f, 0.0f} };// 右上

	// 全頂点に対して
	for (size_t i = 0; i < vertex_.size(); i++)
	{
		// 座標をコピー
		vertexMap_[i] = vertex_[i];
	}
#pragma endregion

	// 変更したのでフラグを[OFF]にする
	hasChanget_ = false;
}

void PostEffect::CreateVertexBuff()
{
	// デバイス取得
	ID3D12Device* device = DX12Cmd::GetInstance()->GetDevice();

	// 関数が成功したかどうかを判別する用変数
	HRESULT result;

	// 頂点データ
	vertex_[0] = { { -1.0f, -1.0f }, {0.0f, 1.0f} };// 左下
	vertex_[1] = { { -1.0f, +1.0f }, {0.0f, 0.0f} };// 左上
	vertex_[2] = { { +1.0f, -1.0f }, {1.0f, 1.0f} };// 右下
	vertex_[3] = { { +1.0f, +1.0f }, {1.0f, 0.0f} };// 右上

#pragma region 頂点バッファ確保
	// 頂点データ全体のサイズ = 頂点データ一つ分のサイズ * 頂点データの要素数
	UINT sizeVB = static_cast<UINT>(sizeof(vertex_[0]) * vertex_.size());

	// 頂点バッファの設定
	D3D12_HEAP_PROPERTIES heapVSProp{};			// ヒープ設定
	heapVSProp.Type = D3D12_HEAP_TYPE_UPLOAD;	// GPUへの転送用

	// リソース設定
	D3D12_RESOURCE_DESC resDesc{};
	resDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	resDesc.Width = sizeVB; // 頂点データ全体のサイズ
	resDesc.Height = 1;
	resDesc.DepthOrArraySize = 1;
	resDesc.MipLevels = 1;
	resDesc.SampleDesc.Count = 1;
	resDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	// 頂点バッファの生成
	result = device->CreateCommittedResource(
		&heapVSProp,// ヒープ設定
		D3D12_HEAP_FLAG_NONE,
		&resDesc,	// リソース設定
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&vertexBuff_));
	assert(SUCCEEDED(result));
#pragma endregion

#pragma region 頂点バッファへのデータ伝送
	// Map処理でメインメモリとGPUのメモリを紐づける
	result = vertexBuff_->Map(0, nullptr, (void**)&vertexMap_);
	assert(SUCCEEDED(result));

	// 全頂点に対して
	for (size_t i = 0; i < vertex_.size(); i++)
	{
		// 座標をコピー
		vertexMap_[i] = vertex_[i];
	}
#pragma endregion

#pragma region 頂点バッファビュー作成

	// GPU仮想アドレス
	vertexView_.BufferLocation = vertexBuff_->GetGPUVirtualAddress();

	// 頂点バッファのサイズ
	vertexView_.SizeInBytes = sizeVB;

	// 頂点1つ分のデータサイズ
	vertexView_.StrideInBytes = sizeof(vertex_[0]);
#pragma endregion
}

void PostEffect::CreateIndexBuff()
{
	// デバイス取得
	ID3D12Device* device = DX12Cmd::GetInstance()->GetDevice();

	// 関数が成功したかどうかを判別する用変数
	HRESULT result;

	// インデックスデータ
	index_[0] = 0;
	index_[1] = 1;
	index_[2] = 2;
	index_[3] = 1;
	index_[4] = 2;
	index_[5] = 3;

#pragma region インデックスバッファ確保
	// インデックスデータ全体のサイズ = インデックスデータ一つ分のサイズ * インデックスデータの要素数
	UINT sizeIB = static_cast<UINT>(sizeof(uint16_t) * index_.size());

	// インデックスバッファの設定
	D3D12_HEAP_PROPERTIES heapVSProp{};			// ヒープ設定
	heapVSProp.Type = D3D12_HEAP_TYPE_UPLOAD;	// GPUへの転送用

	// リリース設定
	D3D12_RESOURCE_DESC resDesc{};
	resDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	resDesc.Width = sizeIB; // インデックス情報が入る分のサイズ
	resDesc.Height = 1;
	resDesc.DepthOrArraySize = 1;
	resDesc.MipLevels = 1;
	resDesc.SampleDesc.Count = 1;
	resDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	// インデックスバッファの生成
	result = device->CreateCommittedResource(
		&heapVSProp,// ヒープ設定
		D3D12_HEAP_FLAG_NONE,
		&resDesc,	// リソース設定
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&indexBuff_)
	);
#pragma endregion

#pragma region インデックスバッファへのデータ転送
	// マッピング用
	uint16_t* indexMap = nullptr;
	result = indexBuff_->Map(0, nullptr, (void**)&indexMap);

	// 全インデックスに対して
	for (int i = 0; i < index_.size(); i++) {
		indexMap[i] = index_[i];
	}

	// マッピング解除
	indexBuff_->Unmap(0, nullptr);
#pragma endregion

#pragma region インデックスバッファビューの作成
	// GPU仮想アドレス
	indexView_.BufferLocation = indexBuff_->GetGPUVirtualAddress();

	// フォーマット指定
	indexView_.Format = DXGI_FORMAT_R16_UINT;

	// インデックスデータ1つ分のサイズ
	indexView_.SizeInBytes = sizeIB;
#pragma endregion
}

void PostEffect::CreateConstBuff()
{
	// デバイス取得
	ID3D12Device* device = DX12Cmd::GetInstance()->GetDevice();

	// 関数が成功したかどうかを判別する用変数
	HRESULT result;

#pragma region 定数バッファ生成
	// 定数バッファのヒープ設定
	D3D12_HEAP_PROPERTIES heapProp{};
	heapProp.Type = D3D12_HEAP_TYPE_UPLOAD;

	// 定数バッファのリソース設定
	D3D12_RESOURCE_DESC resdesc{};
	resdesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	resdesc.Width = (sizeof(ConstBufferData) + 0xff) & ~0xff;
	resdesc.Height = 1;
	resdesc.DepthOrArraySize = 1;
	resdesc.MipLevels = 1;
	resdesc.SampleDesc.Count = 1;
	resdesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	// 定数バッファの生成
	result = device->CreateCommittedResource(
		&heapProp,
		D3D12_HEAP_FLAG_NONE,
		&resdesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&constBuff_));
	assert(SUCCEEDED(result));
#pragma endregion

#pragma region 定数バッファへのデータ転送
	// 定数バッファのマッピング
	result = constBuff_->Map(0, nullptr, (void**)&constMap_);
	assert(SUCCEEDED(result));
#pragma endregion
}

void PostEffect::CreateTextureBuff()
{
	// デバイス取得
	ID3D12Device* device = DX12Cmd::GetInstance()->GetDevice();

	// ウィンドウサイズを取得
	float winWidth = static_cast<float>(WinAPI::GetInstance()->GetWidth());
	float winHeight = static_cast<float>(WinAPI::GetInstance()->GetHeight());

	// 関数が成功したかどうかを判別する用変数
	HRESULT result;

	// テクスチャリソース設定
	CD3DX12_RESOURCE_DESC textureDesc =
		CD3DX12_RESOURCE_DESC::Tex2D(
			DXGI_FORMAT_R8G8B8A8_UNORM_SRGB,
			static_cast<UINT>(winWidth), static_cast<UINT>(winHeight),
			1, 0, 1, 0, D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET);

	CD3DX12_HEAP_PROPERTIES texHeapProp = CD3DX12_HEAP_PROPERTIES(
		D3D12_CPU_PAGE_PROPERTY_WRITE_BACK,
		D3D12_MEMORY_POOL_L0);

	CD3DX12_CLEAR_VALUE texClearValue =
		CD3DX12_CLEAR_VALUE(DXGI_FORMAT_R8G8B8A8_UNORM_SRGB, clearColor_.data());

	// テクスチャバッファの生成
	for (size_t i = 0; i < 2; i++) {
		result = device->CreateCommittedResource(
			&texHeapProp,
			D3D12_HEAP_FLAG_NONE,
			&textureDesc,
			D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
			&texClearValue,
			IID_PPV_ARGS(&texBuff_[i]));
		assert(SUCCEEDED(result));

		// イメージデータ
		{
			// 画素数
			const UINT pixelCount = static_cast<UINT>(winWidth * winHeight);

			// 画像一行分のデータサイズ
			const UINT rowPitch = sizeof(UINT) * static_cast<UINT>(winWidth);

			const UINT depthPitch = rowPitch * static_cast<UINT>(winHeight);

			// 画像イメージ
			std::vector<UINT> img(pixelCount);
			for (size_t j = 0; j < pixelCount; j++) { img[j] = 0xff0000ff; }

			// テクスチャバッファにデータ転送
			result = texBuff_[i]->WriteToSubresource(
				0, nullptr, img.data(), rowPitch, depthPitch);
			assert(SUCCEEDED(result));
		}
	}
}

void PostEffect::CreateSRVDescHeap()
{
	// デバイス取得
	ID3D12Device* device = DX12Cmd::GetInstance()->GetDevice();

	// 関数が成功したかどうかを判別する用変数
	HRESULT result;

	// SRV用デスクリプタヒープ設定
	D3D12_DESCRIPTOR_HEAP_DESC srvDescHeapDesc = {};
	srvDescHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	srvDescHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	srvDescHeapDesc.NumDescriptors = 2;

	// SRV用デスクリプタヒープを生成
	result = device->CreateDescriptorHeap(
		&srvDescHeapDesc, IID_PPV_ARGS(&descHeapSRV_));
	assert(SUCCEEDED(result));

	// SRV設定
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};// 設定構造体
	srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	srvDesc.Shader4ComponentMapping =
		D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;// 2Dテクスチャ
	srvDesc.Texture3D.MipLevels = 1;

	// デスクリプタヒープにSRV作成
	for (size_t i = 0; i < 2; i++) {
		CD3DX12_CPU_DESCRIPTOR_HANDLE descHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(
			descHeapSRV_->GetCPUDescriptorHandleForHeapStart(), static_cast<INT>(i),
			device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV));

		device->CreateShaderResourceView(texBuff_[i].Get(),// ビューと関連付けるバッファ
			&srvDesc, descHandle);
	}
}

void PostEffect::CreateRTVDescHeap()
{
	// デバイス取得
	ID3D12Device* device = DX12Cmd::GetInstance()->GetDevice();

	// 関数が成功したかどうかを判別する用変数
	HRESULT result;

	// RTV用デスクリプタヒープ設定
	D3D12_DESCRIPTOR_HEAP_DESC rtvDescHeapDesc{};
	rtvDescHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	rtvDescHeapDesc.NumDescriptors = 2;

	// RTV用デスクリプタヒープを生成
	result = device->CreateDescriptorHeap(
		&rtvDescHeapDesc, IID_PPV_ARGS(&descHeapRTV_));
	assert(SUCCEEDED(result));

	// デスクリプタヒープにRTV作成
	for (size_t i = 0; i < 2; i++) {
		device->CreateRenderTargetView(
			texBuff_[i].Get(), nullptr,
			CD3DX12_CPU_DESCRIPTOR_HANDLE(descHeapRTV_->GetCPUDescriptorHandleForHeapStart(), static_cast<INT>(i),
				device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV)));
	}
}

void PostEffect::CreateDepthBuff()
{
	// デバイス取得
	ID3D12Device* device = DX12Cmd::GetInstance()->GetDevice();

	// ウィンドウサイズを取得
	float winWidth = static_cast<float>(WinAPI::GetInstance()->GetWidth());
	float winHeight = static_cast<float>(WinAPI::GetInstance()->GetHeight());

	// 関数が成功したかどうかを判別する用変数
	HRESULT result;

	// 深度バッファリソース設定
	CD3DX12_RESOURCE_DESC depthResDesc =
		CD3DX12_RESOURCE_DESC::Tex2D(
			DXGI_FORMAT_D32_FLOAT,
			static_cast<UINT64>(winWidth),
			static_cast<UINT>(winHeight),
			1, 0, 1, 0,
			D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL);

	CD3DX12_HEAP_PROPERTIES depthHeapPro =
		CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);

	CD3DX12_CLEAR_VALUE depthClearValue =
		CD3DX12_CLEAR_VALUE(DXGI_FORMAT_D32_FLOAT, 1.0f, 0);


	// 深度バッファの生成
	result = device->CreateCommittedResource(
		&depthHeapPro,
		D3D12_HEAP_FLAG_NONE,
		&depthResDesc,
		D3D12_RESOURCE_STATE_DEPTH_WRITE,
		&depthClearValue,
		IID_PPV_ARGS(&depthBuff_));
	assert(SUCCEEDED(result));
}

void PostEffect::CreateDSVDescHeap()
{
	// デバイス取得
	ID3D12Device* device = DX12Cmd::GetInstance()->GetDevice();

	// 関数が成功したかどうかを判別する用変数
	HRESULT result;

	// DSV用デスクリプタヒープ設定
	D3D12_DESCRIPTOR_HEAP_DESC descHeapDesc{};
	descHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	descHeapDesc.NumDescriptors = 1;

	// DRV用デスクリプタヒープを作成
	result = device->CreateDescriptorHeap(
		&descHeapDesc, IID_PPV_ARGS(&descHeapDSV_));
	assert(SUCCEEDED(result));

	// デスクリプタヒープにDSV作成
	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
	dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;// 深度地フォーマット
	dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	device->CreateDepthStencilView(depthBuff_.Get(),
		&dsvDesc, descHeapDSV_->GetCPUDescriptorHandleForHeapStart());
}