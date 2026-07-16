#pragma once
#include "Fngine.h"
#include "Structured.h"

enum class GPURenderMode {
    Instanced,         // 【草木・群集・通常パーティクル】1つのモデルを大量に複製して描画
    SingleMesh,        // 【布・海】すべての点をつなぎ合わせて、1枚の大きなメッシュとして描画
    IndirectInstanced  // 【寿命ありパーティクル】GPU側で描画数を動的に変えて描画
};

template <typename TForGPU>
class GPUComputeBaseModel
{
public:
    virtual ~GPUComputeBaseModel() = default;

    /// <summary>
    /// 
    /// </summary>
    /// <param name="engine"></param>
    /// <param name="numElements">要素数</param>
    /// <param name="renderMode">扱う種類</param>
    virtual void Initialize(Fngine* engine, uint32_t numElements, GPURenderMode renderMode = GPURenderMode::Instanced) {
        p_engine_ = engine;
        numElements_ = numElements;
        renderMode_ = renderMode;

        // GPU上で読み書きするための構造化バッファを生成
        dataBuffer_ = std::make_unique<RWStructured<TForGPU>>(p_engine_);
        dataBuffer_->Initialize(numElements_);

        // 「間接描画」モードの場合、描画引数をGPU側で書き換えるためのバッファを確保
        if (renderMode_ == GPURenderMode::IndirectInstanced) {
            InitializeIndirectBuffer();
        }

        // 初期化用のCompute Shaderを実行（GPU側で初期データを埋める）
        DispatchInitializeCS();
    }

    // 毎フレームのシミュレーション（物理計算など）
    virtual void Update() {
        // Compute Shaderを実行してGPU側でデータを更新
        DispatchUpdateCS();
    }

    // 描画コマンドの登録
    void Draw() {
        if (numElements_ == 0) return;

        // パイプライン設定や共通定数バッファのバインド
        if (SetCommand) {
            SetCommand();
        }
    }

protected:
    // 派生クラスで、それぞれのCompute Shader（CS）を起動する処理を書く
    virtual void DispatchInitializeCS() = 0;
    virtual void DispatchUpdateCS() = 0;

    using CallBack = std::function<void()>;
    // Draw時のコマンドリスト系
    CallBack SetCommand = nullptr;

private:
    // 間接描画（Indirect）用の引数バッファの初期化
    void InitializeIndirectBuffer() {
        // DirectX12などの「DrawIndexedInstanced」の引数（5つの32bit値）が入るバッファをGPU上に確保する
        indirectArgsBuffer_ = std::make_unique<RWStructured<int32_t>>(p_engine_);
        indirectArgsBuffer_->Initialize(1);
    }

protected:
    Fngine* p_engine_ = nullptr;

    // メインのGPU常駐バッファ（UAV / SRV両対応）
    std::unique_ptr<RWStructured<TForGPU>> dataBuffer_;
    uint32_t numElements_ = 0;
    GPURenderMode renderMode_ = GPURenderMode::Instanced;

    // 描画関連の設定（派生クラスで設定する）
    uint32_t indexCountPerInstance_ = 0; // インスタンス1個あたりのインデックス数
    uint32_t totalIndexCount_ = 0;        // メッシュ全体のインデックス数

    // 間接描画用のバッファとシグネチャ（DirectX12などの場合）
    std::unique_ptr<RWStructured<int32_t>> indirectArgsBuffer_;
};