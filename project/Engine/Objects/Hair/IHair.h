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

class Hair {
public:
	void Initialize(Fngine* engine);
	void Update(float deltaTime, const Matrix4x4& mat);
	void Render(D3D12_GPU_DESCRIPTOR_HANDLE depthHandle);

	void PreHair(ID3D12Resource* randerTargetResource, D3D12_RESOURCE_STATES preState, ID3D12Resource* depthResource);
	void PostHair(ID3D12Resource* randerTargetResource, D3D12_RESOURCE_STATES postState, ID3D12Resource* depthResource);
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

	GuideCurve::Main hairData_;
	std::unique_ptr<Structured<Strands::ChildStrand>>gpuChildStrandBuffer_;// ストランド生成に必要な情報
	std::unique_ptr<ConstantBuffer<GuideCurve::HairPhysicsConfig>>gpuPhysicsConfigBuffer_;// 髪全体の情報その物理
	std::unique_ptr<ConstantBuffer<GuideCurve::FrameConfig>>gpuFrameConfigBuffer_;// 髪全体の情報その物理
	std::unique_ptr<ConstantBuffer<Strands::HairConfig>>gpuConfigBuffer_;// 髪全体の情報その性質
	std::unique_ptr<ConstantBuffer<Strands::HairMakeConfig>>gpuMakeConfigBuffer_;// 髪全体の情報その性質

	std::unique_ptr<Structured<GuideCurve::ControllerPoint>> uploadGuideBuffer;
	std::unique_ptr<RWStructured<GuideCurve::ControllerPoint>>gpuGuideBuffer_;// 全てのガイドの情報
	std::unique_ptr<RWStructured<Strands::StrandVertex>> hairVertexBuffer_;
	std::unique_ptr<RWStructured<D3D12_RAYTRACING_AABB>> hairAABBBuffer_;
	std::unique_ptr<Structured<D3D12_RAYTRACING_INSTANCE_DESC>> instanceBuffer;

	// 加速構造体（BLAS）用のリソース
	Microsoft::WRL::ComPtr<ID3D12Resource> scratchBuffer_;
	Microsoft::WRL::ComPtr<ID3D12Resource> blasResultBuffer_;
	// 加速構造体（TLAS）用のリソース
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
};