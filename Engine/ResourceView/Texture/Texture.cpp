#include "Texture.h"
#include "Log.h"


void Texture::Initialize(D3D12System& d3d12, SRV& srv, const std::string& filePath,int num) {
	mipImages_ = LoadTexture(filePath);
	const DirectX::TexMetadata metaData_ = mipImages_.GetMetadata();
	textureResource_ = std::move(CreateTextureResource(d3d12.GetDevice().Get(), metaData_));
	textureResource_->SetName(L"TextureResource");
	UploadTextureData(textureResource_, mipImages_);

	SetDesc(metaData_.format, D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING, D3D12_SRV_DIMENSION_TEXTURE2D, UINT(metaData_.mipLevels));
	textureSrvHandleCPU_ = srv.GetCPUDescriptorHandle();
	textureSrvHandleGPU_ = srv.GetGPUDescriptorHandle();
	d3d12.GetDevice().Get()->CreateShaderResourceView(textureResource_.Get(), &srvDesc_, textureSrvHandleCPU_);
}

void Texture::SetDesc(DXGI_FORMAT fmt, UINT mapping, D3D12_SRV_DIMENSION dimension, UINT mipLevel) {
	srvDesc_.Format = fmt;
	srvDesc_.Shader4ComponentMapping = mapping;
	srvDesc_.ViewDimension = dimension;
	srvDesc_.Texture2D.MipLevels = mipLevel;
}

// 1,Textureデータを読み込む
DirectX::ScratchImage Texture::LoadTexture(const std::string& filePath) {
	DirectX::ScratchImage image{};
	std::wstring filePathW = ConvertString(filePath);
	HRESULT hr = DirectX::LoadFromWICFile(filePathW.c_str(), DirectX::WIC_FLAGS_FORCE_SRGB, nullptr, image);
	assert(SUCCEEDED(hr));
	//ミニマップの作成
	DirectX::ScratchImage mipImges{};
	hr = DirectX::GenerateMipMaps(image.GetImages(), image.GetImageCount(), image.GetMetadata(), DirectX::TEX_FILTER_SRGB, 0, mipImges);
	assert(SUCCEEDED(hr));
	//みっぷマップ付きのデータを返す
	return mipImges;
}

// 2,DirectX12のテクスチャリソースを作成する
Microsoft::WRL::ComPtr<ID3D12Resource> Texture::CreateTextureResource(ID3D12Device* device, const DirectX::TexMetadata& metadata) {
	// 1,metadataをもとにResourceの設定
	D3D12_RESOURCE_DESC resourceDesc = {};
	resourceDesc.Width = UINT(metadata.width);//テクスチャの幅
	resourceDesc.Height = UINT(metadata.height);//テクスチャの高さ
	resourceDesc.MipLevels = UINT16(metadata.mipLevels);//ミップマップの数
	resourceDesc.DepthOrArraySize = UINT16(metadata.arraySize);//テクスチャの深さ
	resourceDesc.Format = metadata.format;//テクスチャのフォーマット
	resourceDesc.SampleDesc.Count = 1;//サンプル数
	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION(metadata.dimension);//テクスチャの次元
	// 2,利用するHeapの設定
	D3D12_HEAP_PROPERTIES heapProperties = {};
	heapProperties.Type = D3D12_HEAP_TYPE_CUSTOM;//細かい設定
	heapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_WRITE_BACK;//WriteBackポリシーでアクセス可能
	heapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_L0;//プロセッサの近くに配置
	// 3,Resourceを生成する
	Microsoft::WRL::ComPtr<ID3D12Resource> resource = nullptr;
	HRESULT hr = device->CreateCommittedResource(
		&heapProperties,
		D3D12_HEAP_FLAG_NONE,
		&resourceDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(resource.GetAddressOf()));
	assert(SUCCEEDED(hr));
	return resource;
}

void Texture::UploadTextureData(Microsoft::WRL::ComPtr < ID3D12Resource>& texture, const DirectX::ScratchImage& mipImages) {
	//Meta情報を取得
	const DirectX::TexMetadata& metadata = mipImages.GetMetadata();
	//全MipMapに対して
	for (size_t mipLevel = 0;mipLevel < metadata.mipLevels;++mipLevel) {
		//MipMapLevelを指定して書くImageを取得
		const DirectX::Image* img = mipImages.GetImage(mipLevel, 0, 0);
		//Textureに転送
		HRESULT hr = texture->WriteToSubresource(
			UINT(mipLevel),
			nullptr,//全領域へコピー
			img->pixels,//元データアドレス
			UINT(img->rowPitch),//1ラインサイズ
			UINT(img->slicePitch)//1枚サイズ
		);
		assert(SUCCEEDED(hr));
	}
}

//// DescriptorHandleを取得する関数(CPU)
//D3D12_CPU_DESCRIPTOR_HANDLE Texture::GetCPUDescriptorHandle(ID3D12DescriptorHeap* descriptorHeap, uint32_t descriptorSize, uint32_t index) {
//	D3D12_CPU_DESCRIPTOR_HANDLE handleCPU = descriptorHeap->GetCPUDescriptorHandleForHeapStart();
//	handleCPU.ptr += descriptorSize * index;
//	return handleCPU;
//}
//// DescriptorHandleを取得する関数(GPU)
//D3D12_GPU_DESCRIPTOR_HANDLE Texture::GetGPUDescriptorHandle(ID3D12DescriptorHeap* descriptorHeap, uint32_t descriptorSize, uint32_t index) {
//	D3D12_GPU_DESCRIPTOR_HANDLE handleGPU = descriptorHeap->GetGPUDescriptorHandleForHeapStart();
//	handleGPU.ptr += descriptorSize * index;
//	return handleGPU;
//}