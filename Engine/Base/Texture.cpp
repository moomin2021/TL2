#include "Texture.h"
#include "DX12Cmd.h"
#include "float4.h"
#include "Util.h"
//#include "DirectXTex/d3dx12.h"

#include <vector>
#include <d3dx12.h>

using namespace DirectX;
using namespace std;

Texture* Texture::GetInstance() {
	// インスタンス生成
	static Texture instance;

	// インスタンスを返す
	return &instance;
}

void Texture::Initialize() {
	// SRV用でスクリプタヒープを生成
	CreateDescriptorHeap();

	// デバック用のテクスチャを読み込む
	LoadTexture("Sprite/debug.png");
}

void Texture::LoadImageFile(const std::string filePath, DirectX::ScratchImage& scratchImage, DirectX::TexMetadata& metadata)
{
	HRESULT result;

	// WICテクスチャロード
	result = LoadFromWICFile(
		Util::StringToWideChar(filePath).data(),
		WIC_FLAGS_FORCE_SRGB,
		&metadata, scratchImage
	);
	assert(SUCCEEDED(result));

	// ミップマップ生成
	ScratchImage mipChain{};
	result = GenerateMipMaps(
		scratchImage.GetImages(),
		scratchImage.GetImageCount(),
		scratchImage.GetMetadata(),
		TEX_FILTER_SRGB,
		0, mipChain
	);

	// ミップマップが正常に生成されたら
	if (SUCCEEDED(result)) {
		scratchImage = move(mipChain);
		metadata = scratchImage.GetMetadata();
	}

	// 読み込んだディフューズテクスチャをSRGBとして扱う
	metadata.format = MakeSRGB(metadata.format);
}

ID3D12Resource* Texture::CreateTextureResource(const DirectX::TexMetadata& metadata, D3D12_RESOURCE_DESC& texResourceDesc)
{
	// デバイス取得
	ID3D12Device* device = DX12Cmd::GetInstance()->GetDevice();

	// リソース設定
	texResourceDesc.Width = UINT(metadata.width);
	texResourceDesc.Height = UINT(metadata.height);
	texResourceDesc.MipLevels = UINT16(metadata.mipLevels);
	texResourceDesc.DepthOrArraySize = UINT16(metadata.arraySize);
	texResourceDesc.Format = metadata.format;
	texResourceDesc.SampleDesc.Count = 1;
	texResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION(metadata.dimension);

	// 利用するHeapの設定。
	D3D12_HEAP_PROPERTIES heapProperties{};
	heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;
	//heapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_WRITE_BACK;
	//heapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_L0;

	// Resourceの生成
	ID3D12Resource* resource = nullptr;
	HRESULT result;
	result = device->CreateCommittedResource(
		&heapProperties,
		D3D12_HEAP_FLAG_NONE,
		&texResourceDesc,
		D3D12_RESOURCE_STATE_COPY_DEST,
		nullptr,
		IID_PPV_ARGS(&resource));
	assert(SUCCEEDED(result));

	return resource;
}

[[nodiscard]]
ID3D12Resource* Texture::UploadTextureData(ID3D12Resource* texture, const DirectX::ScratchImage& mipImages)
{
	// デバイス取得
	ID3D12Device* device = DX12Cmd::GetInstance()->GetDevice();

	// コマンドリスト取得
	ID3D12GraphicsCommandList* cmdList = DX12Cmd::GetInstance()->GetCmdList();
	
	std::vector<D3D12_SUBRESOURCE_DATA> subResources;

	PrepareUpload(device, mipImages.GetImages(),
		mipImages.GetImageCount(), mipImages.GetMetadata(), subResources);

	uint64_t intermediateSize = GetRequiredIntermediateSize(texture, 0, UINT(subResources.size()));
	ID3D12Resource* intermediateResource = CreateBufferResource(intermediateSize);
	UpdateSubresources(cmdList, texture, intermediateResource, 0, 0,
		UINT(subResources.size()), subResources.data());
	
	// Textureへの転送後は利用できるように、D3D12_RESOURCE_STATE_COPY_DESTから
	// D3D12_RESOURCE_STATE_GENERIC_READへResourceStateを変更する
	D3D12_RESOURCE_BARRIER barrier{};
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier.Transition.pResource = texture;
	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_GENERIC_READ;
	cmdList->ResourceBarrier(1, &barrier);
	return intermediateResource;
}

ID3D12Resource* Texture::CreateBufferResource(size_t sizeInBytes)
{
	// デバイス取得
	ID3D12Device* device = DX12Cmd::GetInstance()->GetDevice();

	// 定数バッファのヒープ設定
	D3D12_HEAP_PROPERTIES heapProp{};		// ヒープ設定
	heapProp.Type = D3D12_HEAP_TYPE_UPLOAD;	// GPUへの転送用

	// 定数バッファのリソース設定
	D3D12_RESOURCE_DESC resDesc{};
	resDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	resDesc.Width = sizeInBytes;
	resDesc.Height = 1;
	resDesc.DepthOrArraySize = 1;
	resDesc.MipLevels = 1;
	resDesc.SampleDesc.Count = 1;
	resDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	// Resourceの生成
	ID3D12Resource* resource = nullptr;
	HRESULT result;
	result = device->CreateCommittedResource(
		&heapProp,
		D3D12_HEAP_FLAG_NONE,
		&resDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&resource));
	assert(SUCCEEDED(result));

	return resource;
}

uint16_t Texture::LoadTexture(const std::string fileName)
{
	// デバイス取得
	ID3D12Device* device = DX12Cmd::GetInstance()->GetDevice();

	// 既に読み込んだ物だったら
	if (texBuff_.find(fileName) != texBuff_.end()) {
		return texHandle_[fileName];
	}

	TexMetadata metadata{};
	ScratchImage scratchImage{};

	std::string path = "Resources/" + fileName;

	// 画像読み込み
	LoadImageFile(path, scratchImage, metadata);

	D3D12_RESOURCE_DESC texResourceDesc{};
	ID3D12Resource* texResource{};
	
	// テクスチャバッファの生成
	texResource = CreateTextureResource(metadata, texResourceDesc);

	// テクスチャバッファにデータを転送
	intermediateResources_.emplace_back(UploadTextureData(texResource, scratchImage));

	// 読み込んだディフューズテクスチャをSRGBとして扱う
	metadata.format = MakeSRGB(metadata.format);

	// CBV, SRV, UAVの1個分のサイズを取得
	UINT descriptorSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	// 読み込んだ画像のハンドル
	uint16_t handle = static_cast<uint16_t>(descriptorSize) * loadCounter_;

	// 設定を保存
	texBuff_.emplace(fileName, texResource);
	texHandle_.emplace(fileName, handle);

	// シェーダリソースビュー設定
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Format = texResourceDesc.Format;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = UINT(texResourceDesc.MipLevels);

	// ハンドルの指す位置にシェーダーリソースビュー作成
	device->CreateShaderResourceView(texBuff_[fileName].Get(), &srvDesc, srvHandle_);

	// ハンドルを1つ進める
	srvHandle_.ptr += descriptorSize;

	// カウンターを進める
	loadCounter_++;

	// ハンドルを返す
	return handle;
}

void Texture::ReleaseIntermediateResources()
{
	if (intermediateResources_.empty()) return;

	for (auto& i : intermediateResources_) {
		i->Release();
	}

	intermediateResources_.clear();
}

//uint16_t Texture::LoadTexture(const std::string fileName)
//{
//	// デバイス取得
//	ID3D12Device* device = DX12Cmd::GetInstance()->GetDevice();
//
//	// 既に読み込んだ物だったら
//	if (texBuff_.find(fileName) != texBuff_.end()) {
//		return texHandle_[fileName];
//	}
//
//	// 関数が成功したかどうかを判別する用変数
//	HRESULT result;
//
//	TexMetadata metadata{};
//	ScratchImage scratchImg{};
//
//	// WICテクスチャのロード
//	result = LoadFromWICFile(
//		Util::StringToWideChar(fileName).data(),
//		WIC_FLAGS_NONE,
//		&metadata, scratchImg
//	);
//
//	ScratchImage mipChain{};
//
//	// ミップマップ生成
//	result = GenerateMipMaps(
//		scratchImg.GetImages(),
//		scratchImg.GetImageCount(),
//		scratchImg.GetMetadata(),
//		TEX_FILTER_DEFAULT,
//		0,
//		mipChain
//	);
//
//	if (SUCCEEDED(result)) {
//		scratchImg = std::move(mipChain);
//		metadata = scratchImg.GetMetadata();
//	}
//
//	// 読み込んだディフューズテクスチャをSRGBとして扱う
//	metadata.format = MakeSRGB(metadata.format);
//
//	// ヒープ設定
//	D3D12_HEAP_PROPERTIES textureHeapProp{};
//	textureHeapProp.Type = D3D12_HEAP_TYPE_CUSTOM;
//	textureHeapProp.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_WRITE_BACK;
//	textureHeapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_L0;
//
//	// リソース設定
//	D3D12_RESOURCE_DESC textureResourceDesc{};
//	textureResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
//	textureResourceDesc.Format = metadata.format;
//	textureResourceDesc.Width = metadata.width;
//	textureResourceDesc.Height = static_cast<uint32_t>(metadata.height);
//	textureResourceDesc.DepthOrArraySize = static_cast<uint32_t>(metadata.arraySize);
//	textureResourceDesc.MipLevels = static_cast<uint16_t>(metadata.mipLevels);
//	textureResourceDesc.SampleDesc.Count = 1;
//
//	// テクスチャバッファの生成
//	ComPtr<ID3D12Resource> texBuff = nullptr;
//	result = device->CreateCommittedResource(
//		&textureHeapProp,
//		D3D12_HEAP_FLAG_NONE,
//		&textureResourceDesc,
//		D3D12_RESOURCE_STATE_GENERIC_READ,
//		nullptr,
//		IID_PPV_ARGS(&texBuff)
//	);
//
//	// 全ミップマップについて
//	for (size_t i = 0; i < metadata.mipLevels; i++)
//	{
//		// ミップマップレベルを指定してイメージを取得
//		const Image* img = scratchImg.GetImage(i, 0, 0);
//
//		// テクスチャバッファにデータ転送
//		result = texBuff->WriteToSubresource(
//			static_cast<uint32_t>(i),
//			nullptr,								// 全領域へコピー
//			img->pixels,							// 元データアドレス
//			static_cast<uint32_t>(img->rowPitch),	// 1ラインサイズ
//			static_cast<uint32_t>(img->slicePitch)	// 1枚サイズ
//		);
//		assert(SUCCEEDED(result));
//	}
//
//	// CBV, SRV, UAVの1個分のサイズを取得
//	UINT descriptorSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
//
//	// 読み込んだ画像のハンドル
//	uint16_t handle = descriptorSize * loadCounter_;
//
//	// 設定を保存
//	texBuff_.emplace(fileName, texBuff);
//	texHandle_.emplace(fileName, handle);
//
//	// シェーダリソースビュー設定
//	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
//	srvDesc.Format = textureResourceDesc.Format;
//	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
//	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
//	srvDesc.Texture2D.MipLevels = textureResourceDesc.MipLevels;
//
//	// ハンドルの指す①にシェーダーリソースビュー作成
//	device->CreateShaderResourceView(texBuff_[fileName].Get(), &srvDesc, srvHandle_);
//
//	// ハンドルを1つ進める
//	srvHandle_.ptr += descriptorSize;
//
//	// カウンターを進める
//	loadCounter_++;
//
//	// ハンドルを返す
//	return handle;
//}


Texture::Texture() {}

void Texture::CreateDescriptorHeap()
{
	// 関数が成功したかどうかを判別する用変数
	HRESULT result;

	// デスクリプタヒープの設定
	D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc = {};
	srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	srvHeapDesc.NumDescriptors = 2056;

	// 設定をもとにSRV用デスクリプタヒープを生成
	result = DX12Cmd::GetInstance()->GetDevice()->CreateDescriptorHeap(&srvHeapDesc, IID_PPV_ARGS(&srvHeap_));
	assert(SUCCEEDED(result));

	// SRVヒープの先頭ハンドルを取得
	srvHandle_ = srvHeap_->GetCPUDescriptorHandleForHeapStart();
}

uint16_t LoadTexture(const std::string fileName) {
	// 画像読み込み
	return Texture::GetInstance()->LoadTexture(fileName);
}
