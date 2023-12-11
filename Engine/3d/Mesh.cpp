#include "Mesh.h"
#include "DX12Cmd.h"

#include <cassert>

Mesh::Mesh() :
#pragma region 初期化リスト
	// 頂点データ
	vertices_{},			// 頂点データ
	vertexView_{},			// 頂点バッファービュー
	vertexBuff_(nullptr),	// 頂点バッファ
	
	// インデックスデータ
	indexes_{},			// インデックスデータ
	indexView_{},		// インデックスバッファビュー
	indexBuff_(nullptr),// インデックスバッファ
	
	// 適用されるマテリアルの名前
	materialName_{}
#pragma endregion
{
}

void Mesh::Draw()
{
	// コマンドリスト取得
	static ID3D12GraphicsCommandList* cmdList = DX12Cmd::GetInstance()->GetCmdList();

	// 頂点バッファビューの設定コマンド
	cmdList->IASetVertexBuffers(0, 1, &vertexView_);

	// インデックスバッファビューの設定コマンド
	cmdList->IASetIndexBuffer(&indexView_);

	// 描画コマンド
	cmdList->DrawIndexedInstanced(static_cast<UINT>(indexes_.size()), 1, 0, 0, 0);
}

void Mesh::CreateVertexBuff()
{
	// デバイス取得
	ID3D12Device* device = DX12Cmd::GetInstance()->GetDevice();

	// 関数実行の成否を判別用の変数
	HRESULT result;

	// 頂点データ全体のサイズ = 頂点データ一つ分のサイズ * 頂点データの要素数
	UINT sizeVB = static_cast<UINT>(sizeof(vertices_[0]) * vertices_.size());

	// 頂点バッファの設定
	D3D12_HEAP_PROPERTIES heapProp{};		// ヒープ設定
	heapProp.Type = D3D12_HEAP_TYPE_UPLOAD;	// GPUへの転送用

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
		&heapProp, // ヒープ設定
		D3D12_HEAP_FLAG_NONE,
		&resDesc, // リソース設定
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&vertexBuff_));
	assert(SUCCEEDED(result));

	// 頂点バッファビューの作成
	vertexView_.BufferLocation = vertexBuff_->GetGPUVirtualAddress();// GPU仮想アドレス
	vertexView_.SizeInBytes = sizeVB;				// 頂点バッファのサイズ
	vertexView_.StrideInBytes = sizeof(vertices_[0]);	// 頂点1つ分のデータサイズ

	// Map処理でメインメモリとGPUのメモリを紐づける
	Vertex* vertMap = nullptr;
	result = vertexBuff_->Map(0, nullptr, (void**)&vertMap);
	assert(SUCCEEDED(result));

	// 全頂点に対して
	for (size_t i = 0; i < vertices_.size(); i++)
	{
		vertMap[i] = vertices_[i]; // 座標をコピー
	}

	// 繋がりを解除
	vertexBuff_->Unmap(0, nullptr);
}

void Mesh::CreateIndexBuff()
{
	// デバイス取得
	ID3D12Device* device = DX12Cmd::GetInstance()->GetDevice();

	// 関数実行の成否を判別用の変数
	HRESULT result;

	// インデックスデータ全体のサイズ
	UINT sizeIB = static_cast<UINT>(sizeof(uint16_t) * indexes_.size());

	// 頂点バッファの設定
	D3D12_HEAP_PROPERTIES heapProp{};		// ヒープ設定
	heapProp.Type = D3D12_HEAP_TYPE_UPLOAD;	// GPUへの転送用

	// リソース設定
	D3D12_RESOURCE_DESC resDesc{};
	resDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	resDesc.Width = sizeIB; // 頂点データ全体のサイズ
	resDesc.Height = 1;
	resDesc.DepthOrArraySize = 1;
	resDesc.MipLevels = 1;
	resDesc.SampleDesc.Count = 1;
	resDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	// インデックスバッファの生成
	result = device->CreateCommittedResource(
		&heapProp,// ヒープ設定
		D3D12_HEAP_FLAG_NONE,
		&resDesc,// リソース設定
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&indexBuff_)
	);

	// インデックスバッファビュー作成
	indexView_.BufferLocation = indexBuff_->GetGPUVirtualAddress();
	indexView_.Format = DXGI_FORMAT_R16_UINT;
	indexView_.SizeInBytes = sizeIB;

	// インデックスバッファをマッピング
	uint16_t* indexMap = nullptr;
	result = indexBuff_->Map(0, nullptr, (void**)&indexMap);
	assert(SUCCEEDED(result));

	// 全インデックスに対して
	for (size_t i = 0; i < indexes_.size(); i++)
	{
		indexMap[i] = indexes_[i];
	}

	// マッピング解除
	indexBuff_->Unmap(0, nullptr);
}
