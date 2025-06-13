#include <windows.h>
#include <cstdint>
#include <d3d12.h>
#include <dxgi1_6.h>
#include <cassert>
#include <string>
#include <format>
#include <filesystem>
#include <fstream>//ファイルに書いたり読んだりするライブラリ
#include <sstream>
#include <chrono>//時間を扱うライブラリ
//#include <dbghelp.h>//CrashHandler06
//#include <strsafe.h>//Dumpを出力06
#include <dxgidebug.h>//0103 ReportLiveobject
#include <dxcapi.h>//DXCの初期化
#include <cmath>
#include <wrl.h>


#include "Window.h"
#include "ErrorGuardian.h"
#include "Log.h"
#include "DXGI.h"
#include "D3D12System.h"

#include "CommandQueue.h"
#include "CommandList.h"

#include "TheOrderCommand.h"

#include "SwapChain.h"
#include "TachyonSync.h"
#include "OmnisTechOracle.h"

#include "ImGuiManager.h"
#include "Structures.h"
#include "ResourceBarrier.h"
#include "InputManager.h"
#include "Mouse.h"
#include "Music.h"
#include "DebugCamera.h"

#include "externals/DirectXTex/DirectXTex.h"

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib,"dxgi.lib")
//#pragma comment(lib,"Dbghelp.lib")//ClashHandler06
#pragma comment(lib,"dxguid.lib")//0103 ReportLiveobject
#pragma comment(lib,"dxcompiler.lib")//DXCの初期化


#define pi float(3.14159265358979323846f)

#pragma region 構造体
struct Transform {
	Vector3 scale;
	Vector3 rotate;
	Vector3 translate;
};
struct Ortho {
	float left, right, top, bottom;
};

struct VertexData {
	Vector4 position;
	Vector2 texcoord;//u座標とv座標のことかな
	Vector3 normal;//法線ベクトル
};

struct Material {
	Vector4 color;
	int32_t enableLighting;
	float padding[3];
	Matrix4x4 uvTransform;
};

struct TransformationMatrix {
	Matrix4x4 WVP;
	Matrix4x4 World;
};

struct MaterialData {
	std::string textureFilePath;
};

struct ModelData {
	std::vector<VertexData> vertices;
	MaterialData material;
};

//平行光源
struct DirectionalLight {
	Vector4 color;// ライトの色
	Vector3 direction;// ライトの向き
	float intensity;// 輝度
};
#pragma endregion

class ResourceObject {
public:
	ResourceObject(ID3D12Resource*resource): resource_(resource){};
	// デストラクタはオブジェクトの寿命が尽きたときに呼ばれる
	~ResourceObject() {
		// ここでReleaseを呼ぶ
		if (resource_) {
			resource_->Release();
		}
	}
	ID3D12Resource* Get() { return resource_; }
private:
	ID3D12Resource* resource_;
};

struct D3D12ResourceLeakChecker {
	~D3D12ResourceLeakChecker() {
		// リソースリーク✅
		Microsoft::WRL::ComPtr<IDXGIDebug1> debug;
		//リソースリークチェック==================-↓↓↓
		if (SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(&debug)))) {
			debug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_ALL);
			debug->ReportLiveObjects(DXGI_DEBUG_APP, DXGI_DEBUG_RLO_ALL);
			debug->ReportLiveObjects(DXGI_DEBUG_D3D12, DXGI_DEBUG_RLO_ALL);
			//debug->Release();
		}
		//リソースリークチェック==================-↑↑↑
	}
};

//CompileShader関数
IDxcBlob* CompileShader(
	//CompilerするShaderファイルへのパス
	const std::wstring& filePath,
	//Compilerに使用するProfile
	const wchar_t* profile,
	//初期化で生成したものを三つ
	IDxcUtils* dxcUtils,
	IDxcCompiler3* dxcCompiler,
	IDxcIncludeHandler* includeHandler) 
{
	// ここの中身をこの後書いていく
	// １，hlslファイルを読む
	//これからシェーダーをコンパイルする旨をログに出す
	Log::View(ConvertString(std::format(L"Begin CompileShader,path:{}, profile:{}\n", filePath, profile)));
	//hlslファイルを読み込む
	IDxcBlobEncoding* shaderSource = nullptr;
	HRESULT hr = dxcUtils->LoadFile(filePath.c_str(), nullptr, &shaderSource);
	//読めなかったら止める
	assert(SUCCEEDED(hr));
	//読み込んだファイルの内容を設定する
	DxcBuffer shaderSourceBuffer;
	shaderSourceBuffer.Ptr = shaderSource->GetBufferPointer();
	shaderSourceBuffer.Size = shaderSource->GetBufferSize();
	shaderSourceBuffer.Encoding = DXC_CP_UTF8;//UTF8の文字コードであることを通知
	// ２，Compileする
	LPCWSTR arguments[] = {
		filePath.c_str(),
		L"-E",L"main",
		L"-T",profile,
		L"-Zi",L"-Qembed_debug",
		L"-Od",
		L"-Zpr",
	};
	//実際にShaderをコンパイルする
	IDxcResult* shaderResult = nullptr;
	hr = dxcCompiler->Compile(
		&shaderSourceBuffer,
		arguments,
		_countof(arguments),
		includeHandler,
		IID_PPV_ARGS(&shaderResult)
	);
	//コンパイルエラーではなくdxcが起動できないなど致命的な状況
	assert(SUCCEEDED(hr));

	// ３，警告・エラーが惰ていないか確認する
	//警告・エラーが出ていたらログに出して止める
	IDxcBlobUtf8* shaderError = nullptr;
	shaderResult->GetOutput(DXC_OUT_ERRORS, IID_PPV_ARGS(&shaderError), nullptr);
	if (shaderError != nullptr && shaderError->GetStringLength() != 0) {
		Log::View(shaderError->GetStringPointer());
		//警告エラーダメ絶対
		assert(false);
	}
	// ４，Compile結果を受け取って返す
	//コンパイル結果から実行用のバイナリ部分を取得
	IDxcBlob* shaderBlob = nullptr;
	hr = shaderResult->GetOutput(DXC_OUT_OBJECT, IID_PPV_ARGS(&shaderBlob), nullptr);
	assert(SUCCEEDED(hr));
	//成功したログを出す
	Log::View(ConvertString(std::format(L"CompileSucceeded, path:{},profile:{}\n", filePath, profile)));
	//もう使わないリソースを開放
	shaderSource->Release();
	shaderResult->Release();
	//実行用のバイナリを返却
	return shaderBlob;
}

ID3D12Resource* CreateBufferResource(ID3D12Device* device, size_t sizeInBytes) {
		ID3D12Resource* buffer = nullptr;
		// ヒーププロパティの設定
		//頂点リソース用のヒープの生成
		D3D12_HEAP_PROPERTIES heapProps = {};
		heapProps.Type = D3D12_HEAP_TYPE_UPLOAD;// use UploadHeap
		heapProps.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
		heapProps.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
		// リソースの記述
		//頂点リソースの設定
		D3D12_RESOURCE_DESC resourceDesc = {};
		//バッファリソース。テクさちゃの場合はまた別の設定
		resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		resourceDesc.Width = sizeInBytes;//リソースのサイズ
		//バッファの場合は 1にしないといけない
		resourceDesc.Height = 1;
		resourceDesc.DepthOrArraySize = 1;
		resourceDesc.MipLevels = 1;
		resourceDesc.Format = DXGI_FORMAT_UNKNOWN;
		resourceDesc.SampleDesc.Count = 1;
		//バッファはこれ
		resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
		// リソースの作成
		HRESULT hr = device->CreateCommittedResource(
			&heapProps,
			D3D12_HEAP_FLAG_NONE,
			&resourceDesc,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&buffer)
		);
		assert(SUCCEEDED(hr));
		return buffer;
}

ID3D12DescriptorHeap* CreateDescriptorHeap(ID3D12Device* device, D3D12_DESCRIPTOR_HEAP_TYPE type, UINT numDescriptors, bool shaderVisible) {
	//ディスクリプタヒープの設定
	D3D12_DESCRIPTOR_HEAP_DESC descriptorHeapDesc{};
	descriptorHeapDesc.Type = type;	//ディスクリプタの種類
	descriptorHeapDesc.NumDescriptors = numDescriptors;	//ディスクリプタの数
	descriptorHeapDesc.Flags = shaderVisible ? D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE : D3D12_DESCRIPTOR_HEAP_FLAG_NONE;	//シェーダーからアクセスできるようにする
	ID3D12DescriptorHeap* descriptorHeap = nullptr;
	HRESULT hr = device->CreateDescriptorHeap(&descriptorHeapDesc, IID_PPV_ARGS(&descriptorHeap));
	assert(SUCCEEDED(hr));
	return descriptorHeap;
}
MaterialData LoadMaterialTemplateFile(const std::string& directoryPath, const std::string& filename) {
	// 1, 中で必要となる変数の宣言
	MaterialData materialData;// 構築するMaterialData
	std::string line;
	std::ifstream file(directoryPath + "/" + filename);
	assert(file.is_open());
	// 2, ファイルを開く
	while (std::getline(file, line)) {
		std::string identifier;
		std::istringstream s(line);
		s >> identifier;// 行の最初の文字列を取得
		if (identifier == "map_Kd") {
			std::string textureFilename;
			s >> textureFilename;// テクスチャファイル名を取得
			// テクスチャファイルのパスを構築
			materialData.textureFilePath = directoryPath + "/" + textureFilename;
		}
	}


	// 3, 実際にファイルを読み、MaterialDataを構築していく
	// 4, MaterialDataを返す
	return materialData;
}
/// <summary>
/// ObjファイルからModelDataを構築する
/// </summary>
/// <param name="directoryPath ファイルがある場所を指定"></param>
/// <param name="filename ファイルの名前"></param>
/// <returns></returns>
ModelData LoadObjFile(const std::string& directoryPath, const std::string& filename) {
	// 1, 中で必要となる変数の宣言
	ModelData modelData;// 構築するModelData
	std::vector<Vector4> positions;// v
	std::vector<Vector3> normals;// vn
	std::vector<Vector2>texcoords;// vt
	std::string line;// mtlの設定
	// 2, ファイルを開く
	std::ifstream file(directoryPath + "/" + filename);
	assert(file.is_open());
	// 3, 実装にファイルを読み、ModelDataを構築していく
	while (std::getline(file, line)) {
		std::string identifier;
		std::istringstream s(line);
		s >> identifier;// 行の最初の文字列を取得
		if (identifier == "v") {
			Vector4 position;
			s >> position.x >> position.y >> position.z;
			position.x *= -1.0f; // X軸を反転
			//position.y *= -1.0f; // X軸を反転
			position.w = 1.0f;
			positions.push_back(position);
		}
		else if (identifier == "vt") {
			Vector2 texcoord;
			s >> texcoord.x >> texcoord.y;
			texcoord.y = 1.0f - texcoord.y; // Y軸を反転
			texcoords.push_back(texcoord);
		}
		else if (identifier == "vn") {
			Vector3 normal;
			s >> normal.x >> normal.y >> normal.z;
			normal.x *= -1.0f; // X軸を反転
			//normal.y *= -1.0f; // X軸を反転
			normals.push_back(normal);
		}
		else if (identifier == "f") {
			VertexData triangle[3];
			// 面は三角形限定
			for (int32_t faceVertex = 0; faceVertex < 3;++faceVertex) {
				std::string vertexDefinition;
				s >> vertexDefinition;
				// 頂点の要素へのIndexは位置UV法線なので分解する
				std::istringstream v(vertexDefinition);
				uint32_t elementIndices[3];
				for (int32_t element = 0;element < 3;++element) {
					std::string index;
					std::getline(v, index, '/');// '/'で分割
					elementIndices[element] = std::stoi(index);//文字列を数値に変換
				}
				// 要素へのIndexから、実際の要素の値を取得して、頂点を構築する
				Vector4 position = positions[elementIndices[0] - 1];
				Vector2 texcoord = texcoords[elementIndices[1] - 1];
				Vector3 normal = normals[elementIndices[2] - 1];
				// 頂点データを構築
				//VertexData vertex = { position,texcoord,normal };
				//modelData.vertices.push_back(vertex);
				triangle[faceVertex] = { position,texcoord,normal };
			}
			// 頂点を逆順に登録
			modelData.vertices.push_back(triangle[2]);
			modelData.vertices.push_back(triangle[1]);
			modelData.vertices.push_back(triangle[0]);
		}
		else if (identifier == "mtllib") {
			// materialTemplateLibraryファイルの名前を取得する
			std::string materialFilename;
			s >> materialFilename;
			// 基本的にObjファイルと同一階層にMtlは存在
			modelData.material = LoadMaterialTemplateFile(directoryPath, materialFilename);
		}
	}
	// 4, ModelDataを返す
	return modelData;
}


// 1,Textureデータを読み込む
DirectX::ScratchImage LoadTexture(const std::string& filePath) {
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
ID3D12Resource* CreateTextureResource(ID3D12Device* device, const DirectX::TexMetadata& metadata) {
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
	ID3D12Resource* resource = nullptr;
	HRESULT hr = device->CreateCommittedResource(
		&heapProperties,
		D3D12_HEAP_FLAG_NONE,
		&resourceDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&resource));
	assert(SUCCEEDED(hr));
	return resource;
}

void UploadTextureData(ID3D12Resource* texture, const DirectX::ScratchImage& mipImages) {
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

//DepthStencilTexture(奥行きの根幹をなすもの大量に読み書きするらしい)
ID3D12Resource* CreateDepthStencilTextureResource(ID3D12Device* device, int32_t width, int32_t height) {
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
	ID3D12Resource* resource = nullptr;
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

// DescriptorHandleを取得する関数(CPU)
D3D12_CPU_DESCRIPTOR_HANDLE GetCPUDescriptorHandle(ID3D12DescriptorHeap* descriptorHeap,uint32_t descriptorSize, uint32_t index) {
	D3D12_CPU_DESCRIPTOR_HANDLE handleCPU = descriptorHeap->GetCPUDescriptorHandleForHeapStart();
	handleCPU.ptr += descriptorSize * index;
	return handleCPU;
}
// DescriptorHandleを取得する関数(GPU)
D3D12_GPU_DESCRIPTOR_HANDLE GetGPUDescriptorHandle(ID3D12DescriptorHeap* descriptorHeap, uint32_t descriptorSize, uint32_t index) {
	D3D12_GPU_DESCRIPTOR_HANDLE handleGPU = descriptorHeap->GetGPUDescriptorHandleForHeapStart();
	handleGPU.ptr += descriptorSize * index;
	return handleGPU;
}

// windowsアプリでのエントリーポイント（main関数）
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
	D3D12ResourceLeakChecker leakCheck;
	int32_t kClienWidth = 1280;
	int32_t kClienHeight = 720;

	InputManager input;

	// モデルデータを読み込み
	ModelData modelData = LoadObjFile("resources", "axis.obj");
	
	//分割数
	static const float kSubdivision = 16.0f;
	//頂点数 (分割数(縦÷緯度)x分割数(横÷経度)x６)
	uint32_t vertexCount = static_cast<uint32_t>(kSubdivision * kSubdivision) * 6;
////////////////////////////////////////////////////////////
#pragma region DirectX12の初期化独立している

	//COMの初期化
	CoInitializeEx(0, COINIT_MULTITHREADED);
	//誰も補足しなかった場合に（Unhandled）、補足する関数を登録
	//main関数はじまってすぐに登録するといい
	SetUnhandledExceptionFilter(ErrorGuardian::ExportDump);

	Log::Initialize();

#pragma endregion
////////////////////////////////////////////////////////////

#pragma region ウィンドウ
#pragma endregion
	Window window;
	//ウィンドウの生成
	window.Initialize(L"CG2CLASSNAME", L"CG2");
#pragma region デバッグレイヤー(開発時のデバッグ支援)
#pragma endregion
	ErrorGuardian::SetDebugInterface();
	
	////////////////////////////////////////////////////////////
#pragma region DXGIファクトリーの作成
#pragma endregion
	DXGI::RecruitEngineer();
	HRESULT hr;
	////////////////////////////////////////////////////////////

	////////////////////////////////////////////////////////////
#pragma region 使用するアダプタの決定
#pragma endregion
	OmnisTechOracle::Oracle();

	////////////////////////////////////////////////////////////

	////////////////////////////////////////////////////////////
#pragma region D3D12Deviceの生成
	//Microsoft::WRL::ComPtr <ID3D12Device> device = nullptr;
	////機能レベルとログ出力用の文字列
	//D3D_FEATURE_LEVEL featureLevels[] = {
	//	D3D_FEATURE_LEVEL_12_2,D3D_FEATURE_LEVEL_12_1,D3D_FEATURE_LEVEL_12_0
	//};
	//const char* featureLevelStrings[] = { "12.2","12.1","12.0" };
	////高い順に生成できるか試していく
	//for (size_t i = 0;i < _countof(featureLevels);++i) {
	//	//採用したアダプターでデバイスを生成
	//	hr = D3D12CreateDevice(OmnisTechOracle::useAdapter_.Get(), featureLevels[i], IID_PPV_ARGS(&device));
	//	//指定した機能レベルでデバイスが生成出来たかを確認
	//	if (SUCCEEDED(hr)) {
	//		//生成出来たのでログ出力を行ってループを抜ける
	//		Log::View((std::format("FeatureLevel : {}\n", featureLevelStrings[i])));
	//		break;
	//	}
	//}
	////デバイスの生成が上手くいかったので起動できない
	//assert(device != nullptr);
	//Log::ViewFile("Complete create D3D12Device!!!\n");//初期化完了のログを出す
#pragma endregion
	D3D12System d3d12;
	d3d12.SelectLevel();

	////////////////////////////////////////////////////////////

#pragma region エラー警告 直ちに停止独立している
#pragma endregion
	ErrorGuardian::SetQueue(d3d12.GetDevice().Get());

	////////////////////////////////////////////////////////////
#pragma region MaterialResourceこれが描画に必要
	Microsoft::WRL::ComPtr <ID3D12Resource> materialResource = CreateBufferResource(d3d12.GetDevice().Get(), sizeof(Material));//
#pragma region Sprite
	Microsoft::WRL::ComPtr <ID3D12Resource> materialResourceSprite = CreateBufferResource(d3d12.GetDevice().Get(), sizeof(Material));
#pragma endregion
#pragma endregion
	////////////////////////////////////////////////////////////

	////////////////////////////////////////////////////////////
#pragma region CommandQueueの生成
#pragma endregion
#pragma region CommandList
#pragma endregion
	TheOrderCommand command;
	command.GetQueue().SetDescD();
	hr = d3d12.GetDevice()->CreateCommandQueue(&command.GetQueue().GetDesc(), IID_PPV_ARGS(&command.GetQueue().GetQueue()));
	//コマンドキューの生成が上手くいかなかったので起動できない
	assert(SUCCEEDED(hr));
	hr = d3d12.GetDevice()->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT,
	IID_PPV_ARGS(&command.GetList().GetAllocator()));
	//コマンドアロケータの生成が上手くいかなかったので起動できない
	assert(SUCCEEDED(hr));
	hr = d3d12.GetDevice()->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, command.GetList().GetAllocator().Get(), nullptr,
	IID_PPV_ARGS(&command.GetList().GetList()));
	//コマンドリストの生成がうまくいかなかったので起動できない
	assert(SUCCEEDED(hr));
#pragma region SwapChain
#pragma endregion
	SwapChain::Initialize(window);

	////////////////////////////////////////////////////////////
	//コマンドキュー、ウィンドウハンドル、設定を渡して生成する
	DXGI::AssignTaskToEngineer(command.GetQueue().GetQueue().Get(), window);

	////////////////////////////////////////////////////////////
#pragma region ディスクリプタヒープの生成
	Microsoft::WRL::ComPtr <ID3D12DescriptorHeap> rtvDescriptorHeap = CreateDescriptorHeap(d3d12.GetDevice().Get(), D3D12_DESCRIPTOR_HEAP_TYPE_RTV, 2, false);
	Microsoft::WRL::ComPtr <ID3D12DescriptorHeap> srvDescriptorHeap = CreateDescriptorHeap(d3d12.GetDevice().Get(), D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 128, true);
	Microsoft::WRL::ComPtr <ID3D12DescriptorHeap> dsvDescriptorHeap = CreateDescriptorHeap(d3d12.GetDevice().Get(), D3D12_DESCRIPTOR_HEAP_TYPE_DSV, 1, false);
#pragma endregion
	////////////////////////////////////////////////////////////

	////////////////////////////////////////////////////////////
#pragma region DescriptorSize
	const uint32_t descriptorSizeSRV = d3d12.GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	const uint32_t descriptorSizeRTV = d3d12.GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	const uint32_t descriptorSizeDSV = d3d12.GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
#pragma endregion
	////////////////////////////////////////////////////////////

	////////////////////////////////////////////////////////////
#pragma region SwapChainからResourceを引っ張ってくる
	Microsoft::WRL::ComPtr <ID3D12Resource> swapChainResources[2] = { nullptr };
	hr = SwapChain::swapChain_->GetBuffer(0, IID_PPV_ARGS(&swapChainResources[0]));
	//上手く取得出来なければ起動できない
	assert(SUCCEEDED(hr));
	hr = SwapChain::swapChain_->GetBuffer(1, IID_PPV_ARGS(&swapChainResources[1]));
	assert(SUCCEEDED(hr));
#pragma endregion
	////////////////////////////////////////////////////////////

	////////////////////////////////////////////////////////////
#pragma region VertexResource 描画に必要
	//ID3D12Resource* vertexResource = CreateBufferResource(d3d12.GetDevice(), sizeof(VertexData) * vertexCount);//
	// 頂点リソースを作る
	Microsoft::WRL::ComPtr <ID3D12Resource> vertexResource = CreateBufferResource(d3d12.GetDevice().Get(), sizeof(VertexData) * modelData.vertices.size());
#pragma region Sprite
	Microsoft::WRL::ComPtr <ID3D12Resource> vertexResourceSprite = CreateBufferResource(d3d12.GetDevice().Get(), sizeof(VertexData) * 4);
	Microsoft::WRL::ComPtr <ID3D12Resource> indexResourceSprite = CreateBufferResource(d3d12.GetDevice().Get(), sizeof(uint32_t) * 6);
#pragma endregion
#pragma endregion
	////////////////////////////////////////////////////////////

	////////////////////////////////////////////////////////////
#pragma region DirectionLightResource一個でいいが複数作れるようにしておくのもあり
	Microsoft::WRL::ComPtr <ID3D12Resource> directionLightResource = CreateBufferResource(d3d12.GetDevice().Get(), sizeof(DirectionalLight) * vertexCount);
#pragma endregion
	////////////////////////////////////////////////////////////

	////////////////////////////////////////////////////////////
#pragma region DepthStencilTexture
	Microsoft::WRL::ComPtr <ID3D12Resource> depthStencilResource = CreateDepthStencilTextureResource(d3d12.GetDevice().Get(), kClienWidth, kClienHeight);
#pragma endregion
	////////////////////////////////////////////////////////////

	////////////////////////////////////////////////////////////
#pragma region TransformationMatrix
	Microsoft::WRL::ComPtr <ID3D12Resource> wvpResource = CreateBufferResource(d3d12.GetDevice().Get(), sizeof(TransformationMatrix));
	Microsoft::WRL::ComPtr <ID3D12Resource> transformationMatrixResourceSprite = CreateBufferResource(d3d12.GetDevice().Get(), sizeof(TransformationMatrix));
#pragma endregion
	////////////////////////////////////////////////////////////

	////////////////////////////////////////////////////////////
#pragma region RTVを作る
	D3D12_RENDER_TARGET_VIEW_DESC rtvDesc = {};
	rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;//出力結果をSRGBに変換して書き込む
	rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;//２Dテクスチャッとして書き込む
	//ディスクリプタの先頭を取得する
	D3D12_CPU_DESCRIPTOR_HANDLE rtvStartHandle = rtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	//RTVを２つ作るのでディスクリプタを二つ用意
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandles[2];
	//まず1つ目を作る。一つ目は最初のところに作る。作る場所をこちらで指定してあげる必要がある
	rtvHandles[0] = rtvStartHandle;
	d3d12.GetDevice()->CreateRenderTargetView(swapChainResources[0].Get(), &rtvDesc, rtvHandles[0]);
	//2二つ目のディスクリプタ班どりを作る（自力で）
	rtvHandles[1].ptr = rtvHandles[0].ptr + d3d12.GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	//2つ目を作る
	d3d12.GetDevice()->CreateRenderTargetView(swapChainResources[1].Get(), &rtvDesc, rtvHandles[1]);
#pragma endregion
	////////////////////////////////////////////////////////////

	////////////////////////////////////////////////////////////
#pragma region FenceとEvent
#pragma endregion
	TachyonSync::GetCGPU().Initialize(d3d12.GetDevice().Get());

	////////////////////////////////////////////////////////////

	////////////////////////////////////////////////////////////
#pragma region DXCの初期化
	//dxCompilerを初期化
	IDxcUtils* dxcUtils = nullptr;
	IDxcCompiler3* dxcCompiler = nullptr;
	hr = DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(&dxcUtils));
	assert(SUCCEEDED(hr));
	hr = DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&dxcCompiler));
	assert(SUCCEEDED(hr));

	//現時点でincludeはしないが、includeに対応するための設定を行っていく
	IDxcIncludeHandler* includeHandler = nullptr;
	hr = dxcUtils->CreateDefaultIncludeHandler(&includeHandler);
	assert(SUCCEEDED(hr));
#pragma endregion
	////////////////////////////////////////////////////////////

	////////////////////////////////////////////////////////////
#pragma region Pipeline State Object
	// 1, RootSignatureの生成
	D3D12_ROOT_SIGNATURE_DESC descriptionRootSignature = {};
	descriptionRootSignature.Flags =
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	//RootParameterの作成。複数設定できるので配列(CBufferの数に応じて増やす)
	D3D12_ROOT_PARAMETER rootParameters[4] = {};

	D3D12_DESCRIPTOR_RANGE descriptorRange[1] = {};
	descriptorRange[0].BaseShaderRegister = 0;
	descriptorRange[0].NumDescriptors = 1;
	descriptorRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	descriptorRange[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;//Use CBV
	rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;//Use PixelShader
	rootParameters[0].Descriptor.ShaderRegister = 0;//レジスタ番号０(hlslのやつ)とバインド
	rootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;//Use CBV
	rootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;//Use VertexShader
	rootParameters[1].Descriptor.ShaderRegister = 0;//レジスタ番号０(hlslのやつ)
	rootParameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;//Use DescriptorTable
	rootParameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;// PixelShader Use
	rootParameters[2].DescriptorTable.pDescriptorRanges = descriptorRange;//Tableの中身の配列を指定
	rootParameters[2].DescriptorTable.NumDescriptorRanges = _countof(descriptorRange);//Tableで利用する数
	rootParameters[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;//Use CBV
	rootParameters[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;//Use PixelShader
	rootParameters[3].Descriptor.ShaderRegister = 1;//レジスタ番号１(hlslのやつ)とバインド

	descriptionRootSignature.pParameters = rootParameters;//ルートパラメータ配列へのポインタ
	descriptionRootSignature.NumParameters = _countof(rootParameters);//配列の長さ

	//Samplerのせってい
	D3D12_STATIC_SAMPLER_DESC staticSamplers[1] = {};
	staticSamplers[0].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
	staticSamplers[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[0].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
	staticSamplers[0].MaxLOD = D3D12_FLOAT32_MAX;
	staticSamplers[0].ShaderRegister = 0;
	staticSamplers[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	descriptionRootSignature.pStaticSamplers = staticSamplers;
	descriptionRootSignature.NumStaticSamplers = _countof(staticSamplers);

	//シリアライズしてバイナりにする
	ID3DBlob* signatureBlob = nullptr;
	ID3DBlob* errorBlob = nullptr;
	hr = D3D12SerializeRootSignature(&descriptionRootSignature,
		D3D_ROOT_SIGNATURE_VERSION_1, &signatureBlob, &errorBlob);
	if (FAILED(hr)) {
		Log::View(reinterpret_cast<char*>(errorBlob->GetBufferPointer()));
		assert(false);
	}
	//バイナリをもとに生成
	Microsoft::WRL::ComPtr <ID3D12RootSignature> rootSignature = nullptr;
	hr = d3d12.GetDevice()->CreateRootSignature(0,
		signatureBlob->GetBufferPointer(), signatureBlob->GetBufferSize(),
		IID_PPV_ARGS(&rootSignature));
	assert(SUCCEEDED(hr));
	// 2, InputLayoutの設定
	D3D12_INPUT_ELEMENT_DESC inputElementDescs[3] = {};
	inputElementDescs[0].SemanticName = "POSITION";
	inputElementDescs[0].SemanticIndex = 0;
	inputElementDescs[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	inputElementDescs[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	inputElementDescs[1].SemanticName = "TEXCOORD";
	inputElementDescs[1].SemanticIndex = 0;
	inputElementDescs[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	inputElementDescs[1].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	inputElementDescs[2].SemanticName = "Normal";
	inputElementDescs[2].SemanticIndex = 0;
	inputElementDescs[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	inputElementDescs[2].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	D3D12_INPUT_LAYOUT_DESC inputLayoutDesc = {};
	inputLayoutDesc.pInputElementDescs = inputElementDescs;
	inputLayoutDesc.NumElements = _countof(inputElementDescs);
	// 3, BlendStateの設定
	D3D12_BLEND_DESC blendDesc = {};
	//すべての色要素を書き込む
	blendDesc.RenderTarget[0].RenderTargetWriteMask =
		D3D12_COLOR_WRITE_ENABLE_ALL;
	// 4, RasterizerStateの設定(面をピクセルに分解してPixelShaderを起動)
	D3D12_RASTERIZER_DESC rasterizerDesc = {};
	//裏面（時計回り）を表示しない
	rasterizerDesc.CullMode = D3D12_CULL_MODE_BACK;
	//三角形の中を塗りつぶす
	rasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;
	// 5, ShaderをCompileする
	IDxcBlob* vertexShaderBlob = CompileShader(L"Object3D.VS.hlsl",
		L"vs_6_0", dxcUtils, dxcCompiler, includeHandler);
	assert(vertexShaderBlob != nullptr);
	IDxcBlob* pixelShaderBlob = CompileShader(L"Object3D.PS.hlsl",
		L"ps_6_0", dxcUtils, dxcCompiler, includeHandler);
	assert(pixelShaderBlob != nullptr);
#pragma endregion
	////////////////////////////////////////////////////////////

	////////////////////////////////////////////////////////////
#pragma region PSOを生成
	D3D12_GRAPHICS_PIPELINE_STATE_DESC graphicsPipelineStateDesc = {};
	graphicsPipelineStateDesc.pRootSignature = rootSignature.Get();//RootSignature
	graphicsPipelineStateDesc.InputLayout = inputLayoutDesc;//InputLayout
	graphicsPipelineStateDesc.BlendState = blendDesc;//BlendState
	graphicsPipelineStateDesc.RasterizerState = rasterizerDesc;//RasterizerState
	graphicsPipelineStateDesc.VS = { vertexShaderBlob->GetBufferPointer(),
	vertexShaderBlob->GetBufferSize() };// VertexShader
	graphicsPipelineStateDesc.PS = { pixelShaderBlob->GetBufferPointer(),
	pixelShaderBlob->GetBufferSize() };//PixelShader

#pragma region DepthStencilStateの設定
	D3D12_DEPTH_STENCIL_DESC depthStencilDesc{};
	//Depthの機能を有効化する
	depthStencilDesc.DepthEnable = true;
	//書き込みします
	depthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	//比較関数はLessEqual。つまり、近ければ描画される
	depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
#pragma endregion

	//書きこむRYVの情報
	graphicsPipelineStateDesc.NumRenderTargets = 1;
	graphicsPipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	//利用するトボロジ（形状）のタイプ。三角形
	graphicsPipelineStateDesc.PrimitiveTopologyType =
		D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	//どのように画面に色を打ち込むかの設定
	graphicsPipelineStateDesc.SampleDesc.Count = 1;
	graphicsPipelineStateDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;
	//DepthStencil
	graphicsPipelineStateDesc.DepthStencilState = depthStencilDesc;
	graphicsPipelineStateDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	//実際に生成
	Microsoft::WRL::ComPtr <ID3D12PipelineState> graphicsPipelineState = nullptr;
	hr = d3d12.GetDevice()->CreateGraphicsPipelineState(&graphicsPipelineStateDesc,
		IID_PPV_ARGS(&graphicsPipelineState));
	assert(SUCCEEDED(hr));
#pragma endregion
	////////////////////////////////////////////////////////////

#pragma region XAudio2初期化
#pragma endregion
	Music music;
	music.Initialize();
	
#pragma region 入力情報の初期化
#pragma endregion
	input.Initialize(window.GetWindowClass(), window.GetHwnd());

////////////////////////////////////////////////////////////
#pragma region Textureを読み込んで転送これだけでクラス作るアリ
	DirectX::ScratchImage mipImages = LoadTexture("resources/uvChecker.png");
	const DirectX::TexMetadata& metaData = mipImages.GetMetadata();
	Microsoft::WRL::ComPtr <ID3D12Resource> textureResource = CreateTextureResource(d3d12.GetDevice().Get(), metaData);
	UploadTextureData(textureResource.Get(), mipImages);
	//　2枚目のTextureを読む
	DirectX::ScratchImage mipImages2 = LoadTexture(modelData.material.textureFilePath);
	const DirectX::TexMetadata& metaData2 = mipImages2.GetMetadata();
	Microsoft::WRL::ComPtr <ID3D12Resource> textureResource2 = CreateTextureResource(d3d12.GetDevice().Get(), metaData2);
	UploadTextureData(textureResource2.Get(), mipImages2);
#pragma endregion
////////////////////////////////////////////////////////////

	////////////////////////////////////////////////////////////
#pragma region ShaderResourceView(SRV)
	//Textureより下は確定
#pragma region Texture1
	//metaDataをもとにSRVの設定
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Format = metaData.format;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;//2Dテクスチャ
	srvDesc.Texture2D.MipLevels = UINT(metaData.mipLevels);
	//SRVを作成するDescriptorHeapの場所を決める
	D3D12_CPU_DESCRIPTOR_HANDLE textureSrvHandleCPU = GetCPUDescriptorHandle(srvDescriptorHeap.Get(), descriptorSizeSRV, 1);
	D3D12_GPU_DESCRIPTOR_HANDLE textureSrvHandleGPU = GetGPUDescriptorHandle(srvDescriptorHeap.Get(), descriptorSizeSRV, 1);
	//SRVの作成
	d3d12.GetDevice()->CreateShaderResourceView(textureResource.Get(), &srvDesc, textureSrvHandleCPU);
#pragma endregion
#pragma region Texture2
	//metaDataをもとにSRVの設定
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc2{};
	srvDesc2.Format = metaData2.format;
	srvDesc2.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc2.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;//2Dテクスチャ
	srvDesc2.Texture2D.MipLevels = UINT(metaData2.mipLevels);
	//SRVを作成するDescriptorHeapの場所を決める
	D3D12_CPU_DESCRIPTOR_HANDLE textureSrvHandleCPU2 = GetCPUDescriptorHandle(srvDescriptorHeap.Get(), descriptorSizeSRV, 2);
	D3D12_GPU_DESCRIPTOR_HANDLE textureSrvHandleGPU2 = GetGPUDescriptorHandle(srvDescriptorHeap.Get(), descriptorSizeSRV, 2);
	//SRVの作成
	d3d12.GetDevice()->CreateShaderResourceView(textureResource2.Get(), &srvDesc2, textureSrvHandleCPU2);

#pragma endregion
#pragma endregion
	////////////////////////////////////////////////////////////

	////////////////////////////////////////////////////////////
#pragma region DepthStencilView(DSV)
	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc{};
	dsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;//Format。基本的にはResorceに合わせる
	dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;//2DTexture
	//DSVHeapの先頭にDSVを作る
	d3d12.GetDevice()->CreateDepthStencilView(depthStencilResource.Get(), &dsvDesc, dsvDescriptorHeap->GetCPUDescriptorHandleForHeapStart());
#pragma endregion
	////////////////////////////////////////////////////////////

	////////////////////////////////////////////////////////////
#pragma region Resourceにデータを書き込む描画に必要


	//マテリアルにデータを書き込む
	Material* materialData = nullptr;
	//書き込むためのアドレスを取得
	materialResource->Map(0, nullptr, reinterpret_cast<void**>(&materialData));
	//今回は
	materialData->color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	materialData->enableLighting = true;
	materialData->uvTransform = Matrix4x4::Make::Identity();

	//マテリアルにデータを書き込む
	Material* materialDataSprite = nullptr;
	//書き込むためのアドレスを取得
	materialResourceSprite->Map(0, nullptr, reinterpret_cast<void**>(&materialDataSprite));
	//今回は白
	materialDataSprite->color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	materialDataSprite->enableLighting = false;
	materialDataSprite->uvTransform = Matrix4x4::Make::Identity();

#pragma region TransformationMatrix系
	//データを書き込む
	TransformationMatrix* wvpData = nullptr;
	//書き込むためのアドレスを取得
	wvpResource->Map(0, nullptr, reinterpret_cast<void**>(&wvpData));
	//単位行列を書き込んでいく
	wvpData->WVP = Matrix4x4::Make::Identity();
	wvpData->World = Matrix4x4::Make::Identity();
	//データを書き込み
	TransformationMatrix* transformationMatrixDataSprite = nullptr;
	//書きこむためのアドレスを習得
	transformationMatrixResourceSprite->Map(0, nullptr, reinterpret_cast<void**>(&transformationMatrixDataSprite));
	//単位行列を書き込む
	transformationMatrixDataSprite->World = Matrix4x4::Make::Identity();
	transformationMatrixDataSprite->WVP = Matrix4x4::Make::Identity();
#pragma endregion

	////////////////////////////////////////////////////////////
#pragma region VertexBufferView
	//D3D12_VERTEX_BUFFER_VIEW vertexBufferView = {};
	////リソースの先頭のアドレスから使う
	//vertexBufferView.BufferLocation = vertexResource->GetGPUVirtualAddress();
	////使用するリソースのサイズはちゅてん三つ分
	//vertexBufferView.SizeInBytes = sizeof(VertexData) * vertexCount;//
	////１個当たりのサイズ
	//vertexBufferView.StrideInBytes = sizeof(VertexData);//
	// 頂点バッファービューを作成する
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView{};
	vertexBufferView.BufferLocation = vertexResource->GetGPUVirtualAddress();
	vertexBufferView.SizeInBytes = UINT(sizeof(VertexData) * modelData.vertices.size());
	vertexBufferView.StrideInBytes = sizeof(VertexData);
#pragma region Sprite
	D3D12_VERTEX_BUFFER_VIEW vertexBufferViewSprite = {};
	//リソースの先頭のアドレスから使う
	vertexBufferViewSprite.BufferLocation = vertexResourceSprite->GetGPUVirtualAddress();
	//使用するリソースのサイズはちゅてん三つ分
	vertexBufferViewSprite.SizeInBytes = sizeof(VertexData) * 4;//
	//１個当たりのサイズ
	vertexBufferViewSprite.StrideInBytes = sizeof(VertexData);//

#pragma region IndexSprite
	D3D12_INDEX_BUFFER_VIEW indexBufferViewSprite = {};
	// リソースの先頭のアドレスから使う
	indexBufferViewSprite.BufferLocation = indexResourceSprite->GetGPUVirtualAddress();
	// 使用するリソースのサイズはインデックス６つ分のサイズ
	indexBufferViewSprite.SizeInBytes = sizeof(uint32_t) * 6;
	// インデックスはuint32_tとする
	indexBufferViewSprite.Format = DXGI_FORMAT_R32_UINT;
#pragma endregion

#pragma endregion
#pragma endregion
	////////////////////////////////////////////////////////////
#pragma region VertexData
	//頂点リソースにでーたを書き込む1
	//VertexData* vertexData = nullptr;
	//書き込む溜めのアドレスを取得
	//vertexResource->Map(0, nullptr, reinterpret_cast<void**>(&vertexData));
#pragma region 三角形の描画
	////左下
	//vertexData[0].position = { -0.5f,-0.5f,0.0f,1.0f };
	//vertexData[0].texcoord = { 0.0f,1.0f };
	////上
	//vertexData[1].position = { 0.0f,0.5f,0.0f,1.0f };
	//vertexData[1].texcoord = { 0.5f,0.0f };
	////右下
	//vertexData[2].position = { 0.5f,-0.5f,0.0f,1.0f };
	//vertexData[2].texcoord = { 1.0f,1.0f };

	////左下2
	//vertexData[0 + 3].position = { -0.5f,-0.5f,0.5f,1.0f };
	//vertexData[0 + 3].texcoord = { 0.0f,1.0f };
	////上2
	//vertexData[1 + 3].position = { 0.0f,0.0f,0.0f,1.0f };
	//vertexData[1 + 3].texcoord = { 0.5f,0.0f };
	////右下2
	//vertexData[2 + 3].position = { 0.5f,-0.5f,-0.5f,1.0f };
	//vertexData[2 + 3].texcoord = { 1.0f,1.0f };
#pragma endregion

#pragma region ModelData
	// 頂点リソースにデータを書き込む
	VertexData* vertexData = nullptr;
	vertexResource->Map(0, nullptr, reinterpret_cast<void**>(&vertexData));
	std::memcpy(vertexData, modelData.vertices.data(), sizeof(VertexData) * modelData.vertices.size());
#pragma endregion

#pragma region 球の描画

	//uint32_t latIndex = 0;
	//uint32_t lonIndex = 0;
	////経度分割１つ分の角度φd
	//const float kLonEvery = pi * 2.0f / static_cast<float>(kSubdivision);
	////緯度分割1つ分の角度θd
	//const float kLatEvery = pi / static_cast<float>(kSubdivision);
	////緯度の方向に分割
	//for (latIndex = 0;latIndex < kSubdivision; ++latIndex) {
	//	float lat = -pi / 2.0f + kLatEvery * latIndex;//θ
	//	//緯度の方向に分割しながら線を描く
	//	for (lonIndex = 0;lonIndex < kSubdivision;++lonIndex) {
	//		uint32_t start = static_cast<uint32_t>(latIndex * kSubdivision + lonIndex) * 6;
	//		float lon = lonIndex * kLonEvery;//φ
	//		//頂点にデータを入力する。基準点a
	//		vertexData[start + 0].position.x = std::cos(lat) * std::cos(lon);
	//		vertexData[start + 0].position.y = std::sin(lat);
	//		vertexData[start + 0].position.z = std::cos(lat) * std::sin(lon);
	//		vertexData[start + 0].position.w = 1.0f;
	//		vertexData[start + 0].texcoord.x = lonIndex / kSubdivision;
	//		vertexData[start + 0].texcoord.y = 1.0f - float(latIndex) / float(kSubdivision);
	//		//基準点b
	//		vertexData[start + 1].position.x = std::cos(lat + kLatEvery) * std::cos(lon);
	//		vertexData[start + 1].position.y = std::sin(lat + kLatEvery);
	//		vertexData[start + 1].position.z = std::cos(lat + kLatEvery) * std::sin(lon);
	//		vertexData[start + 1].position.w = 1.0f;
	//		vertexData[start + 1].texcoord.x = lonIndex / kSubdivision;
	//		vertexData[start + 1].texcoord.y = 1.0f - float(latIndex + 1) / float(kSubdivision);
	//		//基準点c
	//		vertexData[start + 2].position.x = std::cos(lat) * std::cos(lon + kLonEvery);
	//		vertexData[start + 2].position.y = std::sin(lat);
	//		vertexData[start + 2].position.z = std::cos(lat) * std::sin(lon + kLonEvery);
	//		vertexData[start + 2].position.w = 1.0f;
	//		vertexData[start + 2].texcoord.x = (lonIndex + 1) / kSubdivision;
	//		vertexData[start + 2].texcoord.y = 1.0f - float(latIndex) / float(kSubdivision);
	//		//基準点d
	//		vertexData[start + 3].position.x = std::cos(lat + kLatEvery) * std::cos(lon + kLonEvery);
	//		vertexData[start + 3].position.y = std::sin(lat + kLatEvery);
	//		vertexData[start + 3].position.z = std::cos(lat + kLatEvery) * std::sin(lon + kLonEvery);
	//		vertexData[start + 3].position.w = 1.0f;
	//		vertexData[start + 3].texcoord.x = (lonIndex + 1) / kSubdivision;
	//		vertexData[start + 3].texcoord.y = 1.0f - float(latIndex + 1) / float(kSubdivision);
	//		//基準点e
	//		vertexData[start + 4].position.x = std::cos(lat) * std::cos(lon + kLonEvery);
	//		vertexData[start + 4].position.y = std::sin(lat);
	//		vertexData[start + 4].position.z = std::cos(lat) * std::sin(lon + kLonEvery);
	//		vertexData[start + 4].position.w = 1.0f;
	//		vertexData[start + 4].texcoord.x = (lonIndex + 1) / kSubdivision;
	//		vertexData[start + 4].texcoord.y = 1.0f - float(latIndex) / float(kSubdivision);
	//		//基準点f
	//		vertexData[start + 5].position.x = std::cos(lat + kLatEvery) * std::cos(lon);
	//		vertexData[start + 5].position.y = std::sin(lat + kLatEvery);
	//		vertexData[start + 5].position.z = std::cos(lat + kLatEvery) * std::sin(lon);
	//		vertexData[start + 5].position.w = 1.0f;
	//		vertexData[start + 5].texcoord.x = lonIndex / kSubdivision;
	//		vertexData[start + 5].texcoord.y = 1.0f - float(latIndex + 1) / float(kSubdivision);

	//		for (int i = 0;i < 6;i++) {
	//			vertexData[start + i].normal.x = vertexData[start + i].position.x;
	//			vertexData[start + i].normal.y = vertexData[start + i].position.y;
	//			vertexData[start + i].normal.z = vertexData[start + i].position.z;
	//		}
	//	}
	//}
#pragma endregion

#pragma region Sprite
	VertexData* vertexDataSprite = nullptr;
	vertexResourceSprite->Map(0, nullptr, reinterpret_cast<void**>(&vertexDataSprite));
	//1枚目の三角形
	vertexDataSprite[0].position = { 0.0f,360.0f,0.0f,1.0f };
	vertexDataSprite[0].texcoord = { 0.0f,1.0f };
	vertexDataSprite[1].position = { 0.0f,0.0f,0.0f,1.0f };
	vertexDataSprite[1].texcoord = { 0.0f,0.0f };
	vertexDataSprite[2].position = { 640.0f,360.0f,0.0f,1.0f };
	vertexDataSprite[2].texcoord = { 1.0f,1.0f };
	vertexDataSprite[3].position = { 640.0f, 0.0f, 0.0f, 1.0f };
	vertexDataSprite[3].texcoord = { 1.0f, 0.0f };

#pragma region IndexSprite
	// インデックスリソースにデータを書き込む
	uint32_t* indexDataSprite = nullptr;
	indexResourceSprite->Map(0, nullptr, reinterpret_cast<void**>(&indexDataSprite));
	indexDataSprite[0] = 0;indexDataSprite[1] = 1;indexDataSprite[2] = 2;
	indexDataSprite[3] = 1;indexDataSprite[4] = 3;indexDataSprite[5] = 2;
#pragma endregion

#pragma endregion

#pragma endregion

#pragma region DictionalLightData
	DirectionalLight* directionalLightData = nullptr;
	//書き込むためのアドレスを取得
	directionLightResource->Map(0, nullptr, reinterpret_cast<void**>(&directionalLightData));
	//デフォルト値
	directionalLightData->color = { 1.0f,1.0f,1.0f,1.0f };
	directionalLightData->direction = { 0.0f,-1.0f,0.0f };
	directionalLightData->intensity = 1.0f;
#pragma endregion

#pragma endregion
	////////////////////////////////////////////////////////////

	////////////////////////////////////////////////////////////
#pragma region ViewportとScissor独立しているが後回し
	//ビューポート
	D3D12_VIEWPORT viewport = {};
	//クライアント領域のサイズと一緒にして画面全体に表示
	viewport.Width = static_cast<float>(window.GetWindowRect().right);
	viewport.Height = static_cast<float>(window.GetWindowRect().bottom);
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	//シザー矩形
	D3D12_RECT scissorRect = {};
	//個本的にビューポート同じ矩形が構成されるようにする
	scissorRect.left = 0;
	scissorRect.right = window.GetWindowRect().right;
	scissorRect.top = 0;
	scissorRect.bottom = window.GetWindowRect().bottom;
#pragma endregion
	////////////////////////////////////////////////////////////

	ImGuiManager::SetImGui(window.GetHwnd(),d3d12.GetDevice().Get(),srvDescriptorHeap.Get());

	MSG msg{};

	float left = static_cast<float>(scissorRect.left);
	float right = static_cast<float>(scissorRect.right);
	float top = static_cast<float>(scissorRect.top);
	float bottom = static_cast<float>(scissorRect.bottom);

	bool useMonsterBall = true;
	Transform transform = { {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f} };
	Transform transformSprite{ {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f} };
	Transform uvTransformSprite{
		{1.0f,1.0f,1.0f},
		{0.0f,0.0f,0.0f},
		{0.0f,0.0f,0.0f},
	};
	Transform camera = { 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, -5.0f };
	DebugCamera debugCamera;
	debugCamera.Initialize();
	
	music.GetBGM().LoadWAVE("resources/loop101204.wav");
	music.GetBGM().SetPlayAudioBuf();
	//ウィンドウのｘボタンが押されるまでループ
	while (msg.message != WM_QUIT) {
		
		//Widnowにメッセージが来てたら最優先で処理させる
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) 
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else {
			ImGuiManager::BeginFrame();

			// キー情報の更新があった
			input.Update();
			

////////////////////////////////////////////////////////////
#pragma region コマンドを積み込んで確定させる
	//これから書き込むバックバッファのインデックスを取得
			UINT backBufferIndex = SwapChain::swapChain_->GetCurrentBackBufferIndex();

			ResourceBarrier barrier = {};
			barrier.SetBarrier(command.GetList().GetList().Get(), swapChainResources[backBufferIndex].Get(),
				D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);

			//描画先のRTVを設定する
			command.GetList().GetList()->OMSetRenderTargets(1, &rtvHandles[backBufferIndex], false, nullptr);
			//描画先のRTVとDSVを設定する
			D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = dsvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
			command.GetList().GetList()->OMSetRenderTargets(1, &rtvHandles[backBufferIndex], false, &dsvHandle);
			//指定した色で画面全体をクリアする
			float clearColor[] = { 0.1f,0.25f,0.5f,1.0f };//RGBAの設定
			command.GetList().GetList()->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);//
			command.GetList().GetList()->ClearRenderTargetView(rtvHandles[backBufferIndex], clearColor, 0, nullptr);
			
#pragma endregion
////////////////////////////////////////////////////////////
			debugCamera.UpData();

			//ゲームの処理
			Matrix4x4 worldMatrix = Matrix4x4::Make::Affine(transform.scale, transform.rotate, transform.translate);
			wvpData->WVP = debugCamera.DrawCamera(worldMatrix);
			wvpData->World = worldMatrix;
			//*wvpData = worldMatrix;

			Matrix4x4 worldMatrixSprite = Matrix4x4::Make::Affine(transformSprite.scale, transformSprite.rotate, transformSprite.translate);
			Matrix4x4 viewMatrixSprite = Matrix4x4::Make::Identity();
			Matrix4x4 projectionMatrixSprite = Matrix4x4::Make::OrthoGraphic(0.0f, 0.0f, static_cast<float>(window.GetWindowRect().right), static_cast<float>(window.GetWindowRect().bottom), 0.0f, 100.0f);
			Matrix4x4 worldViewProjectionMatrixSprite = Matrix4x4::Multiply(worldMatrixSprite, Matrix4x4::Multiply(viewMatrixSprite, projectionMatrixSprite));
			transformationMatrixDataSprite->WVP = worldViewProjectionMatrixSprite;
			transformationMatrixDataSprite->World = worldMatrixSprite;

			Matrix4x4 uvTransformMatrix = Matrix4x4::Make::Scale(uvTransformSprite.scale);
			uvTransformMatrix = Matrix4x4::Multiply(uvTransformMatrix, Matrix4x4::Make::RotateZ(uvTransformSprite.rotate.z));
			uvTransformMatrix = Matrix4x4::Multiply(uvTransformMatrix, Matrix4x4::Make::Translate(uvTransformSprite.translate));
			materialDataSprite->uvTransform = uvTransformMatrix;

			////////////////////////////////////////////////////////////

#pragma region FPS
			ImGui::Begin("FPS");
			ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);
			ImGui::End();
#pragma endregion
#pragma region ImGui1系
			ImGui::Begin("02");
			ImGui::Checkbox("UseMonsterBall", &useMonsterBall);
			ImGuiManager::CreateImGui("Translate", transform.translate, -0.5f, 0.5f);
			ImGuiManager::CreateImGui("Rotate", transform.rotate, -180.0f, 180.0f);
			ImGuiManager::CreateImGui("RGB", materialData->color, 0.0f, 1.0f);
			ImGuiManager::CreateImGui("TransFromSpriteX", transformSprite.translate, -0.5f, 640.0f);
			ImGuiManager::CreateImGui("LightColor", directionalLightData->color, 0, 1);
			ImGuiManager::CreateImGui("LightDirectional", directionalLightData->direction, -1.0f, 1.0f);
			ImGuiManager::CreateImGui("Intensity", directionalLightData->intensity, 0, 1);
			ImGui::GetFrameCount();
			ImGui::End();
#pragma endregion
#pragma region ImGuiUVTransform
			ImGui::Begin("UV系");
			ImGui::DragFloat2("UVTranslate", &uvTransformSprite.translate.x, 0.01f, -10.0f, 10.0f);
			ImGui::DragFloat2("UVScale", &uvTransformSprite.scale.x, 0.01f, -10.0f, 10.0f);
			ImGui::SliderAngle("UVRotate", &uvTransformSprite.rotate.z);
			ImGui::End();
#pragma endregion
#pragma region Scisser
			ImGui::Begin("scissorRect");
			ImGuiManager::CreateImGui("left", left, 0.0f, 10.0f);
			ImGuiManager::CreateImGui("right", right, 0.0f, static_cast<float>(window.GetWindowRect().right));
			ImGuiManager::CreateImGui("top", top, 0.0f, 10.0f);
			ImGuiManager::CreateImGui("bottom", bottom, 0.0f, static_cast<float>(window.GetWindowRect().bottom));
			ImGui::End();
			scissorRect.left = static_cast<LONG>(left);
			scissorRect.right = static_cast<LONG>(right);
			scissorRect.top = static_cast<LONG>(top);
			scissorRect.bottom = static_cast<LONG>(bottom);
#pragma endregion
			
			////////////////////////////////////////////////////////////
			
			
			ID3D12DescriptorHeap* descriptorHeaps[] = { srvDescriptorHeap.Get()};
			command.GetList().GetList()->SetDescriptorHeaps(1, descriptorHeaps);
			/////////////////////////////////////////////////////////////////////////
			
			//描画0200
			command.GetList().GetList()->RSSetViewports(1, &viewport);
			command.GetList().GetList()->RSSetScissorRects(1, &scissorRect);

			//RootSignalの設定
			command.GetList().GetList()->SetGraphicsRootSignature(rootSignature.Get());
			command.GetList().GetList()->SetPipelineState(graphicsPipelineState.Get());
			command.GetList().GetList()->IASetVertexBuffers(0, 1, &vertexBufferView);

			//形状を設定
			command.GetList().GetList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
			//マテリアルCBufferの場所を設定
			command.GetList().GetList()->SetGraphicsRootConstantBufferView(0, materialResource->GetGPUVirtualAddress());
			//wvp用のCBufferの場所を設定
			command.GetList().GetList()->SetGraphicsRootConstantBufferView(1, wvpResource->GetGPUVirtualAddress());
			//DirectionLight用のCBufferの場所を設定
			command.GetList().GetList()->SetGraphicsRootConstantBufferView(3, directionLightResource->GetGPUVirtualAddress());
			//SRVのDescritorTableの先頭を設定。2はrootParameter[2]である
			command.GetList().GetList()->SetGraphicsRootDescriptorTable(2, /*useMonsterBall ? textureSrvHandleGPU2 : */textureSrvHandleGPU);
			//描画
			command.GetList().GetList()->DrawInstanced(UINT(modelData.vertices.size()), 1, 0, 0);
			/////////////////////////////////////////////////////////////////////////
			
			// Spriteの描画。変更が必要なものだけ変更する
			command.GetList().GetList()->IASetVertexBuffers(0, 1, &vertexBufferViewSprite);
			// IndexBufferView(IBV)の設定
			command.GetList().GetList()->IASetIndexBuffer(&indexBufferViewSprite);
			// TransformationMatrixの場所を設定
			command.GetList().GetList()->SetGraphicsRootConstantBufferView(0, materialResourceSprite->GetGPUVirtualAddress());
			command.GetList().GetList()->SetGraphicsRootConstantBufferView(1, transformationMatrixResourceSprite->GetGPUVirtualAddress());
			command.GetList().GetList()->SetGraphicsRootDescriptorTable(2, textureSrvHandleGPU);
			// 描画
			//command.GetList().GetList()->DrawInstanced(6, 1, 0, 0);
			
			// 描画
			command.GetList().GetList()->DrawIndexedInstanced(6, 1, 0, 0, 0);

			//描画
			ImGuiManager::EndFrame(command.GetList().GetList());

			//画面表示できるようにするために
			//RenderTargetからPresent
			//barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
			//barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
			////TransitionBarrierを張る
			//command.GetList().GetList()->ResourceBarrier(1, &barrier);

			barrier.SetTransition(command.GetList().GetList().Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);

			//画面表示できるようにするために

			//コマンドリストの内容を確定させる。全てのコマンドを積んでからCloseすること
			hr = command.GetList().GetList()->Close();
			assert(SUCCEEDED(hr));

////////////////////////////////////////////////////////////
#pragma region 積んだコマンドをキックする
	//GPUにコマンドリストの実行を行わさせる
			ID3D12CommandList* commandLists[] = { command.GetList().GetList().Get()};
			command.GetQueue().GetQueue()->ExecuteCommandLists(1, commandLists);


			//GPUとOSに画面の交換を行うように通知する
			SwapChain::swapChain_->Present(1, 0);

			TachyonSync::GetCGPU().Update(command.GetQueue().GetQueue().Get());


			//次のフレーム用のコマンドリストを準備
			hr = command.GetList().GetAllocator()->Reset();
			assert(SUCCEEDED(hr));
			hr = command.GetList().GetList()->Reset(command.GetList().GetAllocator().Get(), nullptr);
			assert(SUCCEEDED(hr));
#pragma endregion
////////////////////////////////////////////////////////////
		}
		
	}
	ImGuiManager::Shutdown();

	//解放処理
	TachyonSync::GetCGPU().UnLoad();
	signatureBlob->Release();
	if (errorBlob) {
		errorBlob->Release();
	}
	//rootSignature->Release();
	pixelShaderBlob->Release();
	vertexShaderBlob->Release();

	//xAudio2の解放
	//xAudio2.Reset();
	// 音声データの開放
	//SoundUnload(&soundData1);
	music.UnLoad();

#ifdef _DEBUG
	//debugController->Release();
#endif // _DEBUG
	CloseWindow(window.GetHwnd());

	//COMの初期化を解除
	CoUninitialize();
	return 0;
}
