#include "MapChip.h"
#include <map>
#include <fstream>
#include <sstream>
#include <cassert>
#include "ImGuiManager.h"

void Block::Initialize(ModelObject* model,Transform transform, Vector3 pos) {
	model_ = model;

	transform_ = transform;
	transform_.translate = pos;

	uvTransform_ = {
		{1.0f,1.0f,1.0f},
		{0.0f,0.0f,0.0f},
		{0.0f,0.0f,0.0f},
	};

	
}

void Block::UpDate() {
}

void Block::Draw(Camera& camera, TheOrderCommand& command, PSO& pso, DirectionLight& light, Texture& tex) {
	//ゲームの処理
	Matrix4x4 worldMatrix = Matrix4x4::Make::Affine(transform_.scale, transform_.rotate, transform_.translate);
	Matrix4x4 uvTransformMatrix = Matrix4x4::Make::Scale(uvTransform_.scale);
	uvTransformMatrix = Matrix4x4::Multiply(uvTransformMatrix, Matrix4x4::Make::RotateZ(uvTransform_.rotate.z));
	uvTransformMatrix = Matrix4x4::Multiply(uvTransformMatrix, Matrix4x4::Make::Translate(uvTransform_.translate));
	model_->SetWVPData(camera.DrawCamera(worldMatrix), worldMatrix, uvTransformMatrix);
	model_->Draw(command, pso, light, tex);
}

void Block::Draw(DebugCamera& camera, TheOrderCommand& command, PSO& pso, DirectionLight& light, Texture& tex) {
	//ゲームの処理
	Matrix4x4 worldMatrix = Matrix4x4::Make::Affine(transform_.scale, transform_.rotate, transform_.translate);
	Matrix4x4 uvTransformMatrix = Matrix4x4::Make::Scale(uvTransform_.scale);
	uvTransformMatrix = Matrix4x4::Multiply(uvTransformMatrix, Matrix4x4::Make::RotateZ(uvTransform_.rotate.z));
	uvTransformMatrix = Matrix4x4::Multiply(uvTransformMatrix, Matrix4x4::Make::Translate(uvTransform_.translate));
	model_->SetWVPData(camera.DrawCamera(worldMatrix), worldMatrix, uvTransformMatrix);
	model_->Draw(command, pso, light, tex);
}


namespace {

	std::map<std::string, MapChipType> mapChipTable = {
		{"0", MapChipType::kBlank},
		{"1", MapChipType::kBlock},
	};

}

void MapChipField::ResetMapChipData() {
	// 2次元ベクターの初期化
	mapChipData_.data.clear();
	mapChipData_.data.resize(kNumBlockVirtical);
	for (std::vector<MapChipType>& mapChipDataLine : mapChipData_.data) {
		mapChipDataLine.resize(kNumBlockHorizontal);
	}
}

void MapChipField::LoadMapChipCsv(const std::string& filePath) {
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

MapChipType MapChipField::GetMapChipTypeByIndex(uint32_t xIndex, uint32_t yIndex) {
	if (xIndex < 0 || kNumBlockHorizontal - 1 < xIndex) {
		return MapChipType::kBlank;
	}
	if (yIndex < 0 || kNumBlockVirtical - 1 < yIndex) {
		return MapChipType::kBlank;
	}
	return mapChipData_.data[yIndex][xIndex];
}

Vector3 MapChipField::GetBlockPositionByIndex(uint32_t xIndex, uint32_t yIndex) {
	return Vector3(float(kBlockWidth * float(xIndex)), float(kBlockHeight * (kNumBlockVirtical - 1.0f - yIndex)), 0.0f);
}

MapChipField::IndexSet MapChipField::GetMapChipIndexSetByPosition(const Vector3& position) {
	IndexSet indexSet = {};
	indexSet.xIndex = static_cast<uint32_t>((position.x + kBlockWidth / 2.0f) / kBlockWidth);
	indexSet.yIndex = static_cast<uint32_t> (kNumBlockVirtical - 1 - static_cast<uint32_t>((position.y + kBlockHeight / 2.0f) / kBlockHeight));
	return indexSet;
}

MapChipField::Rect MapChipField::GetRectByIndex(uint32_t xIndex, uint32_t yIndex) {
	// 指定ブロックの中心座標を取得する
	Vector3 center = MapChipField::GetBlockPositionByIndex(xIndex, yIndex);

	MapChipField::Rect rect;
	rect.left = center.x - kBlockWidth / 2.0f;
	rect.right = center.x + kBlockWidth / 2.0f;
	rect.bottom = center.y - kBlockHeight / 2.0f;
	rect.top = center.y + kBlockHeight / 2.0f;

	return rect;
}