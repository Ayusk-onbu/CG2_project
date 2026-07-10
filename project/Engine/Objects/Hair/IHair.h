#pragma once
#include "GuideCurve.h"
#include <memory>
#include "Fngine.h"
#include "Structured.h"
#include "Constant.h"

struct HairCamera {
	Matrix4x4 inverseProjView; // カメラの逆射影ビュー行列
	Vector3 cameraPosition;     // カメラの位置
	float padding;             // 4の倍数にするためのパディング
};

// 全てのGuideは違う頂点数が可能であり、
// 全てのStrandsは違う頂点数が可能である。
// 以上のことから、和が国家はStrandsInfoとGuideInfoをセーブする必要がある。

class Hair {
public:
	void Initialize(Fngine* engine, bool isLoad, const Strands::HairSaveData& savedata);
	void Update(float deltaTime, const Matrix4x4& mat);
	
	void Render(D3D12_GPU_DESCRIPTOR_HANDLE depthHandle);
	void PreHair(ID3D12Resource* randerTargetResource, D3D12_RESOURCE_STATES preState, ID3D12Resource* depthResource);
	void PostHair(ID3D12Resource* randerTargetResource, D3D12_RESOURCE_STATES postState, ID3D12Resource* depthResource);
public:

	void RequestNotifyUpdate() {
		isGpuUpdateRequested_ = true;
	}
	GuideCurve::ControllerPoint* GetCPUGuideData() {
		return uploadGuideBuffer->GetMappedData();
	}
	Strands::HairConfig* GetCPUGuideConfig() {
		return gpuConfigBuffer_->GetMappedData();
	}
	// エディタ側で Slider の最大値を決めるために、要素数も一緒に取得できるようにする
	uint32_t GetCPUGuideCount() const {
		return uploadGuideBuffer->GetNumElements();
	}
	uint32_t GetCPUActiveGuideCount() const {
		uint32_t count = 0;
		if (guideInfoBuffer_) {
			for (uint32_t i = 0; i < guideInfoBuffer_->GetNumElements(); ++i) {
				if (guideInfoBuffer_->GetMappedData()[i].vertexCount > 0) {
					count += guideInfoBuffer_->GetMappedData()[i].vertexCount;
				}
				else {
					break;
				}
			}
		}
		return count;
	}

	GuideCurve::GuideInfo* GetCPUGuideInfoData() {
		return guideInfoBuffer_ ? guideInfoBuffer_->GetMappedData() : nullptr;
	}
	uint32_t GetCPUGuideInfoCount() const {
		return guideInfoBuffer_ ? guideInfoBuffer_->GetNumElements() : 0;
	}
	Strands::StrandInfo* GetCPUStrandInfoData() {
		return strandInfoBuffer_ ? strandInfoBuffer_->GetMappedData() : nullptr;
	}
	uint32_t GetCPUStrandInfoCount() const {
		return strandInfoBuffer_ ? strandInfoBuffer_->GetNumElements() : 0;
	}

	Strands::SegmentData* GetCPUSegmentData() {
		return segmentBuffer_ ? segmentBuffer_->GetMappedData() : nullptr;
	}

	Strands::ChildStrand* GetCPUChildStrandData() {
		return gpuChildStrandBuffer_ ? gpuChildStrandBuffer_->GetMappedData() : nullptr;
	}

	GuideCurve::HairPhysicsConfig* GetCPUPhysicsConfig() {
		return gpuPhysicsConfigBuffer_ ? gpuPhysicsConfigBuffer_->GetMappedData() : nullptr;
	}
	Strands::HairMakeConfig* GetCPUMakeConfig() {
		return gpuMakeConfigBuffer_ ? gpuMakeConfigBuffer_->GetMappedData() : nullptr;
	}
private:
	Microsoft::WRL::ComPtr<ID3D12StateObject> CreateHairRaytracingPSO(
		ID3D12Device5* device, // DXR対応のDevice5が必要
		ID3D12RootSignature* globalRootSignature,
		const void* shaderByteCode,
		SIZE_T shaderByteCodeSize);

private:
	Fngine* engine_ = nullptr;

	// コマンドリストとパイプライン
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList4> dxrCmdList_; // QueryInterfaceしたやつ
	Microsoft::WRL::ComPtr<ID3D12RootSignature> globalRootSignature_;
	Microsoft::WRL::ComPtr<ID3D12StateObject> rtpso_;
	Microsoft::WRL::ComPtr<IDxcBlob> hairShaderBlob_;

private:
	GuideCurve::GuideHear guide_;
	bool isGpuUpdateRequested_ = false; // フラグ

	GuideCurve::Main hairData_;
	//【定数バッファ】
	std::unique_ptr<ConstantBuffer<GuideCurve::HairPhysicsConfig>>gpuPhysicsConfigBuffer_;// 髪全体の情報その物理
	std::unique_ptr<ConstantBuffer<GuideCurve::FrameConfig>>gpuFrameConfigBuffer_;// 髪全体の情報その物理
	std::unique_ptr<ConstantBuffer<Strands::HairConfig>>gpuConfigBuffer_;// 髪全体の情報その性質
	std::unique_ptr<ConstantBuffer<Strands::HairMakeConfig>>gpuMakeConfigBuffer_;// 髪全体の情報その性質

	std::unique_ptr<Structured<GuideCurve::ControllerPoint>> uploadGuideBuffer;// データ転送用バッファ
	std::unique_ptr<Structured<GuideCurve::GuideInfo>>guideInfoBuffer_;
	std::unique_ptr<Structured<Strands::ChildStrand>>gpuChildStrandBuffer_;// ストランド生成に必要な情報
	std::unique_ptr<Structured<Strands::StrandInfo>> strandInfoBuffer_;
	
	std::unique_ptr<Structured<Strands::SegmentData>> segmentBuffer_; // レイトレ＆AABB用
	std::unique_ptr<RWStructured<GuideCurve::ControllerPoint>>gpuGuideBuffer_;// 全てのガイドの情報
	std::unique_ptr<RWStructured<Strands::StrandVertex>> hairVertexBuffer_;
	
	std::unique_ptr<Structured<D3D12_RAYTRACING_INSTANCE_DESC>> instanceBuffer;
	
	std::unique_ptr<RWStructured<Strands::StrandVertex>> hairTriangleBuffer_;

	// 加速構造体（BLAS）用のリソース
	Microsoft::WRL::ComPtr<ID3D12Resource> scratchBuffer_;
	Microsoft::WRL::ComPtr<ID3D12Resource> blasResultBuffer_;
	
	// 加速構造体（TLAS）用のリソース：これは世界で一個のみかな
	Microsoft::WRL::ComPtr<ID3D12Resource> tlasScratchBuffer_;
	Microsoft::WRL::ComPtr<ID3D12Resource> tlasResultBuffer_;

	// シェーダーバイディングテーブル（SBT）用のリソース（※名前は環境に合わせて調整してね）
	Microsoft::WRL::ComPtr<ID3D12Resource> sbtBuffer_;

	// ディスパッチの設定を保持する構造体
	D3D12_DISPATCH_RAYS_DESC dispatchDesc_{};

	// 出力用UAVのGPUハンドル
	D3D12_GPU_DESCRIPTOR_HANDLE renderTargetUAVHandleGPU_{};
	std::unique_ptr<RWTexture2D> outputTexture_;

	std::unique_ptr<ConstantBuffer<HairCamera>> hairCameraBuffer_;

public:
	// 追従させているキャラクターの行列
	Matrix4x4 characterMatrix_;

public:
	/**
     * @brief 現在の巨大な固定長バッファから、アクティブなデータのみをバイナリとして保存する
     * @param filename 保存先ファイルパス
     * @return 成功したら true
     */
    bool SaveToFile(const std::string& filename);

    /**
     * @brief バイナリファイルからデータを読み込み、既存の固定長バッファに数値を直接上書き（再登録）する
     * @param filename 読み込み元ファイルパス
     * @return 成功したら true
     */
    bool LoadFromFile(const std::string& filename);
};