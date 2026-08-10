#pragma once
#include "ISingleton.h"
#include "Component.h"
#include <functional>
#include <string>
#include <unordered_map>

class ComponentFactory :
	public ISingleton<ComponentFactory>
{
public:
	friend class ISingleton<ComponentFactory>;
public:
	using ComponentCreatorFunc = std::function<std::unique_ptr<Component>()>;

	void Initialize();

	void Register(const std::string& name, ComponentCreatorFunc creator) {
		registry_[name] = creator;
	}

	// 名前（文字列）からコンポーネントを作って返す
	std::unique_ptr<Component> Create(const std::string& name);

	// 登録済みのコンポーネント一覧の取得
	std::vector<std::string> GetRegisteredNames() const {
		std::vector<std::string> names;
		for (const auto& [name, _] : registry_) {
			names.push_back(name);
		}
		return names;
	}
private:
	std::unordered_map<std::string, ComponentCreatorFunc> registry_;
};