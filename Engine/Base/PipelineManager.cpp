#include "PipelineManager.h"
#include "DX12Cmd.h"
#include "Texture.h"

std::map<std::string, std::unique_ptr<PipelineObj>> PipelineManager::pipelineObj_ = {};

PipelineManager* PipelineManager::GetInstance()
{
	// インスタンス生成
	static PipelineManager inst;

	// インスタンスを返す
	return &inst;
}

void PipelineManager::PreDraw(std::string pipelineName, D3D_PRIMITIVE_TOPOLOGY primitiveType, bool isDescHeap)
{
	// コマンドリスト取得
	ID3D12GraphicsCommandList* cmdList = DX12Cmd::GetInstance()->GetCmdList();

	// SRVヒープ取得
	ID3D12DescriptorHeap* srvHeap = Texture::GetInstance()->GetSRVHeap();

	// パイプラインステートの設定
	cmdList->SetPipelineState(pipelineObj_[pipelineName]->GetPipelineState());

	// ルートシグネチャの設定
	cmdList->SetGraphicsRootSignature(pipelineObj_[pipelineName]->GetRootSignature());

	// プリミティブ形状を設定
	cmdList->IASetPrimitiveTopology(primitiveType);

	// デスクリプタヒープの配列をセットするコマンド
	if (isDescHeap) {
		std::vector<ID3D12DescriptorHeap*> ppHeaps = { srvHeap };
		cmdList->SetDescriptorHeaps(1, ppHeaps.data());
	}
}

PipelineManager::PipelineManager() {
	AddPipeline("Object3D");
	pipelineObj_["Object3D"]->LoadShader("Resources/Shaders/Object3DPS.hlsl", PS);
	pipelineObj_["Object3D"]->LoadShader("Resources/Shaders/Object3DVS.hlsl", VS);
	pipelineObj_["Object3D"]->AddInputLayout("POSITION", DXGI_FORMAT_R32G32B32_FLOAT);
	pipelineObj_["Object3D"]->AddInputLayout("NORMAL", DXGI_FORMAT_R32G32B32_FLOAT);
	pipelineObj_["Object3D"]->AddInputLayout("TEXCOORD", DXGI_FORMAT_R32G32_FLOAT);
	pipelineObj_["Object3D"]->CreateRootParams(1, 3);
	pipelineObj_["Object3D"]->CreatePipeline(2, PipelineObj::ALPHA);

	AddPipeline("Line3D");
	pipelineObj_["Line3D"]->LoadShader("Resources/Shaders/Line3DPS.hlsl", PS);
	pipelineObj_["Line3D"]->LoadShader("Resources/Shaders/Line3DVS.hlsl", VS);
	pipelineObj_["Line3D"]->AddInputLayout("POSITION", DXGI_FORMAT_R32G32B32_FLOAT);
	pipelineObj_["Line3D"]->CreateRootParams(0, 1);
	//pipelineObj_["Line3D"]->CreatePipeline(1);
	pipelineObj_["Line3D"]->CreatePipeline(1, PipelineObj::NONE, D3D12_PRIMITIVE_TOPOLOGY_TYPE::D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE);

	AddPipeline("Toon");
	pipelineObj_["Toon"]->LoadShader("Resources/Shaders/ToonPS.hlsl", PS);
	pipelineObj_["Toon"]->LoadShader("Resources/Shaders/ToonVS.hlsl", VS);
	pipelineObj_["Toon"]->AddInputLayout("POSITION", DXGI_FORMAT_R32G32B32_FLOAT);
	pipelineObj_["Toon"]->AddInputLayout("NORMAL", DXGI_FORMAT_R32G32B32_FLOAT);
	pipelineObj_["Toon"]->AddInputLayout("TEXCOORD", DXGI_FORMAT_R32G32_FLOAT);
	pipelineObj_["Toon"]->CreateRootParams(1, 3);
	pipelineObj_["Toon"]->CreatePipeline(2);

	AddPipeline("Particle");
	pipelineObj_["Particle"]->LoadShader("Resources/Shaders/ParticlePS.hlsl", PS);
	pipelineObj_["Particle"]->LoadShader("Resources/Shaders/ParticleVS.hlsl", VS);
	pipelineObj_["Particle"]->LoadShader("Resources/Shaders/ParticleGS.hlsl", GS);
	pipelineObj_["Particle"]->AddInputLayout("POSITION", DXGI_FORMAT_R32G32B32_FLOAT);
	pipelineObj_["Particle"]->AddInputLayout("TEXCOORD", DXGI_FORMAT_R32_FLOAT);
	pipelineObj_["Particle"]->CreateRootParams(1, 1);
	pipelineObj_["Particle"]->CreatePipeline(2, PipelineObj::ADD,  D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT, false);

	AddPipeline("Sprite");
	pipelineObj_["Sprite"]->LoadShader("Resources/Shaders/SpritePS.hlsl", PS);
	pipelineObj_["Sprite"]->LoadShader("Resources/Shaders/SpriteVS.hlsl", VS);
	pipelineObj_["Sprite"]->AddInputLayout("POSITION", DXGI_FORMAT_R32G32_FLOAT);
	pipelineObj_["Sprite"]->AddInputLayout("TEXCOORD", DXGI_FORMAT_R32G32_FLOAT);
	pipelineObj_["Sprite"]->CreateRootParams(1, 1);
	pipelineObj_["Sprite"]->CreatePipeline(1, PipelineObj::ALPHA, D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE, false);

	AddPipeline("Gaussian");
	pipelineObj_["Gaussian"]->LoadShader("Resources/Shaders/GaussianPS.hlsl", PS);
	pipelineObj_["Gaussian"]->LoadShader("Resources/Shaders/GaussianVS.hlsl", VS);
	pipelineObj_["Gaussian"]->AddInputLayout("POSITION", DXGI_FORMAT_R32G32_FLOAT);
	pipelineObj_["Gaussian"]->AddInputLayout("TEXCOORD", DXGI_FORMAT_R32G32_FLOAT);
	pipelineObj_["Gaussian"]->CreateRootParams(2, 1);
	pipelineObj_["Gaussian"]->CreatePipeline(2);

	AddPipeline("HighLumi");
	pipelineObj_["HighLumi"]->LoadShader("Resources/Shaders/HighLumiPS.hlsl", PS);
	pipelineObj_["HighLumi"]->LoadShader("Resources/Shaders/HighLumiVS.hlsl", VS);
	pipelineObj_["HighLumi"]->AddInputLayout("POSITION", DXGI_FORMAT_R32G32_FLOAT);
	pipelineObj_["HighLumi"]->AddInputLayout("TEXCOORD", DXGI_FORMAT_R32G32_FLOAT);
	pipelineObj_["HighLumi"]->CreateRootParams(2, 1);
	pipelineObj_["HighLumi"]->CreatePipeline(2);

	AddPipeline("Bloom");
	pipelineObj_["Bloom"]->LoadShader("Resources/Shaders/BloomPS.hlsl", PS);
	pipelineObj_["Bloom"]->LoadShader("Resources/Shaders/BloomVS.hlsl", VS);
	pipelineObj_["Bloom"]->AddInputLayout("POSITION", DXGI_FORMAT_R32G32_FLOAT);
	pipelineObj_["Bloom"]->AddInputLayout("TEXCOORD", DXGI_FORMAT_R32G32_FLOAT);
	pipelineObj_["Bloom"]->CreateRootParams(2, 1);
	pipelineObj_["Bloom"]->CreatePipeline(1);
}

void PipelineManager::AddPipeline(std::string pipelineName)
{
	std::unique_ptr<PipelineObj> obj = std::make_unique<PipelineObj>();
	pipelineObj_.insert(std::make_pair(pipelineName, std::move(obj)));
}
