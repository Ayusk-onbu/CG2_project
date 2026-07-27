#include "Texture.h"
#include "Log.h"
#include "ResourceFunc.h"
#include "SRVManager.h"
#include "externals/DirectXTex/d3dx12.h"

Microsoft::WRL::ComPtr<ID3D12Resource> Texture::Initialize(D3D12System& d3d12, const std::string& filePath, int num, ID3D12GraphicsCommandList* commandList) {
	mipImages_ = LoadTexture(filePath);
	const DirectX::TexMetadata metaData_ = mipImages_.GetMetadata();
	textureResource_ = std::move(CreateTextureResource(d3d12.GetDevice().Get(), metaData_));
	textureResource_->SetName(L"TextureResource");

	auto srv = SRVManager::GetInstance();
	SRVAllocation alloc = srv->Allocate();

	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Format = metaData_.format;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	if (metaData_.IsCubemap()) {
		// キューブマップの場合は、SRVの設定をキューブマップ用に変更する
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBE;
		srvDesc.TextureCube.MipLevels = UINT_MAX;
		srvDesc.TextureCube.MostDetailedMip = 0;
		srvDesc.TextureCube.ResourceMinLODClamp = 0.0f;
	}
	else {
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MipLevels = UINT(metaData_.mipLevels);
	}
	d3d12.GetDevice().Get()->CreateShaderResourceView(
		textureResource_.Get(), 
		&srvDesc, 
		alloc.cpu
	);

	this->srvAllocation_ = alloc;

	textureSize_ = {
		static_cast<float>(metaData_.width),
		static_cast<float>(metaData_.height) 
	};

	return UploadTextureData(textureResource_, mipImages_, d3d12.GetDevice().Get(), commandList);
}

// 1,Textureデータを読み込む
DirectX::ScratchImage Texture::LoadTexture(const std::string& filePath) {
	DirectX::ScratchImage image{};
	std::wstring filePathW = ConvertString(filePath);

	HRESULT hr;
	if (filePathW.ends_with(L".dds")) {
		hr = DirectX::LoadFromDDSFile(filePathW.c_str(), DirectX::DDS_FLAGS_NONE, nullptr, image);
	}
	else {
		hr = DirectX::LoadFromWICFile(filePathW.c_str(), DirectX::WIC_FLAGS_FORCE_SRGB, nullptr, image);
	}
	assert(SUCCEEDED(hr));

	//ミニマップの作成
	DirectX::ScratchImage mipImages{};
	if (DirectX::IsCompressed(image.GetMetadata().format)) {
		mipImages = std::move(image);
	}
	else {
		hr = DirectX::GenerateMipMaps(image.GetImages(), image.GetImageCount(), image.GetMetadata(), DirectX::TEX_FILTER_SRGB, 0, mipImages);
		assert(SUCCEEDED(hr));
	}
	//みっぷマップ付きのデータを返す
	return mipImages;
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
	heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;// GPUのVRAMに配置
	heapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN; // 自動設定に任せる
	heapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;  // 自動設定に任せる
	// 3,Resourceを生成する
	Microsoft::WRL::ComPtr<ID3D12Resource> resource = nullptr;
	HRESULT hr = device->CreateCommittedResource(
		&heapProperties,
		D3D12_HEAP_FLAG_NONE,
		&resourceDesc,
		D3D12_RESOURCE_STATE_COPY_DEST,
		nullptr,
		IID_PPV_ARGS(resource.GetAddressOf()));
	assert(SUCCEEDED(hr));
	return resource;
}

[[nodiscard]]// <= 戻り値を破棄したくないときにつける
Microsoft::WRL::ComPtr <ID3D12Resource> Texture::UploadTextureData(Microsoft::WRL::ComPtr < ID3D12Resource>& texture, const DirectX::ScratchImage& mipImages, ID3D12Device* device, ID3D12GraphicsCommandList* commandList) {
	std::vector<D3D12_SUBRESOURCE_DATA> subresources;
	// DirectXTexの関数を使って、テクスチャデータをGPUに転送するためのSubresourceDataを作成する
	DirectX::PrepareUpload(device, mipImages.GetImages(), mipImages.GetImageCount(), mipImages.GetMetadata(), subresources);
	// 転送に必要な中間バッファのサイズを取得する
	uint64_t intermediateSize = GetRequiredIntermediateSize(texture.Get(), 0, UINT(subresources.size()));
	// 転送に必要な中間バッファを作成する
	Microsoft::WRL::ComPtr <ID3D12Resource> intermediateResource = CreateBufferResource(device, intermediateSize);
	// コマンドリストを使って、テクスチャデータをGPUに転送する
	UpdateSubresources(commandList, texture.Get(), intermediateResource.Get(), 0, 0, UINT(subresources.size()), subresources.data());
	// Textureへの転送後は利用できるよう、D3D12_RESOURCE_STATE_COPY_DESTからD3D12_RESOURCE_STATE_GENERIC_READへResourceStateを変更する
	D3D12_RESOURCE_BARRIER barrier{};
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier.Transition.pResource = texture.Get();
	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_GENERIC_READ;
	commandList->ResourceBarrier(1, &barrier);
	return intermediateResource;
}

float Texture::SampleNoiseCPU(float u, float v) const {
	// 画像データがなければ 0 を返す
	if (mipImages_.GetImageCount() == 0) return 0.0f;

	// ミップレベル0、配列インデックス0、深度0の画像を取得
	const DirectX::Image* image = mipImages_.GetImage(0, 0, 0);
	if (!image || !image->pixels) return 0.0f;

	// UV座標をテクスチャのピクセル座標（0 ～ Width-1, 0 ～ Height-1）に変換
	// リピート（タイリング）するようにラップ処理を入れる
	int x = static_cast<int>(std::floor(u * image->width)) % image->width;
	int y = static_cast<int>(std::floor(v * image->height)) % image->height;
	if (x < 0) x += (int)image->width;
	if (y < 0) y += (int)image->height;

	// ピクセル配列の先頭アドレス
	const uint8_t* pixels = image->pixels;

	// image->rowPitch は「横1行分のバイト数」
	// 1ピクセル4バイト（RGBA）のデータ位置を計算
	const uint8_t* targetPixel = pixels + (y * image->rowPitch) + (x * 4);

	// R成分（インデックス0）を取り出して 0.0f ～ 1.0f に正規化して返す
	// （白黒のノイズ画像ならRGBすべて同じ値なのでRだけでOK）
	return targetPixel[0] / 255.0f;
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