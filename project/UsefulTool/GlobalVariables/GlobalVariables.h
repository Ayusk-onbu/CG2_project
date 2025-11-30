#pragma once
#include <memory>
#include <variant>
#include <iostream>
#include <map>
#include <json.hpp>
#include "Structures.h"

/// <summary>
/// グローバル変数
/// </summary>
class GlobalVariables
{
public:
	static GlobalVariables* GetInstance() {
		if (instance_ == nullptr) {
			instance_ = std::make_unique<GlobalVariables>();
		}
		return instance_.get();
	}
public:
	GlobalVariables() = default;
	~GlobalVariables() = default;
	GlobalVariables(const GlobalVariables&) = delete;
	GlobalVariables& operator=(const GlobalVariables& x) = delete;
private:static std::unique_ptr<GlobalVariables>instance_;
public:
	// [ Struct ]

	struct Item {
		// 項目の値
		std::variant<int32_t, float, Vector3, Vector4, std::string> value;
	};
	struct Group {
		// グループ
		std::map<std::string, Item>items;
	};
public:
	// [ Get ]
	template<typename T>
	T GetValue(const std::string& groupName, const std::string& key) const {
		// 指定グループが存在するかどうかを確認する
		std::map<std::string, Group>::const_iterator itGroup = datas_.find(groupName);
		assert(itGroup != datas_.end());
		// 存在していたら指定したキーは存在するのか確認する
		const Group& group = datas_.at(groupName);
		std::map<std::string, Item>::const_iterator itItem = group.items.find(key);
		assert(itItem != group.items.end());
		const Item& item = itItem->second;

		return std::get<T>(item.value);
	}
public:
	/// <summary>
	/// 更新処理
	/// </summary>
	void Update();

	/// <summary>
	/// グループの作成
	/// </summary>
	/// <param name="groupName">グループ名</param>
	void CreateGroup(const std::string& groupName);

	// [ 追加 ]
	template<typename T>
	void SetValue(const std::string& groupName, const std::string& key, const T& value) {
		// グループの参照を取得
		Group& group = datas_[groupName];
		// 新しい項目のデータを設定
		Item newItem{};
		newItem.value = value;
		// 設定した項目をstd::mapに追加
		group.items[key] = newItem;
	}

	// [ すでに追加されていなければ追加 ]
	template<typename T>
	void AddItem(const std::string& groupName, const std::string& key, const T& value) {
		// グループの参照を取得
		Group& group = datas_[groupName];

		std::map<std::string, Item>::iterator itItem = group.items.find(key);

		if (itItem == group.items.end()) {
			SetValue(groupName, key, value);
		}
	}

	/// <summary>
	/// ファイルに書き出し
	/// </summary>
	/// <param name="groupName">グループ</param>
	void SaveFile(const std::string& groupName);

	/// <summary>
	/// ディレクトリの全ファイルを読み込み
	/// </summary>
	void LoadFiles();

	void LoadFile(const std::string& groupName);
private:
	// 全データ
	std::map<std::string, Group>datas_;
	// ファイルパス(セーブするときに選べるほうが楽やろこんなの)
	const std::string kDirectoryPath = "resources/GlobalVariables/";
};

