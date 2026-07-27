#pragma once
#include "ISingleton.h"

#include <functional>
#include <string>

class DynamicObject;

class Component {
public:
	DynamicObject* master_ = nullptr;

	virtual ~Component() = default;
	virtual void Initialize() {}
	virtual void Update(float deltaTime){}
};

class ComponentFactory :
	public ISingleton<ComponentFactory>
{
public:
	friend class ISingleton<ComponentFactory>;
public:
	using ComponentCreatorFunc = std::function<std::unique_ptr<Component>()>;

	void Register(const std::string& name, ComponentCreatorFunc creator) {
		registry_[name] = creator;
	}

	// 名前（文字列）からコンポーネントを作って返す
	std::unique_ptr<Component> Create(const std::string& name) {
		auto it = registry_.find(name);
		if (it != registry_.end()) {
			return it->second(); // 登録された生成関数を実行！
		}
		return nullptr; // 見つからない場合
	}

private:
	std::unordered_map<std::string, ComponentCreatorFunc> registry_;
};