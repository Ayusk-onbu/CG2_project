#include "SDFManager.h"
#include "PipelineStateObjectManager.h"
#include "Constant.h"

void SDFManager::Initialize(Fngine* fngine) {
    pFngine_ = fngine;
}

bool SDFManager::LoadAndBake(
    ID3D12GraphicsCommandList* commandList,
    const std::string& modelID,
    uint32_t resolution)
{
    auto modelMgr = ModelManager::GetInstance();

    // 1. ModelManager から BVH を取得
    const BVH* bvh = modelMgr->GetBVH(modelID);
    if (!bvh) {
        Log::View("\n[SDFManager] Failed: BVH not found for ID: " + modelID);
        return false;
    }

    // 2. ModelManager から ModelData を取得して AABB (Min/Max) を自動計算
    const auto& modelData = modelMgr->LoadModelData(modelID);
    if (modelData.vertices.empty()) {
        Log::View("\n[SDFManager] Failed: Vertices empty for ID: " + modelID);
        return false;
    }

    Vector3 minBounds(FLT_MAX, FLT_MAX, FLT_MAX);
    Vector3 maxBounds(-FLT_MAX, -FLT_MAX, -FLT_MAX);

    for (const auto& vertex : modelData.vertices) {
        minBounds.x = (std::min)(minBounds.x, vertex.position.x);
        minBounds.y = (std::min)(minBounds.y, vertex.position.y);
        minBounds.z = (std::min)(minBounds.z, vertex.position.z);
        maxBounds.x = (std::max)(maxBounds.x, vertex.position.x);
        maxBounds.y = (std::max)(maxBounds.y, vertex.position.y);
        maxBounds.z = (std::max)(maxBounds.z, vertex.position.z);
    }

    // 3. 準備できたデータを渡してベイク処理
    return BakeAndRegister(commandList, modelID, bvh, minBounds, maxBounds, resolution);
}

bool SDFManager::BakeAndRegister(
    ID3D12GraphicsCommandList* commandList,
    const std::string& modelID,
    const BVH* bvh,
    const Vector3& modelMinBounds,
    const Vector3& modelMaxBounds,
    uint32_t resolution)
{
    if (!bvh) return false;

    // 1. SDFData の生成
    auto sdfData = std::make_unique<SDFData>();
    sdfData->resolution = resolution;

    // モデルのAABBより少し広めのマージンを取る（10%広げる）
    Vector3 margin = (modelMaxBounds - modelMinBounds) * 0.1f;
    sdfData->gridMin = modelMinBounds - margin;
    sdfData->gridMax = modelMaxBounds + margin;

    // 3Dテクスチャ初期化 (Width, Height, Depth)
    sdfData->texture.Initialize(pFngine_, resolution, resolution, resolution);
    sdfData->texture.GetResource()->SetName(L"SDFData");

    // 2. BVHの平坦化 ＆ GPU用バッファ作成
    std::vector<GPUBVHNode> gpuNodes;
    std::vector<GPUPolygon> gpuPolygons;
    bvh->Flatten(gpuNodes, gpuPolygons);

    if (gpuNodes.empty() || gpuPolygons.empty()) return false;

    // =========================================================
    // 【修正】 メンバ変数として生成・データ更新する
    // =========================================================
    nodeBuffer_ = std::make_unique<Structured<GPUBVHNode>>(pFngine_);
    nodeBuffer_->Initialize(static_cast<uint32_t>(gpuNodes.size()));
    std::memcpy(nodeBuffer_->GetMappedData(), gpuNodes.data(), sizeof(GPUBVHNode) * gpuNodes.size());

    polygonBuffer_ = std::make_unique<Structured<GPUPolygon>>(pFngine_);
    polygonBuffer_->Initialize(static_cast<uint32_t>(gpuPolygons.size()));
    std::memcpy(polygonBuffer_->GetMappedData(), gpuPolygons.data(), sizeof(GPUPolygon) * gpuPolygons.size());

    configBuffer_ = std::make_unique<ConstantBuffer<SDFBakeConfig>>(pFngine_);
    configBuffer_->Initialize();
    configBuffer_->GetMappedData()->gridMin = sdfData->gridMin;
    configBuffer_->GetMappedData()->gridMax = sdfData->gridMax;
    configBuffer_->GetMappedData()->resolution[0] = resolution;
    configBuffer_->GetMappedData()->resolution[1] = resolution;
    configBuffer_->GetMappedData()->resolution[2] = resolution;
    configBuffer_->GetMappedData()->totalNodes = static_cast<uint32_t>(gpuNodes.size());

    ID3D12DescriptorHeap* ppHeaps[] = { SRVManager::GetInstance()->GetHeap() };
    // コマンドリストにヒープをセット（テーブルをセットするより【前】に呼ぶ！）
    commandList->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);

    D3D12_RESOURCE_BARRIER preBarrier{};
    preBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    preBarrier.Transition.pResource = sdfData->texture.GetResource();
    preBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COMMON;
    preBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_UNORDERED_ACCESS; // UAV書き込み可能状態へ
    preBarrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

    commandList->ResourceBarrier(1, &preBarrier);

    // 3. Dispatch 実行
    auto PSO = PSOManager::GetInstance();
    commandList->SetComputeRootSignature(PSO->GetPSO("BakeSDF.CS").GetRootSignature().GetRS().Get());
    commandList->SetPipelineState(PSO->GetPSO("BakeSDF.CS").GetCPS().Get());

    commandList->SetComputeRootDescriptorTable(0, nodeBuffer_->GetSRVHandleGPU());
    commandList->SetComputeRootDescriptorTable(1, polygonBuffer_->GetSRVHandleGPU());
    commandList->SetComputeRootDescriptorTable(2, sdfData->texture.GetUAVHandleGPU());
    commandList->SetComputeRootConstantBufferView(3, configBuffer_->GetGPUVirtualAddress());

    // Dispatch (8x8x8スレッドグループ前提)
    uint32_t group = (resolution + 7) / 8;
    commandList->Dispatch(group, group, group);

    // 4. バリア設定 (書き込みUAV状態から読み込みSRV状態へ遷移)
    D3D12_RESOURCE_BARRIER barrier{};
    barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    barrier.Transition.pResource = sdfData->texture.GetResource();
    barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
    barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE;
    barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

    commandList->ResourceBarrier(1, &barrier);

    // 5. マップに保存
    sdfMap_[modelID] = std::move(sdfData);
    Log::View("SDF Successfully Baked & Registered for Model: " + modelID);

    return true;
}

const SDFData* SDFManager::GetSDF(const std::string& modelID) const {
    auto it = sdfMap_.find(modelID);
    if (it != sdfMap_.end()) {
        return it->second.get();
    }
    return nullptr;
}

void SDFManager::ExportToDDS(
    ID3D12GraphicsCommandList* commandList,
    ID3D12CommandQueue* commandQueue, // GPU完了を待つために使用
    const std::string& modelID,
    const std::wstring& outputFilePath)
{
    const SDFData* sdfData = GetSDF(modelID);
    if (!sdfData) {
        Log::View("[SDFManager] Export failed: Model ID not found: " + modelID);
        return;
    }

    auto device = pFngine_->GetD3D12System().GetDevice().Get();
    ID3D12Resource* srcResource = sdfData->texture.GetResource();
    uint32_t resolution = sdfData->resolution;

    // -------------------------------------------------------------
    // Step 1. GPUメモリ上のレイアウト（RowPitch, SlicePitch）を取得
    // -------------------------------------------------------------
    D3D12_RESOURCE_DESC resDesc = srcResource->GetDesc();
    D3D12_PLACED_SUBRESOURCE_FOOTPRINT footprint{};
    UINT numRows = 0;
    UINT64 rowSizeInBytes = 0;
    UINT64 totalBytes = 0;

    device->GetCopyableFootprints(&resDesc, 0, 1, 0, &footprint, &numRows, &rowSizeInBytes, &totalBytes);

    // -------------------------------------------------------------
    // Step 2. Readback バッファの作成
    // -------------------------------------------------------------
    D3D12_HEAP_PROPERTIES heapProps{};
    heapProps.Type = D3D12_HEAP_TYPE_READBACK;

    D3D12_RESOURCE_DESC bufferDesc{};
    bufferDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
    bufferDesc.Width = totalBytes;
    bufferDesc.Height = 1;
    bufferDesc.DepthOrArraySize = 1;
    bufferDesc.MipLevels = 1;
    bufferDesc.Format = DXGI_FORMAT_UNKNOWN;
    bufferDesc.SampleDesc.Count = 1;
    bufferDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

    Microsoft::WRL::ComPtr<ID3D12Resource> readbackBuffer;
    device->CreateCommittedResource(
        &heapProps, D3D12_HEAP_FLAG_NONE, &bufferDesc,
        D3D12_RESOURCE_STATE_COPY_DEST, nullptr, IID_PPV_ARGS(&readbackBuffer));

    // -------------------------------------------------------------
    // Step 3. 3Dテクスチャから Readback バッファへ転送 (GPUコマンド)
    // -------------------------------------------------------------
    // バリア: SRV/UAV 状態 -> COPY_SOURCE 状態
    D3D12_RESOURCE_BARRIER barrier{};
    barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    barrier.Transition.pResource = srcResource;
    barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE;
    barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_COPY_SOURCE;
    barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
    commandList->ResourceBarrier(1, &barrier);

    // コピー指定
    D3D12_TEXTURE_COPY_LOCATION dstLoc{};
    dstLoc.pResource = readbackBuffer.Get();
    dstLoc.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
    dstLoc.PlacedFootprint = footprint;

    D3D12_TEXTURE_COPY_LOCATION srcLoc{};
    srcLoc.pResource = srcResource;
    srcLoc.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
    srcLoc.SubresourceIndex = 0;

    commandList->CopyTextureRegion(&dstLoc, 0, 0, 0, &srcLoc, nullptr);

    // バリア戻し
    barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_SOURCE;
    barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE;
    commandList->ResourceBarrier(1, &barrier);

    // -------------------------------------------------------------
    // Step 4. コマンドリストを閉じて実行＆完了待ち (Fence Sync)
    // -------------------------------------------------------------
    // ※ 既存のエンジン側のコマンドリスト実行 ＆ GPU完了待ち関数があれば読み替えてください
    commandList->Close();
    ID3D12CommandList* ppCommandLists[] = { commandList };
    commandQueue->ExecuteCommandLists(1, ppCommandLists);

    // 同期用 Fence で GPU の処理完了を完全に待つ
    Microsoft::WRL::ComPtr<ID3D12Fence> fence;
    device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence));
    commandQueue->Signal(fence.Get(), 1);

    HANDLE eventHandle = CreateEvent(nullptr, FALSE, FALSE, nullptr);
    if (fence->GetCompletedValue() < 1) {
        fence->SetEventOnCompletion(1, eventHandle);
        WaitForSingleObject(eventHandle, INFINITE);
    }
    CloseHandle(eventHandle);

    // -------------------------------------------------------------
    // Step 5. CPUでデータを詰めて DirectXTex で DDS に保存
    // -------------------------------------------------------------
    BYTE* mappedData = nullptr;
    readbackBuffer->Map(0, nullptr, reinterpret_cast<void**>(&mappedData));

    // 3D Image コンテナを作成
    DirectX::ScratchImage image;
    image.Initialize3D(DXGI_FORMAT_R32_FLOAT, resolution, resolution, resolution, 1);

    const DirectX::Image* img = image.GetImage(0, 0, 0);
    BYTE* dstPixels = img->pixels;

    // D3D12のパディング（256バイト境界アライメント）を除去しながらコピー
    size_t srcSlicePitch = footprint.Footprint.RowPitch * resolution;
    size_t dstRowPitch = resolution * sizeof(float);
    size_t dstSlicePitch = dstRowPitch * resolution;

    for (uint32_t z = 0; z < resolution; ++z) {
        for (uint32_t y = 0; y < resolution; ++y) {
            BYTE* srcPtr = mappedData + (z * srcSlicePitch) + (y * footprint.Footprint.RowPitch);
            BYTE* dstPtr = dstPixels + (z * dstSlicePitch) + (y * dstRowPitch);
            std::memcpy(dstPtr, srcPtr, dstRowPitch);
        }
    }

    readbackBuffer->Unmap(0, nullptr);

    // DDS ファイルとして書き出し！！
    HRESULT hr = DirectX::SaveToDDSFile(
        image.GetImages(), image.GetImageCount(), image.GetMetadata(),
        DirectX::DDS_FLAGS_NONE, outputFilePath.c_str());

    if (SUCCEEDED(hr)) {
        Log::View("[SDFManager] Successfully exported DDS to file!");
    }
    else {
        Log::View("[SDFManager] Failed to save DDS file.");
    }
}