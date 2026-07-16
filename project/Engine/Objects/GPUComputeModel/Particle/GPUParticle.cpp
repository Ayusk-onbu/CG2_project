#include "GPUParticle.h"
#include "ModelManager.h"
#include <d3dx12.h>

void GPUParticleSystem::Initialize(Fngine* engine, uint32_t numParticles) {
    // 通常のインスタンス描画モードで初期化
    GPUComputeBaseModel::Initialize(engine, numParticles, GPURenderMode::Instanced);

    // 定数バッファの生成など
    perViewBuffer_ = std::make_unique<ConstantBuffer<PerView>>(p_engine_);
    perViewBuffer_->Initialize();

    SetCommand = [this]() {
        auto commandList = p_engine_->GetCommand().GetList().GetList();

        auto preUAVBarrier = CD3DX12_RESOURCE_BARRIER::Transition(
            dataBuffer_->GetResource(),
            D3D12_RESOURCE_STATE_UNORDERED_ACCESS,
            D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);
        commandList->ResourceBarrier(1, &preUAVBarrier);

        auto object = ModelManager::GetInstance()->LoadObjectData("Plane");

        commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        commandList->SetGraphicsRootSignature(PSOManager::GetInstance()->GetPSO("GPUParticle").GetRootSignature().GetRS().Get());
        commandList->SetPipelineState(PSOManager::GetInstance()->GetPSO("GPUParticle").GetGPS().Get());
        commandList->IASetVertexBuffers(0, 1, &object.GetVertexBufferView());
        commandList->IASetIndexBuffer(&object.GetIndexBufferView());

        commandList->SetGraphicsRootConstantBufferView(0, perViewBuffer_->GetGPUVirtualAddress());
        commandList->SetGraphicsRootDescriptorTable(1, dataBuffer_->GetSRVHandleGPU()); 
        commandList->SetGraphicsRootDescriptorTable(2, TextureManager::GetInstance()->GetTexture("GridLine").GetHandleGPU());

        commandList->DrawIndexedInstanced(object.GetIndexCount(), dataBuffer_->GetNumElements(), 0, 0, 0);

        auto postUAVBarrier = CD3DX12_RESOURCE_BARRIER::Transition(
            dataBuffer_->GetResource(),
            D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE,
            D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
        commandList->ResourceBarrier(1, &postUAVBarrier);
    };
}

void GPUParticleSystem::Update(float deltaTime) {
    // 定数バッファの更新 (経過時間やエミッター位置をGPUに送る)
    auto camera = CameraSystem::GetInstance()->GetActiveCamera();
    perViewBuffer_->GetMappedData()->viewProjection = camera->GetViewProjectionMatrix();
    perViewBuffer_->GetMappedData()->billboardMatrix = Matrix4x4::Make::Identity();

    // 基底クラスのUpdate（Compute Shader起動）をキック
    GPUComputeBaseModel::Update();
}

void GPUParticleSystem::DispatchInitializeCS(){
    auto commandList = p_engine_->GetCommand().GetList().GetList();
    ID3D12DescriptorHeap* ppHeaps[] = { p_engine_->GetSRV().GetDescriptorHeap().GetHeap().Get() };
    // コマンドリストにヒープをセット（テーブルをセットするより【前】に呼ぶ！）
    commandList->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);

    commandList->SetComputeRootSignature(PSOManager::GetInstance()->GetPSO("InitializeParticle.CS").GetRootSignature().GetRS().Get());
    commandList->SetPipelineState(PSOManager::GetInstance()->GetPSO("InitializeParticle.CS").GetCPS().Get());

    commandList->SetComputeRootDescriptorTable(0, dataBuffer_->GetUAVHandleGPU());
    // 今回はちょうど1024個なので
    commandList->Dispatch(1,1,1);
}

void GPUParticleSystem::DispatchUpdateCS(){
    //auto commandList = p_engine_->GetCommandList();

    // 1. パイプライン(PSO)に更新用Compute Shaderをセット
    // commandList->SetPipelineState(particleUpdatePSO_);

    // 2. 定数バッファと構造化バッファ(UAV)をバインド
    // commandList->SetComputeRootDescriptorTable(0, constantBuffer_->GetGPUHandle());
    // commandList->SetComputeRootDescriptorTable(1, dataBuffer_->GetUAVHandle());

    // 3. スレッドを起動！ (1024スレッドずつグループ分けして計算)
    //uint32_t groupCount = (numElements_ + 1023) / 1024;
   // commandList->Dispatch(groupCount, 1, 1);
}