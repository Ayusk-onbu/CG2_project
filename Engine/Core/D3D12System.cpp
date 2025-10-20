#include "D3D12System.h"
#include "Log.h"
#include <cassert>

void D3D12System::SelectLevel(OmnisTechOracle& omnisTechOracle) {
	//機能レベルとログ出力用の文字列
	D3D_FEATURE_LEVEL featureLevels[] = {
		D3D_FEATURE_LEVEL_12_2,D3D_FEATURE_LEVEL_12_1,D3D_FEATURE_LEVEL_12_0
	};
	const char* featureLevelStrings[] = { "12.2","12.1","12.0" };
	HRESULT hr;
	//高い順に生成できるか試していく
	for (size_t i = 0;i < _countof(featureLevels);++i) {
		//採用したアダプターでデバイスを生成
		hr = D3D12CreateDevice(omnisTechOracle.GetAdapter().Get(), featureLevels[i], IID_PPV_ARGS(&device_));
		//指定した機能レベルでデバイスが生成出来たかを確認
		if (SUCCEEDED(hr)) {
			//生成出来たのでログ出力を行ってループを抜ける
			Log::View((std::format("FeatureLevel : {}\n", featureLevelStrings[i])));
			break;
		}
	}
	//デバイスの生成が上手くいかったので起動できない
	assert(device_ != nullptr);
	Log::ViewFile("Complete create D3D12Device!!!\n");//初期化完了のログを出す
	device_->SetName(L"D3D12Device");
}