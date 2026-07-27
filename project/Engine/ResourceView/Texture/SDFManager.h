#pragma once
#include "Structured.h"
#include "ModelManager.h"

struct SDFData {
    RWTexture3D texture;
    Vector3 gridMin;
    Vector3 gridMax;
    uint32_t resolution;
};

struct SDFBakeConfig
{
    Vector3 gridMin;     // SDFバウンディングボックスの最小点
    float  pad0;
    Vector3 gridMax;     // SDFバウンディングボックスの最大点
    float  pad1;
    uint32_t resolution[3];  // テクスチャ解像度 (例: 64, 64, 64)
    uint32_t totalNodes;
};

class SDFManager : public ISingleton<SDFManager>
{
    friend class ISingleton<SDFManager>;
public:
    void Initialize(Fngine* fngine);

    /// <summary>
    /// ModelIDだけ渡せば、ModelManagerからBVHと頂点を勝手に取得してSDFをベイクする！
    /// </summary>
    bool LoadAndBake(
        ID3D12GraphicsCommandList* commandList,
        const std::string& modelID,
        uint32_t resolution = 64
    );

    /// <summary>
    /// モデルのBVHからSDFをベイクして管理下に登録する
    /// </summary>
    bool BakeAndRegister(
        ID3D12GraphicsCommandList* commandList,
        const std::string& modelID,
        const BVH* bvh,
        const Vector3& modelMinBounds,
        const Vector3& modelMaxBounds,
        uint32_t resolution = 64
    );

    void ExportToDDS(
        ID3D12GraphicsCommandList* commandList,
        ID3D12CommandQueue* commandQueue, // GPU完了を待つために使用
        const std::string& modelID,
        const std::wstring& outputFilePath);

    /// <summary>
    /// 指定したモデルのSDFデータを取得する
    /// </summary>
    const SDFData* GetSDF(const std::string& modelID) const;

private:
    Fngine* pFngine_ = nullptr;

public:
    // IDごとのSDFデータ構造体マップ
    std::unordered_map<std::string, std::unique_ptr<SDFData>> sdfMap_;
    std::unique_ptr<Structured<GPUBVHNode>> nodeBuffer_;
    std::unique_ptr<Structured<GPUPolygon>> polygonBuffer_;
    std::unique_ptr<ConstantBuffer<SDFBakeConfig>> configBuffer_;
};