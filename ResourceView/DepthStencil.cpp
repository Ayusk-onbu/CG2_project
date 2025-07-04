#include "DepthStencil.h"
#include <cassert>

void DepthStencil::InitializeHeap(D3D12System& d3d12) {
	heap_.CreateDescriptorHeap(d3d12.GetDevice().Get(), D3D12_DESCRIPTOR_HEAP_TYPE_DSV, 1, false);
}

void DepthStencil::InitializeDesc(BOOL is, D3D12_DEPTH_WRITE_MASK mask, D3D12_COMPARISON_FUNC func) {
	SetEnable(is);
	SetMask(mask);
	SetFunc(func);
}

void DepthStencil::MakeResource(D3D12System& d3d12, int32_t width, int32_t height) {
	depthStencilResource_ = CreateDepthStencilTextureResource(d3d12.GetDevice().Get(), width, height);
	SetDesc();
}

//DepthStencilTexture(奥行きの根幹をなすもの大量に読み書きするらしい)
Microsoft::WRL::ComPtr < ID3D12Resource> DepthStencil::CreateDepthStencilTextureResource(ID3D12Device* device, int32_t width, int32_t height) {
	/////////////////////////////////////////////
#pragma region Resource/Heapの設定(生成するResourceと配置するHeapの場所の設定)
	D3D12_RESOURCE_DESC resourceDesc{};
	resourceDesc.Width = width;//Textures width
	resourceDesc.Height = height;//Textures height
	resourceDesc.MipLevels = 1;//mipmaps sum
	resourceDesc.DepthOrArraySize = 1;//奥行き or Textureの配列数
	resourceDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;//DepthStencilとして利用可能なフォーマット
	resourceDesc.SampleDesc.Count = 1;//サンプリングカウント固定
	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;//2次元
	resourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;//DepthStencilとして使う通知

	//利用するHeapの設定
	D3D12_HEAP_PROPERTIES heapProperties{};
	heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;//VRAM上につくる
#pragma endregion
	/////////////////////////////////////////////

	/////////////////////////////////////////////
#pragma region 深度地のクリア最適化設定
	D3D12_CLEAR_VALUE depthClearValue{};
	depthClearValue.DepthStencil.Depth = 1.0f;//1.0f(最大値)でクリアリング
	depthClearValue.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;//Format。Resourceと合わせる
#pragma endregion
	/////////////////////////////////////////////

	/////////////////////////////////////////////
#pragma region MakeResource
	Microsoft::WRL::ComPtr < ID3D12Resource> resource = nullptr;
	HRESULT hr = device->CreateCommittedResource(
		&heapProperties,//HEAPの設定
		D3D12_HEAP_FLAG_NONE,//HEAPの特殊な設定。特になし
		&resourceDesc,//Resourceの設定
		D3D12_RESOURCE_STATE_DEPTH_WRITE,//深度地を書き込む状態にしておく
		&depthClearValue,//Clear最適地
		IID_PPV_ARGS(&resource));//作成するResourceポインタへのポインタ
	assert(SUCCEEDED(hr));
#pragma endregion
	/////////////////////////////////////////////
	return resource;
}
