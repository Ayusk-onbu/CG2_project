#include "GlobalVariables.h"
#include "ImGuiManager.h"
#include <fstream>
#include <iostream>

using json = nlohmann::json;

std::unique_ptr<GlobalVariables> GlobalVariables::instance_ = nullptr;

void GlobalVariables::CreateGroup(const std::string& groupName) {
	datas_[groupName];
}

void GlobalVariables::SaveFile(const std::string& groupName) {
	// グループを検索
	std::map<std::string, Group>::iterator itGroup = datas_.find(groupName);

	// 未登録チェック
	assert(itGroup != datas_.end());

	json root;

	// 実質 std::map<std::sting,Item>
	root = json::object();

	// jsonオブジェクト登録
	// 実質 std::map<std::string,Group>
	root[groupName] = json::object();

	// 各項目について
	for (std::map<std::string, Item>::iterator itItem = itGroup->second.items.begin();itItem != itGroup->second.items.end();++itItem) {
		// 項目名を取得
		const std::string& itemName = itItem->first;
		// 項目の参照を取得
		Item& item = itItem->second;

		if (std::holds_alternative<int32_t>(item.value)) {
			root[groupName][itemName] = std::get<int32_t>(item.value);
		}
		else if (std::holds_alternative<float>(item.value)) {
			root[groupName][itemName] = std::get<float>(item.value);
		}
		else if (std::holds_alternative<Vector3>(item.value)) {
			Vector3 value = std::get<Vector3>(item.value);
			root[groupName][itemName] = json::array({ value.x,value.y,value.z });
		}
		else if (std::holds_alternative<Vector4>(item.value)) {
			Vector4 value = std::get<Vector4>(item.value);
			root[groupName][itemName] = json::array({ value.x,value.y,value.z,value.w });
		}
		else if (std::holds_alternative<std::string>(item.value)) {
			std::string value = std::get<std::string>(item.value);
			root[groupName][itemName] = std::get<std::string>(item.value);;
		}
	}

	// ディレクトリが無ければ作成する
	std::filesystem::path dir(kDirectoryPath);
	if (!std::filesystem::exists(dir)) {
		std::filesystem::create_directory(dir);
	}

	// 書き込むJSONファイルのフルパスを合成する
	std::string filePath = kDirectoryPath + groupName + ".json";
	// 書き込み用ファイルストリーム
	std::ofstream ofs;
	// ファイルを書き込み用に開く
	ofs.open(filePath);

	// ファイルオープン失敗
	if (ofs.fail()) {
		std::string message = "Failed open data file for write.";
		MessageBoxA(nullptr, message.c_str(), "GlovalVariables", 0);
		assert(0);
		return;
	}

	// ファイルにjson文字列を書き込む(インデント幅4)
	ofs << std::setw(4) << root << std::endl;
	// ファイルを閉じる
	ofs.close();
}

void GlobalVariables::LoadFile(const std::string& groupName) {
	// 読み込みJSONファイルのフルパスを合成する
	std::string filePath = kDirectoryPath + groupName + ".json";
	// 読みこみ用ファイルストリーム
	std::ifstream ifs;
	// ファイルを読みこみように開く
	ifs.open(filePath);

	// ファイルオープン失敗
	if (ifs.fail()) {
		std::string message = "Failed open data file for load.";
		MessageBoxA(nullptr, message.c_str(), "GlovalVariables", 0);
		assert(0);
		return;
	}

	json root;

	// json文字列からjsonのデータ構造に展開
	ifs >> root;
	// ファイルを閉じる
	ifs.close();

	// グループ検索
	json::iterator itGroup = root.find(groupName);

	// 未登録チェック
	assert(itGroup != root.end());

	// 各アイテムについて
	for (json::iterator itItem = itGroup->begin();itItem != itGroup->end();++itItem) {
		// アイテム名を取得
		const std::string& itemName = itItem.key();

		// int32_t型の値を保持していれば
		if (itItem->is_number_integer()) {
			// int型の値を登録
			int32_t value = itItem->get<int32_t>();
			SetValue(groupName, itemName, value);
		}
		else if (itItem->is_number_float()) {
			// float型の値を登録
			double value = itItem->get<double>();
			SetValue(groupName, itemName, static_cast<float>(value));
		}
		else if (itItem->is_array() && itItem->size() == 3) {
			// float型のjson配列登録
			Vector3 value = { itItem->at(0),itItem->at(1),itItem->at(2) };
			SetValue(groupName, itemName, value);
		}
		else if (itItem->is_array() && itItem->size() == 4) {
			// float型のjson配列登録
			Vector4 value = { itItem->at(0),itItem->at(1),itItem->at(2),itItem->at(3)};
			SetValue(groupName, itemName, value);
		}
		else if (itItem->is_string()) {
			// float型のjson配列登録
			std::string value = itItem->get<std::string>();
			SetValue(groupName, itemName, value);
		}
	}
}

void GlobalVariables::LoadFiles() {
	std::filesystem::path dir(kDirectoryPath);
	// ディレクトパスが無ければスキップする
	if (!std::filesystem::exists(dir)) {
		return;
	}
	std::filesystem::directory_iterator dir_it(dir/*保存先ディレクトリ*/);
	for (const std::filesystem::directory_entry& entry : dir_it) {
		// ファイルパスを取得
		const std::filesystem::path& filePath = entry.path();

		// ファイル拡張子を取得
		std::string extension = filePath.extension().string();
		// .jsonファイル以外はスキップ
		if (extension.compare(".json") != 0) {
			continue;
		}

		// ファイル読み込み
		LoadFile(filePath.stem().string());
	}
}

void GlobalVariables::Update() {
#ifdef USE_IMGUI
	if (!ImGui::Begin("Gloval Variables", nullptr, ImGuiWindowFlags_MenuBar)) {

		ImGui::End();
		return;
	}
	if (!ImGui::BeginMenuBar()) return;

	// 各グループについて
	for (std::map<std::string, Group>::iterator itGroup = datas_.begin();itGroup != datas_.end(); itGroup++) {
		// グループ名を取得
		const std::string& groupName = itGroup->first;
		// グループの参照を取得
		Group& group = itGroup->second;

		if (!ImGui::BeginMenu(groupName.c_str()))
			continue;

		// 各項目について
		for (std::map<std::string, Item>::iterator itItem = group.items.begin();itItem != group.items.end();++itItem) {
			// 項目名を取得
			const std::string& itemName = itItem->first;
			// 項目の参照を取得
			Item& item = itItem->second;

			if (std::holds_alternative<int32_t>(item.value)) {
				int32_t* ptr = std::get_if<int32_t>(&item.value);
				ImGui::SliderInt(itemName.c_str(), ptr, 0, 100);
			}
			else if (std::holds_alternative<float>(item.value)) {
				float* ptr = std::get_if<float>(&item.value);
				ImGui::SliderFloat(itemName.c_str(), ptr, 0.0f, 100.0f);
			}
			else if (std::holds_alternative<Vector3>(item.value)) {
				Vector3* ptr = std::get_if<Vector3>(&item.value);
				ImGui::SliderFloat3(itemName.c_str(), &ptr->x, 0, 100);
			}
			else if (std::holds_alternative<Vector4>(item.value)) {
				Vector4* ptr = std::get_if<Vector4>(&item.value);
				ImGui::SliderFloat4(itemName.c_str(), &ptr->x, 0, 100);
			}

		}
		if (ImGui::Button("Save")) {
			SaveFile(groupName);
			std::string message = std::format("{}.json saved.", groupName);
			MessageBoxA(nullptr, message.c_str(), "GlobalVariables", 0);
		}
		ImGui::EndMenu();
	}
	ImGui::EndMenuBar();
	ImGui::End();
#endif// USE_IMGUI
}