#include "OmnisTechOracle.h"
#include <cassert>

#include "DXGI.h"
#include "Log.h"

Microsoft::WRL::ComPtr <IDXGIAdapter4> OmnisTechOracle::useAdapter_ = nullptr;

void OmnisTechOracle::Oracle() {
	HRESULT hr;
	//よい順にアダプタを頼む
	for (UINT i = 0;DXGI::dxgiFactory_->EnumAdapterByGpuPreference(i,
		DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(&useAdapter_)) !=
		DXGI_ERROR_NOT_FOUND; ++i) {
		//アダプターの情報を取得する
		DXGI_ADAPTER_DESC3 adapterDesc{};
		hr = useAdapter_->GetDesc3(&adapterDesc);
		assert(SUCCEEDED(hr));//取得できないのは一大事
		//ソフトウェアアダプタでなければ採用！
		if (!(adapterDesc.Flags & DXGI_ADAPTER_FLAG3_SOFTWARE)) {
			//採用したアダプタの情報にログに出力。wstringの方なので注意
			Log::ViewFile(ConvertString(std::format(L"Use Adapter:{}\n", adapterDesc.Description)));
			break;
		}
		useAdapter_ = nullptr;//ソフトウェアアダプタの場合はみなかったことにする
	}
	//適切なアダプタが見つからなかったので起動できない
	assert(useAdapter_ != nullptr);
}