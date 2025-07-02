#include "MapChip.h"
#include <map>
#include <fstream>
#include <sstream>
#include <cassert>

namespace {

	std::map<std::string, MapChipType> mapChipTable = {
		{"0", MapChipType::kBlank},
		{"1", MapChipType::kBlock},
	};

}

void MapChip::ResetMapChipData() {
	// 2次元ベクターの初期化
	mapChipData_.data.clear();
	mapChipData_.data.resize(kNumBlockVirtical);
	for (std::vector<MapChipType>& mapChipDataLine : mapChipData_.data) {
		mapChipDataLine.resize(kNumBlockHorizontal);
	}
}

void MapChip::LoadMapChipCsv(const std::string& filePath) {
	// マップチップデータをリセット
	ResetMapChipData();

	//ファイルを開く
	std::ifstream file;
	file.open(filePath);
	assert(file.is_open());

	//マップチップCsv
	std::stringstream mapChipCsv;
	//ファイルの内容を文字列ストリームにコピー
	mapChipCsv << file.rdbuf();
	// ファイルを閉じる
	file.close();

	//CSVからマップチップデータを読み込む
	for (uint32_t i = 0; i < kNumBlockVirtical; ++i) {
		std::string line;
		getline(mapChipCsv, line);

		//1行分の文字列ストリーム変換して解析しやすくなる
		std::istringstream line_stream(line);

		for (uint32_t j = 0; j < kNumBlockHorizontal; ++j) {
			std::string word;
			getline(line_stream, word, ',');

			if (mapChipTable.contains(word)) {
				mapChipData_.data[i][j] = mapChipTable[word];
			}

		}

	}
}

MapChipType MapChip::GetMapChipTypeByIndex(uint32_t xIndex, uint32_t yIndex) {
	if (xIndex < 0 || kNumBlockHorizontal - 1 < xIndex) {
		return MapChipType::kBlank;
	}
	if (yIndex < 0 || kNumBlockVirtical - 1 < yIndex) {
		return MapChipType::kBlank;
	}
	return mapChipData_.data[yIndex][xIndex];
}

Vector3 MapChip::GetBlockPositionByIndex(uint32_t xIndex, uint32_t yIndex) {
	return Vector3(float(kBlockWidth * xIndex), float(kBlockHeight * (kNumBlockVirtical - 1 - yIndex)), 0);
}

MapChip::IndexSet MapChip::GetMapChipIndexSetByPosition(const Vector3& position) {
	IndexSet indexSet = {};
	indexSet.xIndex = static_cast<uint32_t>((position.x + kBlockWidth / 2.0f) / kBlockWidth);
	indexSet.yIndex = static_cast<uint32_t> (kNumBlockVirtical - 1 - static_cast<uint32_t>((position.y + kBlockHeight / 2.0f) / kBlockHeight));
	return indexSet;
}

MapChip::Rect MapChip::GetRectByIndex(uint32_t xIndex, uint32_t yIndex) {
	// 指定ブロックの中心座標を取得する
	Vector3 center = MapChip::GetBlockPositionByIndex(xIndex, yIndex);

	MapChip::Rect rect;
	rect.left = center.x - kBlockWidth / 2.0f;
	rect.right = center.x + kBlockWidth / 2.0f;
	rect.bottom = center.y - kBlockHeight / 2.0f;
	rect.top = center.y + kBlockHeight / 2.0f;

	return rect;
}