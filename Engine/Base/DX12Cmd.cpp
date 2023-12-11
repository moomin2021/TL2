#include "DX12Cmd.h"
#include "WinAPI.h"
#include "float4.h"
#include "Texture.h"

#include <cassert>
#include <thread>

#pragma comment(lib,"xaudio2.lib")
#pragma comment(lib, "Mf.lib")
#pragma comment(lib, "mfplat.lib")
#pragma comment(lib, "Mfreadwrite.lib")
#pragma comment(lib, "mfuuid.lib")
#pragma comment(lib,"dxgi.lib")
#pragma comment(lib,"dinput8.lib")
#pragma comment(lib,"dxguid.lib")
#pragma comment(lib, "xinput.lib")
#pragma comment(lib,"d3d12.lib")
#pragma comment(lib,"d3dcompiler.lib")
#pragma comment(lib,"xaudio2.lib")
#pragma comment(lib,"d3d12.lib")
#pragma comment(lib,"d3d12.lib")
#pragma comment(lib,"winmm.lib")

DX12Cmd* DX12Cmd::GetInstance()
{
	// インスタンス生成
	static DX12Cmd inst;

	// インスタンスを返す
	return &inst;
}

void DX12Cmd::Initialize()
{
	// 現在時間を記録する(FPS固定処理用)
	reference_ = std::chrono::steady_clock::now();

	// デバックレイヤー初期設定
	InitSetDebugLayer();

	// アダプタの初期設定
	InitSetAdapter();

	// デバイス生成
	CreateDevice();

	// エラー設定
	ErrorSet();

	// コマンドリスト生成
	CreateCmdList();

	// コマンドキュー生成
	CreateCmdQueue();

	// スワップチェーン生成
	CreateSwapChain();

	// レンダーターゲットビュー生成
	CreateRenderTargetView();

	// フェンス生成
	CreateFence();

	// 深度テスト
	CreateDepthBuffer();
}

void DX12Cmd::PreDraw()
{
	// インスタンス取得
	WinAPI* winAPI = WinAPI::GetInstance();

#pragma region リソースバリアで書き込み可能に変更
	// バックバッファの番号を取得(2つなので0番か1番)
	UINT bbIndex = swapChain_->GetCurrentBackBufferIndex();

	barrierDesc_.Transition.pResource = backBuffers_[bbIndex].Get();		// バックバッファを指定
	barrierDesc_.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;		// 表示状態から
	barrierDesc_.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;// 描画状態へ
	cmdList_->ResourceBarrier(1, &barrierDesc_);
#pragma endregion

#pragma region 描画先の変更

	// レンダーターゲットビューのハンドルを取得
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = rtvHeap_->GetCPUDescriptorHandleForHeapStart();
	rtvHandle.ptr += bbIndex * device_->GetDescriptorHandleIncrementSize(rtvHeapDesc_.Type);

	// 深度ステンシルビュー用デスクリプタヒープのハンドルを取得
	D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = dsvHeap_->GetCPUDescriptorHandleForHeapStart();
	cmdList_->OMSetRenderTargets(1, &rtvHandle, false, &dsvHandle);

#pragma endregion

#pragma region 画面クリア R G B A
	/// バックバッファには前回に描いた絵がそのまま残っているので、一旦指定色で塗りつぶす
	//std::vector<FLOAT> clearColor = { 0.1f, 0.25, 0.5f, 0.0f }; // 青っぽい色
	std::vector<FLOAT> clearColor = { 0.0f, 0.0f, 0.0f, 0.0f }; // 青っぽい色
	cmdList_->ClearRenderTargetView(rtvHandle, clearColor.data(), 0, nullptr);
	cmdList_->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);

#pragma endregion

#pragma region ビューポート設定

	// ビューポート設定コマンド
	D3D12_VIEWPORT viewport{};
	viewport.Width = winAPI->GetWidth();
	viewport.Height = winAPI->GetHeight();
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;

	// ビューポート設定コマンドを、コマンドリストに積む
	cmdList_->RSSetViewports(1, &viewport);

#pragma endregion

#pragma region シザー矩形

	// シザー矩形
	D3D12_RECT scissorRect{};
	scissorRect.left = 0; // 切り抜き座標左
	scissorRect.right = scissorRect.left + winAPI->GetWidth();	// 切り抜き座標右
	scissorRect.top = 0; // 切り抜き座標上
	scissorRect.bottom = scissorRect.top + winAPI->GetHeight();	// 切り抜き座標下

	// シザー矩形設定コマンドを、コマンドリストに積む
	cmdList_->RSSetScissorRects(1, &scissorRect);

#pragma endregion
}

void DX12Cmd::PostDraw()
{
	// 関数が成功したかどうかを判別する用変数
	HRESULT result;

#pragma region リソースバリアを戻す

	// バックバッファを書き込み可能状態から画面表示状態に変更する
	barrierDesc_.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;	// 描画状態から
	barrierDesc_.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;			// 表示状態へ
	cmdList_->ResourceBarrier(1, &barrierDesc_);

	// ここまでため込んだコマンドを実行し描画する処理
	{
		// 命令のクローズ
		result = cmdList_->Close();
		assert(SUCCEEDED(result));

		// コマンドリストの実行
		std::vector<ID3D12CommandList*> commandLists = { cmdList_.Get() };
		cmdQueue_->ExecuteCommandLists(1, commandLists.data());

		// 画面に表示するバッファをフリップ(裏表の入替え)
		result = swapChain_->Present(1, 0);
		assert(SUCCEEDED(result));
	}

	// コマンドの実行完了を待つ
	cmdQueue_->Signal(fence_.Get(), ++fenceVal_);
	if (fence_->GetCompletedValue() != fenceVal_)
	{
		HANDLE event = CreateEvent(nullptr, false, false, nullptr);
		fence_->SetEventOnCompletion(fenceVal_, event);
		WaitForSingleObject(event, INFINITE);
		CloseHandle(event);
	}

	// FPS固定
	UpdateFixFPS();

	// キューをクリア
	// 次の使用に備えてコマンドアロケータとコマンドリストをリセットしておく
	result = cmdAllocator_->Reset();
	assert(SUCCEEDED(result));

	// 再びコマンドリストを貯める準備
	result = cmdList_->Reset(cmdAllocator_.Get(), nullptr);
	assert(SUCCEEDED(result));

	Texture::GetInstance()->ReleaseIntermediateResources();

#pragma endregion

}

DX12Cmd::DX12Cmd() {}

void DX12Cmd::UpdateFixFPS()
{
	// 1/60秒ぴったりの時間
	const std::chrono::microseconds kMinTime(uint64_t(1000000.0f / 60.0f));

	// 1/60秒よりわずかに短い時間
	const std::chrono::microseconds kMinCheckTime(uint64_t(1000000.0f / 65.0f));

	// 現在時間を取得する
	std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();

	// 前回記録からの経過時間を取得する
	std::chrono::microseconds elapsed = std::chrono::duration_cast<std::chrono::microseconds>(now - reference_);

	// 1/60秒(よりわずかに短い時間)経っていない場合
	if (elapsed < kMinCheckTime) {
		// 1/60秒経過するまで微小なスリープを繰り返す
		while (std::chrono::steady_clock::now() - reference_ < kMinTime) {
			// 1マイクロ秒スリープ
			std::this_thread::sleep_for(std::chrono::microseconds(1));
		}
	}

	// 現在の時間を記録する
	reference_ = std::chrono::steady_clock::now();
}

void DX12Cmd::InitSetDebugLayer()
{
#ifdef _DEBUG
	// デバックコントロール
	ComPtr<ID3D12Debug1> debugController;
	if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)))) {
		debugController->EnableDebugLayer();				// デバックレイヤーを[ON]にする
		debugController->SetEnableGPUBasedValidation(TRUE);	// シェーダーチェックを[ON]にする

		// ※Debugで重くなったらシェーダーチェックを[OFF]にするといい
	}
#endif
}

void DX12Cmd::InitSetAdapter()
{
	// 関数が成功したかどうかを判別する用変数
	// ※DirectXの関数は、HRESULT型で成功したかどうかを返すものが多いのでこの変数を作成
	HRESULT result;

	result = CreateDXGIFactory(IID_PPV_ARGS(&dxgiFactory_));
	assert(SUCCEEDED(result));

	// アダプタの列挙用
	std::vector<ComPtr<IDXGIAdapter4>> adapters;

	// パフォーマンスが高いものから順に、すべてのアダプタを列挙する
	for (UINT i = 0;
		dxgiFactory_->EnumAdapterByGpuPreference(
			i, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE,
			IID_PPV_ARGS(&tmpAdapter_)) != DXGI_ERROR_NOT_FOUND; i++)
	{
		// 動的配列に追加する
		adapters.push_back(tmpAdapter_);
	}

	// 妥当なアダプタを選別する
	for (size_t i = 0; i < adapters.size(); i++) {
		DXGI_ADAPTER_DESC3 adapterDesc;

		// アダプタの情報を取得する
		adapters[i]->GetDesc3(&adapterDesc);

		// ソフトウェアデバイスを回避
		if (!(adapterDesc.Flags & DXGI_ADAPTER_FLAG3_SOFTWARE)) {
			// デバイスを採用してループを抜ける
			tmpAdapter_ = adapters[i];
			break;
		}
	}
}

void DX12Cmd::CreateDevice()
{
	// 関数が成功したかどうかを判別する用変数
	HRESULT result;

	// 対応レベルの配列
	std::vector<D3D_FEATURE_LEVEL> levels = {
		D3D_FEATURE_LEVEL_12_1,
		D3D_FEATURE_LEVEL_12_0,
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0
	};

	// グラフィックデバイスを操作する為のオブジェクト生成
	D3D_FEATURE_LEVEL featureLevel;
	// ※これは普通、1ゲームに1つしか作らない

	for (size_t i = 0; i < levels.size(); i++) {
		// 採用したアダプターでデバイスを生成
		result = D3D12CreateDevice(tmpAdapter_.Get(), levels[i],
			IID_PPV_ARGS(&device_));

		if (result == S_OK) {
			// デバイスを生成できた時点でループを抜ける
			featureLevel = levels[i];
			break;
		}
	}
}

void DX12Cmd::ErrorSet()
{
#ifdef _DEBUG
	ComPtr<ID3D12InfoQueue> infoQueue;
	if (SUCCEEDED(device_->QueryInterface(IID_PPV_ARGS(&infoQueue))))
	{
		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, true);	// やばいエラーの時止まる
		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, true);		// エラーの時止まる
		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, true);	// 警告の時止まる
	}

	// 抑制するエラー
	std::vector<D3D12_MESSAGE_ID> denyIds = {
		D3D12_MESSAGE_ID_RESOURCE_BARRIER_MISMATCHING_COMMAND_LIST_TYPE
	};

	// 抑制される表示レベル
	std::vector<D3D12_MESSAGE_SEVERITY> severities = { D3D12_MESSAGE_SEVERITY_INFO };
	D3D12_INFO_QUEUE_FILTER filter{};
	filter.DenyList.NumIDs = static_cast<UINT>(denyIds.size());
	filter.DenyList.pIDList = denyIds.data();
	filter.DenyList.NumSeverities = static_cast<UINT>(severities.size());
	filter.DenyList.pSeverityList = severities.data();

	// 指定したエラーの表示を抑制する
	infoQueue->PushStorageFilter(&filter);
#endif
}

void DX12Cmd::CreateCmdList() {
	// 関数が成功したかどうかを判別する用変数
	HRESULT result;

	// コマンドアロケータを生成
	result = device_->CreateCommandAllocator(
		D3D12_COMMAND_LIST_TYPE_DIRECT,	// コマンドアロケータの種類
		IID_PPV_ARGS(&cmdAllocator_));	// 各インターフェイス固有のGUID
	assert(SUCCEEDED(result));			// ID3D12CommandAllocatorインターフェイスのポインタを格納する変数アドレス

	// ※コマンドリストはコマンドアロケータから生成するので、先にコマンドアロケータを作る
	// ※コマンドリストに格納する命令の為のメモリを管理するオブジェクト

	// コマンドリストを生成
	result = device_->CreateCommandList(0,
		D3D12_COMMAND_LIST_TYPE_DIRECT,
		cmdAllocator_.Get(), nullptr,
		IID_PPV_ARGS(&cmdList_));
	assert(SUCCEEDED(result));
}

void DX12Cmd::CreateCmdQueue() {
	// 関数が成功したかどうかを判別する用変数
	HRESULT result;

	// コマンドキューの設定
	D3D12_COMMAND_QUEUE_DESC cmdQueueDesc{};

	// ※{}をつけることで構造体の中身を0でクリアしている。
	// ※値0が標準値になるようにMicrosoftが決めているので、この場合コマンドキューの設定を標準にしている //

	// 標準設定でコマンドキューを生成
	result = device_->CreateCommandQueue(&cmdQueueDesc, IID_PPV_ARGS(&cmdQueue_));
	assert(SUCCEEDED(result));
}

void DX12Cmd::CreateSwapChain() {
	// 関数が成功したかどうかを判別する用変数
	HRESULT result;

	// WinAPI
	WinAPI* winAPI = WinAPI::GetInstance();

	// 画面の幅指定
	swapChainDesc_.Width = winAPI->GetWidth();	// 画面の横幅を指定する
	swapChainDesc_.Height = winAPI->GetHeight();// 画面の縦幅を指定する

	// 色情報の書式（表示形式）
	swapChainDesc_.Format = DXGI_FORMAT_R8G8B8A8_UNORM;//※DXGI_FORMAT_R8G8B8A8_UNORMはアルファを含むチャンネルあたり8ビットをサポート

	// ピクセルあたりのマルチサンプルの数を指定する
	swapChainDesc_.SampleDesc.Count = 1;

	// リソースの使用方法を指定
	swapChainDesc_.BufferUsage = DXGI_USAGE_BACK_BUFFER;// ※DXGI_USAGE_BACK_BUFFERはリソースをバックバッファとして使用する

	// スワップチェーン内のバッファーの数を指定する
	swapChainDesc_.BufferCount = 2;

	// 画面をスワップした後のリソースの処理方法を指定
	swapChainDesc_.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;// ※DXGI_SWAP_EFFECT_FLIP_DISCARDはスワップした後バックバッファーの内容を破棄する設定

	// スワップチェーン動作のオプションを指定
	swapChainDesc_.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	// ※DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCHはウィンドウとフルスクリーンの切り替え時に>>
	// >>解像度がウィンドウサイズに一致するように変更する

	// IDXGISwapChain1のComPtrを用意
	ComPtr<IDXGISwapChain1> swapChain1;

	// スワップチェーンの生成
	result = dxgiFactory_->CreateSwapChainForHwnd(
		cmdQueue_.Get(), winAPI->GetHWND(), &swapChainDesc_, nullptr, nullptr,
		(IDXGISwapChain1**)&swapChain1);
	assert(SUCCEEDED(result));

	// 生成したIDXGISwapChain1のオブジェクトをIDXGISwapChain4に変換する
	swapChain1.As(&swapChain_);
}

void DX12Cmd::CreateRenderTargetView()
{
	// ※レンダーターゲットビューはデスクリプタヒープに生成するので、先にデスクリプタヒープを作る

	// デスクリプタヒープの設定
	rtvHeapDesc_.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV; // レンダーターゲットビュー
	rtvHeapDesc_.NumDescriptors = swapChainDesc_.BufferCount; // 裏表の2つ

	// デスクリプタヒープの生成
	device_->CreateDescriptorHeap(&rtvHeapDesc_, IID_PPV_ARGS(&rtvHeap_));

	// バックバッファ
	backBuffers_.resize(swapChainDesc_.BufferCount);

	// スワップチェーンの全てのバッファについて処理する
	for (size_t i = 0; i < backBuffers_.size(); i++)
	{
		// スワップチェーンからバッファを取得
		swapChain_->GetBuffer((UINT)i, IID_PPV_ARGS(&backBuffers_[i]));

		// デスクリプタヒープのハンドルを取得
		D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = rtvHeap_->GetCPUDescriptorHandleForHeapStart();

		// 裏か表かでアドレスがずれる
		rtvHandle.ptr += i * device_->GetDescriptorHandleIncrementSize(rtvHeapDesc_.Type);

		// シェーダーの計算結果をSRGBに変換して書き込む
		rtvDesc_.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
		rtvDesc_.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;

		// レンダーターゲットビューの生成
		device_->CreateRenderTargetView(backBuffers_[i].Get(), &rtvDesc_, rtvHandle);
	}
}

void DX12Cmd::CreateFence()
{
	// 関数が成功したかどうかを判別する用変数
	HRESULT result;

	// フェンス生成
	result = device_->CreateFence(fenceVal_, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence_));
}

void DX12Cmd::CreateDepthBuffer()
{
	// 関数が成功したかどうかを判別する用変数
	HRESULT result;

	// WinAPI
	WinAPI* winAPI = WinAPI::GetInstance();

	// リソース設定
	D3D12_RESOURCE_DESC depthResourceDesc{};
	depthResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	depthResourceDesc.Width = winAPI->GetWidth();	// レンダーターゲットに合わせる
	depthResourceDesc.Height = winAPI->GetHeight();	// レンダーターゲットに合わせる
	depthResourceDesc.DepthOrArraySize = 1;
	depthResourceDesc.Format = DXGI_FORMAT_D32_FLOAT;// 深度値フォーマット
	depthResourceDesc.SampleDesc.Count = 1;
	depthResourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;// デプスステンシル

	// 深度値用ヒーププロパティ
	D3D12_HEAP_PROPERTIES depthHeapProp{};
	depthHeapProp.Type = D3D12_HEAP_TYPE_DEFAULT;

	// 深度値のクリア設定
	D3D12_CLEAR_VALUE depthClearValue{};
	depthClearValue.DepthStencil.Depth = 1.0f;		// 深度値1.0f（最大値）でクリア
	depthClearValue.Format = DXGI_FORMAT_D32_FLOAT;	// 深度値フォーマット

	// リソース生成
	result = device_->CreateCommittedResource(
		&depthHeapProp,
		D3D12_HEAP_FLAG_NONE,
		&depthResourceDesc,
		D3D12_RESOURCE_STATE_DEPTH_WRITE,// 深度値書き込みに使用
		&depthClearValue,
		IID_PPV_ARGS(&depthBuff_)
	);

	// 深度ビュー用デスクリプタヒープ作成
	D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc{};
	dsvHeapDesc.NumDescriptors = 1;						// 深度ビューは1つ
	dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;	// デプスステンシルビュー
	result = device_->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(&dsvHeap_));

	// 深度ビュー作成
	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc{};
	dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;// 深度値フォーマット
	dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	device_->CreateDepthStencilView(
		depthBuff_.Get(),
		&dsvDesc,
		dsvHeap_->GetCPUDescriptorHandleForHeapStart()
	);
}
