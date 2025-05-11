#include <windows.h>
#include <cstdint>
#include <d3d12.h>
#include <dxgi1_6.h>
#include <cassert>
#include <string>
#include <format>
#include <filesystem>
#include <fstream>//ファイルに書いたり読んだりするライブラリ
#include <chrono>//時間を扱うライブラリ
#include <dbghelp.h>//CrashHandler06
#include <strsafe.h>//Dumpを出力06
#include <dxgidebug.h>//0103 ReportLiveobject
#include <dxcapi.h>//DXCの初期化
#include <cmath>

#include "externals/imgui/imgui.h"
#include "externals/imgui/imgui_impl_dx12.h"
#include "externals/imgui/imgui_impl_win32.h"
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

#include "externals/DirectXTex/DirectXTex.h"

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib,"dxgi.lib")
#pragma comment(lib,"Dbghelp.lib")//ClashHandler06
#pragma comment(lib,"dxguid.lib")//0103 ReportLiveobject
#pragma comment(lib,"dxcompiler.lib")//DXCの初期化

#define pi float(3.14159265358979323846f)

struct Vector3 {
	float x, y, z;
};
struct Transform {
	Vector3 scale;
	Vector3 rotate;
	Vector3 translate;
};
struct Ortho {
	float left, right, top, bottom;
};
float cot(float theta) {
	float ret = 1.0f / std::tanf(theta);
	return ret;
}
struct Vector2 {
	float x, y;
};
// Vector4
struct Vector4 {
	float x, y, z, w;
};

struct VertexData {
	Vector4 position;
	Vector2 texcoord;//u座標とv座標のことかな
};

struct Matrix4x4 {
	float m[4][4];
};
//行列の積
Matrix4x4 Multiply(const Matrix4x4& m1, const Matrix4x4 m2) {
	float a11 = m1.m[0][0], a12 = m1.m[0][1], a13 = m1.m[0][2], a14 = m1.m[0][3],
		a21 = m1.m[1][0], a22 = m1.m[1][1], a23 = m1.m[1][2], a24 = m1.m[1][3],
		a31 = m1.m[2][0], a32 = m1.m[2][1], a33 = m1.m[2][2], a34 = m1.m[2][3],
		a41 = m1.m[3][0], a42 = m1.m[3][1], a43 = m1.m[3][2], a44 = m1.m[3][3];
	float b11 = m2.m[0][0], b12 = m2.m[0][1], b13 = m2.m[0][2], b14 = m2.m[0][3],
		b21 = m2.m[1][0], b22 = m2.m[1][1], b23 = m2.m[1][2], b24 = m2.m[1][3],
		b31 = m2.m[2][0], b32 = m2.m[2][1], b33 = m2.m[2][2], b34 = m2.m[2][3],
		b41 = m2.m[3][0], b42 = m2.m[3][1], b43 = m2.m[3][2], b44 = m2.m[3][3];
	Matrix4x4 ret = {
		a11 * b11 + a12 * b21 + a13 * b31 + a14 * b41,  a11 * b12 + a12 * b22 + a13 * b32 + a14 * b42,  a11 * b13 + a12 * b23 + a13 * b33 + a14 * b43,  a11 * b14 + a12 * b24 + a13 * b34 + a14 * b44,
		a21 * b11 + a22 * b21 + a23 * b31 + a24 * b41,  a21 * b12 + a22 * b22 + a23 * b32 + a24 * b42,  a21 * b13 + a22 * b23 + a23 * b33 + a24 * b43,  a21 * b14 + a22 * b24 + a23 * b34 + a24 * b44,
		a31 * b11 + a32 * b21 + a33 * b31 + a34 * b41,  a31 * b12 + a32 * b22 + a33 * b32 + a34 * b42,  a31 * b13 + a32 * b23 + a33 * b33 + a34 * b43,  a31 * b14 + a32 * b24 + a33 * b34 + a34 * b44,
		a41 * b11 + a42 * b21 + a43 * b31 + a44 * b41,  a41 * b12 + a42 * b22 + a43 * b32 + a44 * b42,  a41 * b13 + a42 * b23 + a43 * b33 + a44 * b43,  a41 * b14 + a42 * b24 + a43 * b34 + a44 * b44
	};
	return ret;
}
//行列の単位
Matrix4x4 MakeIdentity4x4() {
	Matrix4x4 ret = {
		1,0,0,0,
		0,1,0,0,
		0,0,1,0,
		0,0,0,1
	};
	return ret;
}
Matrix4x4 MakeScaleMatrix(const Vector3& scale) {
	Matrix4x4 ret = {
		scale.x,0,0,0,
		0,scale.y,0,0,
		0,0,scale.z,0,
		0,0,0,1
	};
	return ret;
}
//ｘ軸の回転
Matrix4x4 MakeRotateXMatrix(float radian) {
	Matrix4x4 ret = {
		1,0,0,0,
		0,std::cos(radian),std::sin(radian),0,
		0,-std::sin(radian),std::cos(radian),0,
		0,0,0,1
	};
	return ret;
}
//ｙ軸の回転
Matrix4x4 MakeRotateYMatrix(float radian) {
	Matrix4x4 ret = {
		std::cos(radian),0,-std::sin(radian),0,
		0,1,0,0,
		std::sin(radian),0,std::cos(radian),0,
		0,0,0,1
	};
	return ret;
}
//ｚ軸の回転
Matrix4x4 MakeRotateZMatrix(float radian) {
	Matrix4x4 ret = {
		std::cos(radian),std::sin(radian),0,0,
		-std::sin(radian),std::cos(radian),0,0,
		0,0,1,0,
		0,0,0,1
	};
	return ret;
}
Matrix4x4 MakeTranslateMatrix(const Vector3& translate) {
	Matrix4x4 ret = {
		1,0,0,0,
		0,1,0,0,
		0,0,1,0,
		translate.x,translate.y,translate.z,1
	};
	return ret;
}
Matrix4x4 MakeOrthoGraphicMatrix(float left,float top,float right,float bottom,float nearClip,float farClip) {
	Matrix4x4 ret;
	ret.m[0][0] = 2.0f / (right - left);
	ret.m[0][1] = 0;
	ret.m[0][2] = 0;
	ret.m[0][3] = 0;

	ret.m[1][0] = 0;
	ret.m[1][1] = 2.0f / (top - bottom);
	ret.m[1][2] = 0;
	ret.m[1][3] = 0;

	ret.m[2][0] = 0;
	ret.m[2][1] = 0;
	ret.m[2][2] = 1.0f / (farClip - nearClip);
	ret.m[2][3] = 0;

	ret.m[3][0] = (right + left) / (left - right);
	ret.m[3][1] = (top + bottom) / (bottom - top);
	ret.m[3][2] = nearClip / (nearClip - farClip);
	ret.m[3][3] = 1.0f;
	return ret;
}
Matrix4x4 MakeAffineMatrix(const Vector3& scale, const Vector3& rotate, const Vector3& translate) {
	//SRT
	// - [ S ] - //
	Matrix4x4 SRTMatrix = Multiply(MakeScaleMatrix(scale), Multiply(MakeRotateXMatrix(rotate.x), Multiply(MakeRotateYMatrix(rotate.y), MakeRotateZMatrix(rotate.z))));
	SRTMatrix = Multiply(SRTMatrix, MakeTranslateMatrix(translate));
	return SRTMatrix;
}
//透視投影行列
Matrix4x4 MakePerspectiveFovMatrix(float fovY, float aspectRatio, float nearClip, float farClip) {
	Matrix4x4 ret = {
		cot(fovY / 2.0f) / aspectRatio, 0, 0, 0,
		0, cot(fovY / 2.0f), 0, 0,
		0, 0, farClip / (farClip - nearClip), 1,
		0, 0, (-nearClip * farClip) / (farClip - nearClip), 0
	};
	return ret;
}
//行列の逆
Matrix4x4 Inverse(const Matrix4x4& m) {
	float a11 = m.m[0][0], a12 = m.m[0][1], a13 = m.m[0][2], a14 = m.m[0][3],
		a21 = m.m[1][0], a22 = m.m[1][1], a23 = m.m[1][2], a24 = m.m[1][3],
		a31 = m.m[2][0], a32 = m.m[2][1], a33 = m.m[2][2], a34 = m.m[2][3],
		a41 = m.m[3][0], a42 = m.m[3][1], a43 = m.m[3][2], a44 = m.m[3][3];
	float A = a11 * a22 * a33 * a44 + a11 * a23 * a34 * a42 + a11 * a24 * a32 * a43
		- a11 * a24 * a33 * a42 - a11 * a23 * a32 * a44 - a11 * a22 * a34 * a43
		- a12 * a21 * a33 * a44 - a13 * a21 * a34 * a42 - a14 * a21 * a32 * a43
		+ a14 * a21 * a33 * a42 + a13 * a21 * a32 * a44 + a12 * a21 * a34 * a43
		+ a12 * a23 * a31 * a44 + a13 * a24 * a31 * a42 + a14 * a22 * a31 * a43
		- a14 * a23 * a31 * a42 - a13 * a22 * a31 * a44 - a12 * a24 * a31 * a43
		- a12 * a23 * a34 * a41 - a13 * a24 * a32 * a41 - a14 * a22 * a33 * a41
		+ a14 * a23 * a32 * a41 + a13 * a22 * a34 * a41 + a12 * a24 * a33 * a41;
	Matrix4x4 ret = {
		(a22 * a33 * a44 + a23 * a34 * a42 + a24 * a32 * a43 - a24 * a33 * a42 - a23 * a32 * a44 - a22 * a34 * a43) / A,
		(-a12 * a33 * a44 - a13 * a34 * a42 - a14 * a32 * a43 + a14 * a33 * a42 + a13 * a32 * a44 + a12 * a34 * a43) / A,
		(a12 * a23 * a44 + a13 * a24 * a42 + a14 * a22 * a43 - a14 * a23 * a42 - a13 * a22 * a44 - a12 * a24 * a43) / A,
		(-a12 * a23 * a34 - a13 * a24 * a32 - a14 * a22 * a33 + a14 * a23 * a32 + a13 * a22 * a34 + a12 * a24 * a33) / A,

		(-a21 * a33 * a44 - a23 * a34 * a41 - a24 * a31 * a43 + a24 * a33 * a41 + a23 * a31 * a44 + a21 * a34 * a43) / A,
		(a11 * a33 * a44 + a13 * a34 * a41 + a14 * a31 * a43 - a14 * a33 * a41 - a13 * a31 * a44 - a11 * a34 * a43) / A,
		(-a11 * a23 * a44 - a13 * a24 * a41 - a14 * a21 * a43 + a14 * a23 * a41 + a13 * a21 * a44 + a11 * a24 * a43) / A,
		(a11 * a23 * a34 + a13 * a24 * a31 + a14 * a21 * a33 - a14 * a23 * a31 - a13 * a21 * a34 - a11 * a24 * a33) / A,

		(a21 * a32 * a44 + a22 * a34 * a41 + a24 * a31 * a42 - a24 * a32 * a41 - a22 * a31 * a44 - a21 * a34 * a42) / A,
		(-a11 * a32 * a44 - a12 * a34 * a41 - a14 * a31 * a42 + a14 * a32 * a41 + a12 * a31 * a44 + a11 * a34 * a42) / A,
		(a11 * a22 * a44 + a12 * a24 * a41 + a14 * a21 * a42 - a14 * a22 * a41 - a12 * a21 * a44 - a11 * a24 * a42) / A,
		(-a11 * a22 * a34 - a12 * a24 * a31 - a14 * a21 * a32 + a14 * a22 * a31 + a12 * a21 * a34 + a11 * a24 * a32) / A,

		(-a21 * a32 * a43 - a22 * a33 * a41 - a23 * a31 * a42 + a23 * a32 * a41 + a22 * a31 * a43 + a21 * a33 * a42) / A,
		(a11 * a32 * a43 + a12 * a33 * a41 + a13 * a31 * a42 - a13 * a32 * a41 - a12 * a31 * a43 - a11 * a33 * a42) / A,
		(-a11 * a22 * a43 - a12 * a23 * a41 - a13 * a21 * a42 + a13 * a22 * a41 + a12 * a21 * a43 + a11 * a23 * a42) / A,
		(a11 * a22 * a33 + a12 * a23 * a31 + a13 * a21 * a32 - a13 * a22 * a31 - a12 * a21 * a33 - a11 * a23 * a32) / A
	};
	return ret;
}

//ウィンドウプロシージャ
LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg,
	WPARAM wparam, LPARAM lparam) {
	if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, wparam, lparam)) {
		return true;
	}
	//メッセージに応じてゲーム固有の処理を行う
	switch (msg) {
		//ウィンドウが破棄された
	case WM_DESTROY:
		//OSに対して、アプリの終了を伝える
		PostQuitMessage(0);
		return 0;
	}

	//標準のメッセージ処理を行う
	return DefWindowProc(hwnd, msg, wparam, lparam);
}

std::wstring ConvertString(const std::string& str) {
	if (str.empty()) {
		return std::wstring();
	}

	auto sizeNeeded = MultiByteToWideChar(CP_UTF8, 0, reinterpret_cast<const char*>(&str[0]), static_cast<int>(str.size()), NULL, 0);
	if (sizeNeeded == 0) {
		return std::wstring();
	}
	std::wstring result(sizeNeeded, 0);
	MultiByteToWideChar(CP_UTF8, 0, reinterpret_cast<const char*>(&str[0]), static_cast<int>(str.size()), &result[0], sizeNeeded);
	return result;
}

std::string ConvertString(const std::wstring& str) {
	if (str.empty()) {
		return std::string();
	}

	auto sizeNeeded = WideCharToMultiByte(CP_UTF8, 0, str.data(), static_cast<int>(str.size()), NULL, 0, NULL, NULL);
	if (sizeNeeded == 0) {
		return std::string();
	}
	std::string result(sizeNeeded, 0);
	WideCharToMultiByte(CP_UTF8, 0, str.data(), static_cast<int>(str.size()), result.data(), sizeNeeded, NULL, NULL);
	return result;
}

//ログ
void Log(const std::string& message) {
	OutputDebugStringA(message.c_str());
}

void Log(std::ostream& os, const std::string& message) {
	os << message << std::endl;
	OutputDebugStringA(message.c_str());
}

//CrashHandler06
static LONG WINAPI ExportDump(EXCEPTION_POINTERS* exception) {
	//ファイルはクラッシュしたexeまたは、ドイツの同一のexeとソースファイルと同じ場所
	//Dumpを出力する↓↓↓

	SYSTEMTIME time;
	GetLocalTime(&time);
	wchar_t filePath[MAX_PATH] = { 0 };
	CreateDirectory(L"./Dumps", nullptr);
	StringCchPrintfW(filePath, MAX_PATH, L"./Dumps/%04d-%02d%02d-%02d%02d.dmp", time.wYear, time.wMonth, time.wDay, time.wHour, time.wMinute);
	HANDLE dumpFileHandle = CreateFile(filePath, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_WRITE | FILE_SHARE_READ, 0, CREATE_ALWAYS, 0, 0);
	//processId(このexeとId)とクラッシュ(例外)の発生したthreatIdを取得
	DWORD processId = GetCurrentProcessId();
	DWORD threadId = GetCurrentThreadId();
	//設定情報を入力
	MINIDUMP_EXCEPTION_INFORMATION minidumpInformation = { 0 };
	minidumpInformation.ThreadId = threadId;
	minidumpInformation.ExceptionPointers = exception;
	minidumpInformation.ClientPointers = TRUE;
	//Dumpを出力。MiniDumpNormalは最低限の情報を出力するフラグ
	MiniDumpWriteDump(GetCurrentProcess(), processId, dumpFileHandle, MiniDumpNormal, &minidumpInformation, nullptr, nullptr);
	//Dumpの出力↑↑↑
	
	//他に関連づけられているSHE例外ハンドラがあれば実行。通常はプロセスを終了する
	return EXCEPTION_EXECUTE_HANDLER;
}

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
	Log(ConvertString(std::format(L"Begin CompileShader,path:{}, profile:{}\n", filePath, profile)));
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
		Log(shaderError->GetStringPointer());
		//警告エラーダメ絶対
		assert(false);
	}
	// ４，Compile結果を受け取って返す
	//コンパイル結果から実行用のバイナリ部分を取得
	IDxcBlob* shaderBlob = nullptr;
	hr = shaderResult->GetOutput(DXC_OUT_OBJECT, IID_PPV_ARGS(&shaderBlob), nullptr);
	assert(SUCCEEDED(hr));
	//成功したログを出す
	Log(ConvertString(std::format(L"CompileSucceeded, path:{},profile:{}\n", filePath, profile)));
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

// windowsアプリでのエントリーポイント（main関数）
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
	int32_t kClienWidth = 1280;
	int32_t kClienHeight = 720;
	//分割数
	static const float kSubdivision = 16.0f;
	//頂点数 (分割数(縦÷緯度)x分割数(横÷経度)x６)
	uint32_t vertexCount = static_cast<uint32_t>(kSubdivision * kSubdivision) * 6;

	//COMの初期化
	CoInitializeEx(0, COINIT_MULTITHREADED);
	//誰も補足しなかった場合に（Unhandled）、補足する関数を登録
	//main関数はじまってすぐに登録するといい
	SetUnhandledExceptionFilter(ExportDump);//06
	//ログのディレクトリを用意
	std::filesystem::create_directory("logs");
	//この準備は実際にログが呼び出されるときの基盤となるらしい↓↓↓
	//現在時刻を取得（UTC時刻）
	std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
	//ログファイルの名前にコンマ何秒はいらないので削って秒にする
	std::chrono::time_point<std::chrono::system_clock, std::chrono::seconds>
    nowSeconds = std::chrono::time_point_cast<std::chrono::seconds>(now);
	//日本時間に変換(PCの設定時間)
	std::chrono::zoned_time localTime{ std::chrono::current_zone(),nowSeconds };
	//Formatを使って年月日＿時分秒の文字列に変換
	std::string dateString = std::format("{:%Y%m%d_%H%M%S}", localTime);
	//時刻を使ってファイル名を決定
	std::string logFilePath = std::string("logs/") + dateString + ".logs";
	//ファイル名をつかって書き込み準備
	std::ofstream logStream(logFilePath);
	//この準備は実際にログが呼び出されるときの基盤となるらしい↑↑↑
	
////////////////////////////////////////////////////////////
#pragma region ウィンドウクラスの登録
	WNDCLASS wc{};
	// ウィンドウプロシージャ
	wc.lpfnWndProc = WindowProc;
	// ウィンドウクラス名（何でもいい）
	wc.lpszClassName = L"CG2WindowClass";
	// インスタンスハンドル
	wc.hInstance = GetModuleHandle(nullptr);
	// カーソル
	wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
	// ウィンドウクラスを登録する
	RegisterClass(&wc);
#pragma endregion
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
#pragma region ウィンドウのサイズ決め
	//クライアント領域（ゲーム画面）のサイズ
	const int32_t kClientWidth = 1280;
	const int32_t kClientHeight = 720;
	//ウィンドウサイズを表す構造体にクライアント領域を入れる
	RECT wrc = { 0, 0, kClientWidth, kClientHeight };
	//クライアント領域をもとに実際のサイズにwrcを変更してもらう
	AdjustWindowRect(&wrc, WS_OVERLAPPEDWINDOW, false);
#pragma endregion
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
#pragma region ウィンドウの生成＆表示
	//ウィンドウの生成
	HWND hwnd = CreateWindow(
		wc.lpszClassName,      	//利用するクラス名
		L"CG2",					//タイトルバーの文字
		WS_OVERLAPPEDWINDOW,	//よく見るウィンドウスタイル
		CW_USEDEFAULT,			//表示X座標
		CW_USEDEFAULT,			//表示Y座標
		wrc.right - wrc.left,	//ウィンドウ横幅
		wrc.bottom - wrc.top,	//ウィンドウ縦幅
		nullptr,				//親ウィンドウハンドル
		nullptr,				//メニューハンドル
		wc.hInstance,			//インスタンスハンドル
		nullptr					//オプション
	);
	//ウィンドウを表示する
	ShowWindow(hwnd, SW_SHOW);
#pragma endregion
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
#pragma region デバッグレイヤー(開発時のデバッグ支援)
#ifdef _DEBUG
	ID3D12Debug1* debugController = nullptr;
	if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)))) {
		//でばっふレイヤーを有効かする
		debugController->EnableDebugLayer();
		//さらにGPU側でもチェックポイントを行うようにする
		debugController->SetEnableGPUBasedValidation(TRUE);
	}
#endif 
#pragma endregion
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
#pragma region DXGIファクトリーの作成
	IDXGIFactory7* dxgiFactory = nullptr;
	//HRESULTはWindows系のエラーコードであり、
	//関数が成功したかどうかをSUCCEEDEDマクロで判定できる
	HRESULT hr = CreateDXGIFactory(IID_PPV_ARGS(&dxgiFactory));
	//初期化の根本的な部分でエラーが出た場合はプログラムが間違っているか、
	// どうにもできない場合が多いのでassertにしておく
	assert(SUCCEEDED(hr));
#pragma endregion
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
#pragma region 使用するアダプタの決定
	//使用するアダプタ用の変数。最初にnullptrを入れておく
	IDXGIAdapter4* useAdapter = nullptr;
	//よい順にアダプタを頼む
	for (UINT i = 0;dxgiFactory->EnumAdapterByGpuPreference(i,
		DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(&useAdapter)) !=
		DXGI_ERROR_NOT_FOUND; ++i) {
		//アダプターの情報を取得する
		DXGI_ADAPTER_DESC3 adapterDesc{};
		hr = useAdapter->GetDesc3(&adapterDesc);
		assert(SUCCEEDED(hr));//取得できないのは一大事
		//ソフトウェアアダプタでなければ採用！
		if (!(adapterDesc.Flags & DXGI_ADAPTER_FLAG3_SOFTWARE)) {
			//採用したアダプタの情報にログに出力。wstringの方なので注意
			Log(logStream, ConvertString(std::format(L"Use Adapter:{}\n", adapterDesc.Description)));
			break;
		}
		useAdapter = nullptr;//ソフトウェアアダプタの場合はみなかったことにする
	}
	//適切なアダプタが見つからなかったので起動できない
	assert(useAdapter != nullptr);
#pragma endregion
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
#pragma region D3D12Deviceの生成
	ID3D12Device* device = nullptr;
	//機能レベルとログ出力用の文字列
	D3D_FEATURE_LEVEL featureLevels[] = {
		D3D_FEATURE_LEVEL_12_2,D3D_FEATURE_LEVEL_12_1,D3D_FEATURE_LEVEL_12_0
	};
	const char* featureLevelStrings[] = { "12.2","12.1","12.0"};
	//高い順に生成できるか試していく
	for (size_t i = 0;i < _countof(featureLevels);++i) {
		//採用したアダプターでデバイスを生成
		hr = D3D12CreateDevice(useAdapter, featureLevels[i], IID_PPV_ARGS(&device));
		//指定した機能レベルでデバイスが生成出来たかを確認
		if (SUCCEEDED(hr)) {
			//生成出来たのでログ出力を行ってループを抜ける
			Log(logStream, (std::format("FeatureLevel : {}\n", featureLevelStrings[i])));
			break;
		}
	}
	//デバイスの生成が上手くいかったので起動できない
	assert(device != nullptr);
	Log(logStream,"Complete create D3D12Device!!!\n");//初期化完了のログを出す
#pragma endregion
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
#pragma region エラー警告 直ちに停止
#ifdef _DEBUG
	ID3D12InfoQueue* infoQueue = nullptr;
	if (SUCCEEDED(device->QueryInterface(IID_PPV_ARGS(&infoQueue)))) {
		//やばいエラー時に止まる
		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, true);
		//エラー時に止まる
		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, true);
		//警告時に止まる(全部の情報を出すために)
		//infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, true);
		//抑制するメッセージのID
		D3D12_MESSAGE_ID denyIds[] = {
			//Windows11出のDXGIデバッグレイヤーとDX12デバッグレイヤーの相互作用バグによるエラーメッセージ
			//https://stackoverflow.com/questions/09885245/directx-12-application-is-crashing-in-window-ww
			D3D12_MESSAGE_ID_RESOURCE_BARRIER_MISMATCHING_COMMAND_LIST_TYPE
		};
		//抑制するレベル
		D3D12_MESSAGE_SEVERITY severities[] = { D3D12_MESSAGE_SEVERITY_INFO };
		D3D12_INFO_QUEUE_FILTER filter = {};
		filter.DenyList.NumIDs = _countof(denyIds);
		filter.DenyList.pIDList = denyIds;
		filter.DenyList.NumSeverities = _countof(severities);
		filter.DenyList.pSeverityList = severities;
		//確定したメッセージの表示を抑制する
		infoQueue->PushStorageFilter(&filter);
		//解放
		infoQueue->Release();
	}
#endif
#pragma endregion
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
#pragma region MaterialResource
	ID3D12Resource* materialResource = CreateBufferResource(device, sizeof(Vector4));
#pragma endregion
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
#pragma region CommandQueueの生成
	//コマンドの生成
	ID3D12CommandQueue* commandQueue = nullptr;
	D3D12_COMMAND_QUEUE_DESC commandQueueDesc{};
	hr = device->CreateCommandQueue(&commandQueueDesc,
		IID_PPV_ARGS(&commandQueue));
	//コマンドキューの生成が上手くいかなかったので起動できない
	assert(SUCCEEDED(hr));
#pragma endregion
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
#pragma region CommandList
	//コマンドアロケーターを生成
	ID3D12CommandAllocator* commandAllocator = nullptr;
	hr = device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&commandAllocator));
	//コマンドアロケータの生成が上手くいかなかったので起動できない
	assert(SUCCEEDED(hr));

	//コマンドリスト
	ID3D12GraphicsCommandList* commandList = nullptr;
	hr = device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, commandAllocator, nullptr,
		IID_PPV_ARGS(&commandList));
	//コマンドリストの生成がうまくいかなかったので起動できない
	assert(SUCCEEDED(hr));
#pragma endregion
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
#pragma region SwapChain
	IDXGISwapChain4* swapChain = nullptr;
	DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
	swapChainDesc.Width = kClientWidth;//画面の幅。ウィンドウのクライアント領域を同じものにしておく
	swapChainDesc.Height = kClientHeight;//画面の高さ。ウィンドウのクライアント領域を同じものにしておく
	swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;//色の形式
	swapChainDesc.SampleDesc.Count = 1;//マルチサンプルしない
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;//描画のターゲットとして利用する
	swapChainDesc.BufferCount = 2;//ダブルバッファ
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;//モニタにうつしたら、中身を破棄
	//コマンドキュー、ウィンドウハンドル、設定を渡して生成する
	hr = dxgiFactory->CreateSwapChainForHwnd(commandQueue, hwnd, &swapChainDesc, nullptr, nullptr, reinterpret_cast<IDXGISwapChain1**>(&swapChain));
	assert(SUCCEEDED(hr));
#pragma endregion
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
#pragma region ディスクリプタヒープの生成
	ID3D12DescriptorHeap* rtvDescriptorHeap = CreateDescriptorHeap(device, D3D12_DESCRIPTOR_HEAP_TYPE_RTV, 2, false);
	ID3D12DescriptorHeap* srvDescriptorHeap = CreateDescriptorHeap(device, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 128, true);
	ID3D12DescriptorHeap* dsvDescriptorHeap = CreateDescriptorHeap(device, D3D12_DESCRIPTOR_HEAP_TYPE_DSV, 1, false);
#pragma endregion
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
#pragma region SwapChainからResourceを引っ張ってくる
	ID3D12Resource* swapChainResources[2] = { nullptr };
	hr = swapChain->GetBuffer(0, IID_PPV_ARGS(&swapChainResources[0]));
	//上手く取得出来なければ起動できない
	assert(SUCCEEDED(hr));
	hr = swapChain->GetBuffer(1, IID_PPV_ARGS(&swapChainResources[1]));
	assert(SUCCEEDED(hr));
#pragma endregion
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
#pragma region VertexResource
	ID3D12Resource* vertexResource = CreateBufferResource(device, sizeof(VertexData) * vertexCount);//
#pragma region Sprite
	ID3D12Resource* vertexResourceSprite = CreateBufferResource(device, sizeof(VertexData) * vertexCount);
#pragma endregion
#pragma endregion
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
#pragma region DepthStencilTexture
	ID3D12Resource* depthStencilResource = CreateDepthStencilTextureResource(device, kClienWidth, kClienHeight);
#pragma endregion
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
#pragma region TransformationMatrix
	ID3D12Resource* wvpResource = CreateBufferResource(device, sizeof(Matrix4x4));
	ID3D12Resource* transformationMatrixResourceSprite = CreateBufferResource(device, sizeof(Matrix4x4));
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
	device->CreateRenderTargetView(swapChainResources[0], &rtvDesc, rtvHandles[0]);
	//2二つ目のディスクリプタ班どりを作る（自力で）
	rtvHandles[1].ptr = rtvHandles[0].ptr + device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	//2つ目を作る
	device->CreateRenderTargetView(swapChainResources[1], &rtvDesc, rtvHandles[1]);
#pragma endregion
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
#pragma region FenceとEvent
	//初期値を０でFenceを作る
	ID3D12Fence* fence = nullptr;
	uint64_t fenceValue = 0;
	hr = device->CreateFence(fenceValue, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence));
	assert(SUCCEEDED(hr));

	//FenceのSignalを持つためのイベントを作成する
	HANDLE fenceEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	assert(fenceEvent != nullptr);
#pragma endregion
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
	D3D12_ROOT_PARAMETER rootParameters[3] = {};

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
		Log(reinterpret_cast<char*>(errorBlob->GetBufferPointer()));
		assert(false);
	}
	//バイナリをもとに生成
	ID3D12RootSignature* rootSignature = nullptr;
	hr = device->CreateRootSignature(0,
		signatureBlob->GetBufferPointer(), signatureBlob->GetBufferSize(),
		IID_PPV_ARGS(&rootSignature));
	assert(SUCCEEDED(hr));
	// 2, InputLayoutの設定
	D3D12_INPUT_ELEMENT_DESC inputElementDescs[2] = {};
	inputElementDescs[0].SemanticName = "POSITION";
	inputElementDescs[0].SemanticIndex = 0;
	inputElementDescs[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	inputElementDescs[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	inputElementDescs[1].SemanticName = "TEXCOORD";
	inputElementDescs[1].SemanticIndex = 0;
	inputElementDescs[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	inputElementDescs[1].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
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
	graphicsPipelineStateDesc.pRootSignature = rootSignature;//RootSignature
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
	ID3D12PipelineState* graphicsPipelineState = nullptr;
	hr = device->CreateGraphicsPipelineState(&graphicsPipelineStateDesc,
		IID_PPV_ARGS(&graphicsPipelineState));
	assert(SUCCEEDED(hr));
#pragma endregion
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
#pragma region Textureを読み込んで転送
	DirectX::ScratchImage mipImages = LoadTexture("resources/uvChecker.png");
	const DirectX::TexMetadata& metaData = mipImages.GetMetadata();
	ID3D12Resource* textureResource = CreateTextureResource(device, metaData);
	UploadTextureData(textureResource, mipImages);
#pragma endregion
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
#pragma region ShaderResourceView(SRV)
	//Textureより下は確定
	//metaDataをもとにSRVの設定
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Format = metaData.format;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;//2Dテクスチャ
	srvDesc.Texture2D.MipLevels = UINT(metaData.mipLevels);
	//SRVを作成するDescriptorHeapの場所を決める
	D3D12_CPU_DESCRIPTOR_HANDLE textureSrvHandleCPU = srvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	D3D12_GPU_DESCRIPTOR_HANDLE textureSrvHandleGPU = srvDescriptorHeap->GetGPUDescriptorHandleForHeapStart();
	//先頭はImGuiが使っているのでその次を使う
	textureSrvHandleCPU.ptr += device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	textureSrvHandleGPU.ptr += device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	//SRVの作成
	device->CreateShaderResourceView(textureResource, &srvDesc, textureSrvHandleCPU);
#pragma endregion
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
#pragma region DepthStencilView(DSV)
	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc{};
	dsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;//Format。基本的にはResorceに合わせる
	dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;//2DTexture
	//DSVHeapの先頭にDSVを作る
	device->CreateDepthStencilView(depthStencilResource, &dsvDesc, dsvDescriptorHeap->GetCPUDescriptorHandleForHeapStart());
#pragma endregion
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
#pragma region Resourceにデータを書き込む
	

	//マテリアルにデータを書き込む
	Vector4* materialData = nullptr;
	//書き込むためのアドレスを取得
	materialResource->Map(0, nullptr, reinterpret_cast<void**>(&materialData));
	//今回は赤
	*materialData = Vector4(1.0f, 0.0f, 0.0f, 1.0f);

#pragma region TransformationMatrix系
	//データを書き込む
	Matrix4x4* wvpData = nullptr;
	//書き込むためのアドレスを取得
	wvpResource->Map(0, nullptr, reinterpret_cast<void**>(&wvpData));
	//単位行列を書き込んでいく
	*wvpData = MakeIdentity4x4();

	//データを書き込み
	Matrix4x4* transformationMatrixDataSprite = nullptr;
	//書きこむためのアドレスを習得
	transformationMatrixResourceSprite->Map(0, nullptr, reinterpret_cast<void**>(&transformationMatrixDataSprite));
	//単位行列を書き込む
	*transformationMatrixDataSprite = MakeIdentity4x4();
#pragma endregion

////////////////////////////////////////////////////////////
#pragma region VertexBufferView
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView = {};
	//リソースの先頭のアドレスから使う
	vertexBufferView.BufferLocation = vertexResource->GetGPUVirtualAddress();
	//使用するリソースのサイズはちゅてん三つ分
	vertexBufferView.SizeInBytes = sizeof(VertexData) * vertexCount;//
	//１個当たりのサイズ
	vertexBufferView.StrideInBytes = sizeof(VertexData);//
#pragma region Sprite
	D3D12_VERTEX_BUFFER_VIEW vertexBufferViewSprite = {};
	//リソースの先頭のアドレスから使う
	vertexBufferViewSprite.BufferLocation = vertexResourceSprite->GetGPUVirtualAddress();
	//使用するリソースのサイズはちゅてん三つ分
	vertexBufferViewSprite.SizeInBytes = sizeof(VertexData) * vertexCount;//
	//１個当たりのサイズ
	vertexBufferViewSprite.StrideInBytes = sizeof(VertexData);//
#pragma endregion
#pragma endregion
////////////////////////////////////////////////////////////
#pragma region VertexData
	//頂点リソースにでーたを書き込む1
	VertexData* vertexData = nullptr;
	//書き込む溜めのアドレスを取得
	vertexResource->Map(0, nullptr, reinterpret_cast<void**>(&vertexData));
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

#pragma region 球の描画

	uint32_t latIndex = 0;
	uint32_t lonIndex = 0;
	//経度分割１つ分の角度φd
	const float kLonEvery = pi * 2.0f / static_cast<float>(kSubdivision);
	//緯度分割1つ分の角度θd
	const float kLatEvery = pi / static_cast<float>(kSubdivision);
	//緯度の方向に分割
	for (latIndex = 0;latIndex < kSubdivision; ++latIndex) {
		float lat = -pi / 2.0f + kLatEvery * latIndex;//θ
		//緯度の方向に分割しながら線を描く
		for (lonIndex = 0;lonIndex < kSubdivision;++lonIndex) {
			uint32_t start = static_cast<uint32_t>(latIndex * kSubdivision + lonIndex) * 6;
			float lon = lonIndex * kLonEvery;//φ
			//頂点にデータを入力する。基準点a
			vertexData[start + 0].position.x = std::cos(lat) * std::cos(lon);
			vertexData[start + 0].position.y = std::sin(lat);
			vertexData[start + 0].position.z = std::cos(lat) * std::sin(lon);
			vertexData[start + 0].position.w = 1.0f;
			vertexData[start + 0].texcoord.x = lonIndex / kSubdivision;
			vertexData[start + 0].texcoord.y = 1.0f - float(latIndex) / float(kSubdivision);
			//基準点b
			vertexData[start + 1].position.x = std::cos(lat + kLatEvery) * std::cos(lon);
			vertexData[start + 1].position.y = std::sin(lat + kLatEvery);
			vertexData[start + 1].position.z = std::cos(lat + kLatEvery) * std::sin(lon);
			vertexData[start + 1].position.w = 1.0f;
			vertexData[start + 1].texcoord.x = lonIndex / kSubdivision;
			vertexData[start + 1].texcoord.y = 1.0f - float(latIndex + 1) / float(kSubdivision);
			//基準点c
			vertexData[start + 2].position.x = std::cos(lat) * std::cos(lon + kLonEvery);
			vertexData[start + 2].position.y = std::sin(lat);
			vertexData[start + 2].position.z = std::cos(lat) * std::sin(lon + kLonEvery);
			vertexData[start + 2].position.w = 1.0f;
			vertexData[start + 2].texcoord.x = (lonIndex + 1) / kSubdivision;
			vertexData[start + 2].texcoord.y = 1.0f - float(latIndex) / float(kSubdivision);
			//基準点d
			vertexData[start + 3].position.x = std::cos(lat + kLatEvery) * std::cos(lon + kLonEvery);
			vertexData[start + 3].position.y = std::sin(lat + kLatEvery);
			vertexData[start + 3].position.z = std::cos(lat + kLatEvery) * std::sin(lon + kLonEvery);
			vertexData[start + 3].position.w = 1.0f;
			vertexData[start + 3].texcoord.x = (lonIndex + 1) / kSubdivision;
			vertexData[start + 3].texcoord.y = 1.0f - float(latIndex + 1) / float(kSubdivision);
			//基準点e
			vertexData[start + 4].position.x = std::cos(lat) * std::cos(lon + kLonEvery);
			vertexData[start + 4].position.y = std::sin(lat);
			vertexData[start + 4].position.z = std::cos(lat) * std::sin(lon + kLonEvery);
			vertexData[start + 4].position.w = 1.0f;
			vertexData[start + 4].texcoord.x = (lonIndex + 1) / kSubdivision;
			vertexData[start + 4].texcoord.y = 1.0f - float(latIndex) / float(kSubdivision);
			//基準点f
			vertexData[start + 5].position.x = std::cos(lat + kLatEvery) * std::cos(lon);
			vertexData[start + 5].position.y = std::sin(lat + kLatEvery);
			vertexData[start + 5].position.z = std::cos(lat + kLatEvery) * std::sin(lon);
			vertexData[start + 5].position.w = 1.0f;
			vertexData[start + 5].texcoord.x = lonIndex / kSubdivision;
			vertexData[start + 5].texcoord.y = 1.0f - float(latIndex + 1) / float(kSubdivision);
		}
	}

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

	//2枚目の三角形
	vertexDataSprite[3].position = { 0.0f, 0.0f, 0.0f, 1.0f };
	vertexDataSprite[3].texcoord = { 0.0f, 0.0f };
	vertexDataSprite[4].position = { 640.0f, 0.0f, 0.0f, 1.0f };
	vertexDataSprite[4].texcoord = { 1.0f, 0.0f };
	vertexDataSprite[5].position = { 640.0f, 360.0f, 0.0f, 1.0f };
	vertexDataSprite[5].texcoord = { 1.0f, 1.0f };
#pragma endregion

#pragma endregion

#pragma endregion
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
#pragma region ViewportとScissor
	//ビューポート
	D3D12_VIEWPORT viewport = {};
	//クライアント領域のサイズと一緒にして画面全体に表示
	viewport.Width = kClientWidth;
	viewport.Height = kClientHeight;
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	//シザー矩形
	D3D12_RECT scissorRect = {};
	//個本的にビューポート同じ矩形が構成されるようにする
	scissorRect.left = 0;
	scissorRect.right = kClientWidth;
	scissorRect.top = 0;
	scissorRect.bottom = kClientHeight;
#pragma endregion
////////////////////////////////////////////////////////////

	//出力ウィンドウへの文字出力
	//OutputDebugStringA("Hello,DirectX!\n");
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui::StyleColorsDark();
	ImGui_ImplWin32_Init(hwnd);
	ImGui_ImplDX12_Init(device,
		2,                                              
		DXGI_FORMAT_R8G8B8A8_UNORM_SRGB,                
		srvDescriptorHeap,
		srvDescriptorHeap->GetCPUDescriptorHandleForHeapStart(),
		srvDescriptorHeap->GetGPUDescriptorHandleForHeapStart()
	);
	
	MSG msg{};
	Transform transform = { {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f} };
	Transform transformSprite{ {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f} };
	Transform camera = { 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, -5.0f };
	//ウィンドウのｘボタンが押されるまでループ
	while (msg.message != WM_QUIT) {
		//Widnowにメッセージが来てたら最優先で処理させる
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) 
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else {
			ImGui_ImplDX12_NewFrame();
			ImGui_ImplWin32_NewFrame();
			ImGui::NewFrame();

////////////////////////////////////////////////////////////
#pragma region コマンドを積み込んで確定させる
	//これから書き込むバックバッファのインデックスを取得
			UINT backBufferIndex = swapChain->GetCurrentBackBufferIndex();
			//トランジションバリア-----------------
			//TransitionBarrierの設定
			D3D12_RESOURCE_BARRIER barrier = {};
			//今回のバリアはTransition
			barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
			//Noneにしておく
			barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
			//バリアを張る対象のリソース。現在のバックバッファに対して行う
			barrier.Transition.pResource = swapChainResources[backBufferIndex];
			//遷移前の（現在）のResourceState
			barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
			//遷移後のResourceState
			barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
			//TransitionBarrierを張る
			commandList->ResourceBarrier(1, &barrier);
			//描画先のRTVを設定する
			commandList->OMSetRenderTargets(1, &rtvHandles[backBufferIndex], false, nullptr);
			//描画先のRTVとDSVを設定する
			D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = dsvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
			commandList->OMSetRenderTargets(1, &rtvHandles[backBufferIndex], false, &dsvHandle);
			//指定した色で画面全体をクリアする
			float clearColor[] = { 0.1f,0.25f,0.5f,1.0f };//RGBAの設定
			commandList->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);//
			commandList->ClearRenderTargetView(rtvHandles[backBufferIndex], clearColor, 0, nullptr);
			
#pragma endregion
////////////////////////////////////////////////////////////

			//ゲームの処理
			transform.rotate.y += 0.003f;
			Matrix4x4 worldMatrix = MakeAffineMatrix(transform.scale, transform.rotate, transform.translate);
			Matrix4x4 cameraMatrix = MakeAffineMatrix(camera.scale, camera.rotate, camera.translate);
			Matrix4x4 viewMatrix = Inverse(cameraMatrix);
			Matrix4x4 projectionMatrix = MakePerspectiveFovMatrix(0.45f, float(1280) / float(720), 0.1f, 100.0f);
			Matrix4x4 worldViewProjectionMatrix = Multiply(worldMatrix, Multiply(viewMatrix, projectionMatrix));
			*wvpData = worldViewProjectionMatrix;
			//*wvpData = worldMatrix;

			Matrix4x4 worldMatrixSprite = MakeAffineMatrix(transformSprite.scale, transformSprite.rotate, transformSprite.translate);
			Matrix4x4 viewMatrixSprite = MakeIdentity4x4();
			Matrix4x4 projectionMatrixSprite = MakeOrthoGraphicMatrix(0.0f, 0.0f, static_cast<float>(kClientWidth), static_cast<float>(kClienHeight), 0.0f, 100.0f);
			Matrix4x4 worldViewProjectionMatrixSprite = Multiply(worldMatrixSprite, Multiply(viewMatrixSprite, projectionMatrixSprite));
			*transformationMatrixDataSprite = worldViewProjectionMatrixSprite;

			////////////////////////////////////////////////////////////
#pragma region ImGui1系
			ImGui::Begin("02");
			//ImGui::ShowDemoWindow();
			ImGui::SliderFloat("TranslateX", &transform.translate.x, -0.5f, 0.5f);
			ImGui::SliderFloat("TranslateY", &transform.translate.y, -0.5f, 0.5f);
			//ImGui::SliderFloat("TranslateZ", &transform.translate.z, -0.5f, 1.0f);
			ImGui::SliderFloat("R", &materialData->x, 0.0f, 1.0f);
			ImGui::SliderFloat("G", &materialData->y, 0.0f, 1.0f);
			ImGui::SliderFloat("B", &materialData->z, 0.0f, 1.0f);
			//ImGui::SliderFloat("A", &materialData->w, 0.0f, 1.0f);
			ImGui::SliderFloat("TransFromSpriteX", &transformSprite.translate.y, -0.5f, 640.0f);
			ImGui::End();
#pragma endregion
			////////////////////////////////////////////////////////////

			//描画
			ImGui::Render();
			ID3D12DescriptorHeap* descriptorHeaps[] = { srvDescriptorHeap };
			commandList->SetDescriptorHeaps(1, descriptorHeaps);
			/////////////////////////////////////////////////////////////////////////
			
			//描画0200
			commandList->RSSetViewports(1, &viewport);
			commandList->RSSetScissorRects(1, &scissorRect);
			//RootSignalの設定
			commandList->SetGraphicsRootSignature(rootSignature);
			commandList->SetPipelineState(graphicsPipelineState);
			commandList->IASetVertexBuffers(0, 1, &vertexBufferView);
			//形状を設定
			commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
			//マテリアルCBufferの場所を設定
			commandList->SetGraphicsRootConstantBufferView(0, materialResource->GetGPUVirtualAddress());
			//wvp用のCBufferの場所を設定
			commandList->SetGraphicsRootConstantBufferView(1, wvpResource->GetGPUVirtualAddress());
			//SRVのDescritorTableの先頭を設定。2はrootParameter[2]である
			commandList->SetGraphicsRootDescriptorTable(2, textureSrvHandleGPU);
			//描画
			commandList->DrawInstanced(vertexCount, 1, 0, 0);
			/////////////////////////////////////////////////////////////////////////

			// Spriteの描画。変更が必要なものだけ変更する
			commandList->IASetVertexBuffers(0, 1, &vertexBufferViewSprite);
			// TransformationMatrixの場所を設定
			commandList->SetGraphicsRootConstantBufferView(1, transformationMatrixResourceSprite->GetGPUVirtualAddress());
			// 描画
			commandList->DrawInstanced(6, 1, 0, 0);

			ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), commandList);

			//画面表示できるようにするために
			//RenderTargetからPresent
			barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
			barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
			//TransitionBarrierを張る
			commandList->ResourceBarrier(1, &barrier);
			//画面表示できるようにするために

			//コマンドリストの内容を確定させる。全てのコマンドを積んでからCloseすること
			hr = commandList->Close();
			assert(SUCCEEDED(hr));

////////////////////////////////////////////////////////////
#pragma region 積んだコマンドをキックする
	//GPUにコマンドリストの実行を行わさせる
			ID3D12CommandList* commandLists[] = { commandList };
			commandQueue->ExecuteCommandLists(1, commandLists);
			//GPUとOSに画面の交換を行うように通知する
			swapChain->Present(1, 0);

			//GPUにシグナルを送る
			fenceValue++;
			//GPUがここまでたどり着いた時に、Fenceの値っを指定した値に代入する様にSignalを送る
			commandQueue->Signal(fence, fenceValue);

			//Fenceの値が指定したSignal値にたどり着いているか確認する
	        //GetCompletedValueの初期値はFence作成時に渡した初期値
			if (fence->GetCompletedValue() < fenceValue) {
				//指定したSingnalにたどりついていないので、たどりつくまで待つようにイベントを設定する
				fence->SetEventOnCompletion(fenceValue, fenceEvent);
				//イベントを待つ
				WaitForSingleObject(fenceEvent, INFINITE);
			}

			//次のフレーム用のコマンドリストを準備
			hr = commandAllocator->Reset();
			assert(SUCCEEDED(hr));
			hr = commandList->Reset(commandAllocator, nullptr);
			assert(SUCCEEDED(hr));
#pragma endregion
////////////////////////////////////////////////////////////
		}
		
	}
	ImGui_ImplDX12_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	//解放処理
	CloseHandle(fenceEvent);
	fence->Release();
	rtvDescriptorHeap->Release();
	dsvDescriptorHeap->Release();
	swapChainResources[0]->Release();
	swapChainResources[1]->Release();
	swapChain->Release();
	commandList->Release();
	commandAllocator->Release();
	commandQueue->Release();
	device->Release();
	useAdapter->Release();
	dxgiFactory->Release();

	materialResource->Release();
	wvpResource->Release();
	transformationMatrixResourceSprite->Release();

	//Pipeline
	vertexResource->Release();
	vertexResourceSprite->Release();

	graphicsPipelineState->Release();
	signatureBlob->Release();
	if (errorBlob) {
		errorBlob->Release();
	}
	rootSignature->Release();
	pixelShaderBlob->Release();
	vertexShaderBlob->Release();

#ifdef _DEBUG
	debugController->Release();
#endif // _DEBUG
	CloseWindow(hwnd);

	//リソースリークチェック==================-↓↓↓
	IDXGIDebug1* debug;
	if (SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(&debug)))) {
		debug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_ALL);
		debug->ReportLiveObjects(DXGI_DEBUG_APP, DXGI_DEBUG_RLO_ALL);
		debug->ReportLiveObjects(DXGI_DEBUG_D3D12, DXGI_DEBUG_RLO_ALL);
		debug->Release();
	}
	//リソースリークチェック==================-↑↑↑
	//COMの初期化を解除
	CoUninitialize();
	return 0;
}
