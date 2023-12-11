#include "Object3D.h"
#include "DX12Cmd.h"
#include "Texture.h"
#include "Util.h"

// 静的メンバ変数の実態
Camera*		Object3D::sCamera_		= nullptr;// カメラ
LightGroup* Object3D::sLightGroup_	= nullptr;// ライト

Object3D::Object3D(Model* model) : model_(model)
{
	// 関数が成功したかどうかを判別する用変数
	HRESULT result;

	// デバイス取得
	ID3D12Device* device = DX12Cmd::GetInstance()->GetDevice();

#pragma region 定数バッファ生成
	// 定数バッファのヒープ設定
	D3D12_HEAP_PROPERTIES heapProp{};		// ヒープ設定
	heapProp.Type = D3D12_HEAP_TYPE_UPLOAD;	// GPUへの転送用

	// 定数バッファのリソース設定
	D3D12_RESOURCE_DESC resDesc{};
	resDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	resDesc.Width = (sizeof(ConstBufferData) + 0xff) & ~0xff;
	resDesc.Height = 1;
	resDesc.DepthOrArraySize = 1;
	resDesc.MipLevels = 1;
	resDesc.SampleDesc.Count = 1;
	resDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	// 定数バッファの生成
	result = device->CreateCommittedResource(
		&heapProp,
		D3D12_HEAP_FLAG_NONE,
		&resDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&constBuff_));
	assert(SUCCEEDED(result));
#pragma endregion

#pragma region 定数バッファへのデータ転送
	result = constBuff_->Map(0, nullptr, (void**)&constMap_);
	assert(SUCCEEDED(result));
#pragma endregion
}

void Object3D::MatUpdate()
{
	// オブジェクトデータが変更されていたら処理する
	if (hasChanget_) {
#pragma region ワールド行列計算
		// 行列初期化
		matWorld_ = Matrix4Identity();

		// ワールド行列にスケーリングを反映
		matWorld_ *= Matrix4Scale(scale_);

		// ワールド行列に回転を反映
		matWorld_ *= Matrix4RotateZ(Util::Degree2Radian(rotation_.z));
		matWorld_ *= Matrix4RotateX(Util::Degree2Radian(rotation_.x));
		matWorld_ *= Matrix4RotateY(Util::Degree2Radian(rotation_.y));

		// ワールド行列に平行移動を反映
		matWorld_ *= Matrix4Translate(position_);
#pragma endregion

		// 変更したのでフラグを[OFF]にする
		hasChanget_ = false;
	}

#pragma region 定数バッファへのデータ転送
	// ビュープロジェクション転送
	constMap_->viewProj = sCamera_->GetMatView() * sCamera_->GetMatProjection();

	// ワールド行列転送
	constMap_->world = matWorld_;

	// カメラの位置座標(XYZ)転送
	constMap_->cameraPos = sCamera_->GetEye();

	// 色(RGBA)転送
	constMap_->color = color_;
#pragma endregion
}

void Object3D::Draw() {
	// 描画フラグが[OFF]ならこれ以降処理しない
	if (isDraw_ == false) return;

	// コマンドリスト取得
	ID3D12GraphicsCommandList* cmdList = DX12Cmd::GetInstance()->GetCmdList();

	// 定数バッファビュー（CBV）の設定コマンド
	cmdList->SetGraphicsRootConstantBufferView(1, constBuff_->GetGPUVirtualAddress());

	// ライトの描画
	sLightGroup_->Draw();

	// モデルの情報を元に描画
	model_->Draw();
}
