#include "LightGroup.h"
#include "DX12Cmd.h"

#include <d3dx12.h>
#include <cassert>

LightGroup::LightGroup() :
#pragma region 初期化リスト
	// 定数バッファ
	constBuff_(nullptr),

	// 環境光の色
	ambientColor_{ 1.0f, 1.0f, 1.0f },
	
	// ライトデータ
	dirLights_(DIR_LIGHT_LIMIT),		// 平行光源の配列
	pointLights_(POINT_LIGHT_LIMIT),	// 点光源の配列
	spotLights_(SPOT_LIGHT_LIMIT),		// スポットライト用
	circleShadows_(CIRCLE_SHADOW_LIMIT),// 丸影の配列

	// ダーティフラグ
	dirty_(false)
#pragma endregion
{
	// デバイス取得
	ID3D12Device* device = DX12Cmd::GetInstance()->GetDevice();

	// 関数が成功したかどうかを判別する用変数
	HRESULT result;

	// ヒープ設定
	CD3DX12_HEAP_PROPERTIES heapProps = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);

	// リソース設定
	CD3DX12_RESOURCE_DESC resourceDesc =
		CD3DX12_RESOURCE_DESC::Buffer((sizeof(ConstBufferData) + 0xff) & ~0xff);

	// 定数バッファの生成
	result = device->CreateCommittedResource(
		&heapProps, D3D12_HEAP_FLAG_NONE, &resourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr,
		IID_PPV_ARGS(&constBuff_));
	assert(SUCCEEDED(result));
}

void LightGroup::Draw() {
	// 定数バッファ転送
	TransferConstBuffer();

	// コマンドリスト取得
	ID3D12GraphicsCommandList* cmdList = DX12Cmd::GetInstance()->GetCmdList();

	// 定数バッファビューをセット
	cmdList->SetGraphicsRootConstantBufferView(3, constBuff_->GetGPUVirtualAddress());
}

void LightGroup::Reset()
{
	for (auto& it : dirLights_) it = nullptr;
	for (auto& it : pointLights_) it = nullptr;
}

LightGroup* LightGroup::GetInstance()
{
	// インスタンス生成
	static LightGroup inst;

	// インスタンスを返す
	return &inst;
}

void LightGroup::TransferConstBuffer() {

	// ダーティフラグが[OFF]なら処理を飛ばす
	//if (dirty_ == false) return;

	// 関数が成功したかどうかを判別する用変数
	HRESULT result;

#pragma region 定数バッファマッピング
	ConstBufferData* constMap = nullptr;
	result = constBuff_->Map(0, nullptr, (void**)&constMap);
#pragma endregion

	if (SUCCEEDED(result)) {
#pragma region 平行光源
		{// 始
			std::forward_list<DirectionalLight*>::iterator itDirLight = dirLights_.begin();// イテレーター
			size_t index = 0;// インデックス
			while (index < DIR_LIGHT_LIMIT)
			{
				if (*itDirLight == nullptr)
				{
					constMap->dirLights[index].active = false;
				}

				// ライトが有効なら設定を転送
				else if ((*itDirLight)->GetActive())
				{
					constMap->dirLights[index].active = true;
					constMap->dirLights[index].lightVec = -(*itDirLight)->GetLightDir();
					constMap->dirLights[index].lightColor = (*itDirLight)->GetLightColor();
				}

				// ライトが無効なら設定を転送しない
				else constMap->dirLights[index].active = false;

				// インデックスとイテレータを進める
				++index;
				++itDirLight;
			}
		}// 終
#pragma endregion

#pragma region 点光源
		{// 始
			std::forward_list<PointLight*>::iterator itPointLight = pointLights_.begin();// イテレーター
			size_t index = 0;// インデックス
			while (index < POINT_LIGHT_LIMIT && *itPointLight != nullptr)
			{
				if (*itPointLight == nullptr)
				{
					constMap->pointLights[index].active = false;
				}

				// ライトが有効なら設定を転送
				else if ((*itPointLight)->GetActive())
				{
					constMap->pointLights[index].active = true;
					constMap->pointLights[index].pos = (*itPointLight)->GetPosition();
					constMap->pointLights[index].color = float4{ (*itPointLight)->GetColor(), 1.0f };
					constMap->pointLights[index].intensity = (*itPointLight)->GetIntensity();
					constMap->pointLights[index].radius = (*itPointLight)->GetRadius();
					constMap->pointLights[index].decay = (*itPointLight)->GetDecay();
				}

				// ライトが無効なら設定を転送しない
				else constMap->pointLights[index].active = false;

				// インデックスとイテレータを進める
				++index;
				++itPointLight;
			}
		}// 終
#pragma endregion

#pragma region スポットライト
		{// 始
			std::forward_list<SpotLight*>::iterator itSpotLight = spotLights_.begin();// イテレーター
			size_t index = 0;// インデックス
			while (index < SPOT_LIGHT_LIMIT && *itSpotLight != nullptr)
			{
				if (*itSpotLight == nullptr)
				{
					constMap->spotLights[index].active = false;
				}

				// ライトが有効なら設定を転送
				else if ((*itSpotLight)->GetActive())
				{
					constMap->spotLights[index].active = true;
					constMap->spotLights[index].lightVec = -(*itSpotLight)->GetLightDir();
					constMap->spotLights[index].lightPos = (*itSpotLight)->GetLightPos();
					constMap->spotLights[index].lightColor = (*itSpotLight)->GetLightColor();
					constMap->spotLights[index].lightAtten = (*itSpotLight)->GetLightAtten();
					constMap->spotLights[index].lightFactorAngleCos = (*itSpotLight)->GetLightFactorAngle();
				}

				// ライトが無効なら設定を転送しない
				else constMap->spotLights[index].active = false;

				// インデックスとイテレータを進める
				++index;
				++itSpotLight;
			}
		}// 終
#pragma endregion

#pragma region 丸影
		{// 始
			std::forward_list<CircleShadow*>::iterator itCircleShadow = circleShadows_.begin();// イテレーター
			size_t index = 0;// インデックス
			while (index < CIRCLE_SHADOW_LIMIT && *itCircleShadow != nullptr)
			{
				if (*itCircleShadow == nullptr)
				{
					constMap->circleShadows[index].active = false;
				}

				// ライトが有効なら設定を転送
				else if ((*itCircleShadow)->GetActive())
				{
					constMap->circleShadows[index].active = true;
					constMap->circleShadows[index].dir = -(*itCircleShadow)->GetDir();
					constMap->circleShadows[index].casterPos = (*itCircleShadow)->GetCasterPos();
					constMap->circleShadows[index].distCasterLight = (*itCircleShadow)->GetDistCasterLight();
					constMap->circleShadows[index].atten = (*itCircleShadow)->GetAtten();
					constMap->circleShadows[index].factorAngleCos = (*itCircleShadow)->GetFactorAngleCos();
				}

				// ライトが無効なら設定を転送しない
				else constMap->circleShadows[index].active = false;

				// インデックスとイテレータを進める
				++index;
				++itCircleShadow;
			}
		}// 終
#pragma endregion

		// 環境光
		constMap->ambientColor = ambientColor_;

		// マッピング後処理
		constBuff_->Unmap(0, nullptr);
	}

	// 転送し終わったのでダーティフラグを[OFF]にする
	dirty_ = false;
}
