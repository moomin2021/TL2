#include "Material.h"
#include "DX12Cmd.h"
#include "Texture.h"

#include <cassert>

Material::Material() :
#pragma region 初期化リスト
	// マテリアルデータ
	name_{},				// マテリアル名
	ambient_{},				// アンビエント影響度
	diffuse_{},				// ディフューズ影響度
	specular_{},			// スペキュラー影響度
	alpha_(1.0f),			// アルファ値
	texHandle_(0),			// テクスチャハンドル
	materialBuff_(nullptr)	// マテリアルバッファ
#pragma endregion
{

}

void Material::Draw()
{
	// コマンドリスト取得
	static ID3D12GraphicsCommandList* cmdList = DX12Cmd::GetInstance()->GetCmdList();

	// インスタンス取得
	static Texture* tex = Texture::GetInstance();

	// SRVヒープのハンドルを取得
	D3D12_GPU_DESCRIPTOR_HANDLE srvGpuHandle = tex->GetSRVHeap()->GetGPUDescriptorHandleForHeapStart();

	// ハンドルを指定された分まで進める
	srvGpuHandle.ptr += texHandle_;

	// 定数バッファビュー（CBV）の設定コマンド
	cmdList->SetGraphicsRootConstantBufferView(2, materialBuff_->GetGPUVirtualAddress());

	// 指定されたSRVをルートパラメータ1番に設定
	cmdList->SetGraphicsRootDescriptorTable(0, srvGpuHandle);
}

void Material::CreateMaterialBuff()
{
	// デバイス取得
	static ID3D12Device* device = DX12Cmd::GetInstance()->GetDevice();

	// 関数実行の成否を判別用の変数
	HRESULT result;

	// 定数バッファのヒープ設定
	D3D12_HEAP_PROPERTIES heapProp{};
	heapProp.Type = D3D12_HEAP_TYPE_UPLOAD;

	// 定数バッファのリソース設定
	D3D12_RESOURCE_DESC resdesc{};
	resdesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	resdesc.Width = (sizeof(MaterialBuffer) + 0xff) & ~0xff;
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
		IID_PPV_ARGS(&materialBuff_));
	assert(SUCCEEDED(result));

	// マテリアル定数バッファのマッピング
	MaterialBuffer* materialMap;
	result = materialBuff_->Map(0, nullptr, (void**)&materialMap);
	assert(SUCCEEDED(result));
	materialMap->ambient = ambient_;
	materialMap->diffuse = diffuse_;
	materialMap->specular = specular_;
	materialMap->alpha = alpha_;
	materialBuff_->Unmap(0, nullptr);
}
