#pragma once
#include "../ISingleton.h"
#include "Log.h"
#include <map>
#include <functional>
#include <any>

enum EVENTCATEGORY : int {
	EFFECT = 0,
	SE,
	UI,
};

enum GAMEEVENTID : int {
	OnPlayerAttack = 0,
	HairEditor,
	EventBox,
	PlayerTakeDamage,// BoxFilter Random
	PlayerDash,// Radial Blur
	PlayerOMG,// Vignette, Gaussian Blur
	PlayerDie,// Grayscale
};

//struct PairHash{
//	template <class T1, class T2>
//	std::size_t operator () (const std::pair<T1, T2>&p) const {
//		// 2つのハッシュ値を組み合わせて1つのハッシュ値を作る（一般的な手法）
//		auto h1 = std::hash<T1>{}(p.first);
//		auto h2 = std::hash<T2>{}(p.second);
//		return h1 ^ (h2 << 1); // 簡易的なビット演算による結合
//	}
//};

class EventManager : public ISingleton<EventManager>
{
public:
	friend ISingleton<EventManager>;
	using EventArgs = std::vector<std::any>;
	using EventAction = std::function<void(const EventArgs&)>;
	using DirectionTag = std::pair<int, int>;
public:
	void BindEventToTag(GAMEEVENTID eventId, EVENTCATEGORY category, int detailId);
	/// <summary>
	/// 静的関数(メンバ関数以外)を登録するための関数
	/// </summary>
	/// <typeparam name="...FuncArgs"></typeparam>
	/// <param name="category">どのカテゴリか：EVENTCATEGORY</param>
	/// <param name="detailId">そのカテゴリの何か</param>
	/// <param name="func">登録する関数</param>
	template <typename... FuncArgs>
	void RegisterAction(EVENTCATEGORY category, int detailId, void(*func)(FuncArgs...)) {
		DirectionTag tag = std::make_pair(static_cast<int>(category), detailId);


		if (actions_.find(tag) == actions_.end()) {
			// 登録された関数の引数の数に合わせて、コンパイルタイムに 0, 1, 2... とインデックスを作る
			actions_[tag].push_back([func](const EventArgs& args) {
				CallHelper(func, args, std::index_sequence_for<FuncArgs...>{});
			});
		}
		else {
			// すでに同じタグのアクションが登録されている場合、警告
			Log::View("Warning: Action already registered for category " + std::to_string(static_cast<int>(category)) + " and detailId " + std::to_string(detailId));
			return;
		}
		
	}
	/// <summary>
	/// メンバ関数を登録するための関数
	/// </summary>
	/// <typeparam name="T"></typeparam>
	/// <typeparam name="...FuncArgs"></typeparam>
	/// <param name="category">どのカテゴリか：EVENTCATEGORY</param>
	/// <param name="detailId">そのカテゴリの何か</param>
	/// <param name="instance">使用するメンバ関数の持ち主</param>
	/// <param name="func">使用するメンバ関数</param>
	template <typename T, typename... FuncArgs>
	void RegisterAction(EVENTCATEGORY category, int detailId, T* instance, void(T::* func)(FuncArgs...)) {
		DirectionTag tag = std::make_pair(static_cast<int>(category), detailId);

		if (actions_.find(tag) == actions_.end()) {
			actions_[tag].push_back([instance, func](const EventArgs& args) {
				CallHelper(instance, func, args, std::index_sequence_for<FuncArgs...>{});
			});
		}
		else {
			// すでに同じタグのアクションが登録されている場合、警告
			Log::View("Warning: Action already registered for category " + std::to_string(static_cast<int>(category)) + " and detailId " + std::to_string(detailId));
			return;
		}
	}
	/// <summary>
	/// イベントを発火
	/// </summary>
	/// <typeparam name="...Args"></typeparam>
	/// <param name="eventId">発火するイベントID</param>
	/// <param name="...args">引数があれば登録する</param>
	template <typename... Args>
	void FireEvent(GAMEEVENTID eventId,Args... args)
	{
		// 結合テーブルから、このイベントに紐づく演出タグのリストを探す
		auto it = bindings_.find(eventId);
		if (it == bindings_.end()) return; // 何も結合されていなければスルー

		EventArgs eventArgs = { std::any(args)... };

		// 紐づいている演出タグをループで全て実行
		for (const auto& tag : it->second){
			auto actionIt = actions_.find(tag);
			if (actionIt != actions_.end())
			{
				// 登録されている関数（Action）を実行！
				for (auto& action : actionIt->second) {
					action(eventArgs);
				}
			}
		}
	}
private:
	//std::unordered_map<DirectionTag, EventAction, PairHash> events_;
	std::map<DirectionTag, std::vector<EventAction>> actions_;
	std::unordered_map<GAMEEVENTID, std::vector<DirectionTag>> bindings_;

private:
	// 【裏方のヘルパー1】普通の関数用：argsから型を復元して関数に流し込む
	template <typename F, typename... FuncArgs, size_t... I>
	static void CallHelper(F func, const EventArgs& args, std::index_sequence<I...>) {
		func(std::any_cast<FuncArgs>(args.at(I))...);
	}

	// 【裏方のヘルパー2】メンバ関数用：インスタンスのポインタ経由でメソッドを実行
	template <typename T, typename F, typename... FuncArgs, size_t... I>
	static void CallHelper(T* instance, F func, const EventArgs& args, std::index_sequence<I...>) {
		(instance->*func)(std::any_cast<FuncArgs>(args.at(I))...);
	}
};