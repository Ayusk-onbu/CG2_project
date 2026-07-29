#pragma once
#include "ModelObject.h"
#include "StatusComponent.h"
#include "ComponentFactory.h"

class DynamicObject
{
public:
	DynamicObject() = default;
	virtual ~DynamicObject() = default;

///////////////////////////
/// 
/// 基本的な存在
///
//////////////////////////
public:
	/// <summary>
	/// 基本的な初期化処理
	/// </summary>
	/// <param name="engine"></param>
	/// <param name="modelName">使うModelの名前 (例)mesh.objならmesh</param>
	/// <param name="textureName">使うTextureの名前(例)GridLine.pngならGridLine</param>
	virtual void Initialize(Fngine* engine, std::string modelName = "cube", std::string textureName = "GridLine");
	
	/// <summary>
	/// ステータス、等の更新処理
	/// </summary>
	/// <param name="deltaTime"></param>
	virtual void Update(float deltaTime);
	
	/// <summary>
	/// 描画関数
	/// </summary>
	virtual void Draw();
	
	WorldTransform& GetTransform(){ return transform_; }

	void SetPosition(const Vector3& pos) { transform_.set_.Translation(pos); }
	void SetRotation(const Vector3& rot) { transform_.set_.Rotation(rot); }
	void SetScale(const Vector3& scale) { transform_.set_.Scale(scale); }

	void SetName(const std::string& name) { name_ = name; }
	std::string& GetName() { return name_; }
protected:
	WorldTransform transform_;

	std::string name_;

	int type_;// 識別子

///////////////////////////
/// 
/// コンポーネント関係
///
//////////////////////////
protected:
	std::vector<std::unique_ptr<Component>> components_;

public:
	// コンポーネントの追加
	template<typename T>
	T* AddComponent() {
		auto comp = std::make_unique<T>();
		comp->master_ = this; // 自分を親としてセット
		T* ptr = comp.get();
		components_.push_back(std::move(comp));
		return ptr;
	}

	Component* AddComponent(const std::string& componentName) {
		// ファクトリーを使って名前から生成
		auto comp = ComponentFactory::GetInstance()->Create(componentName);
		if (!comp) return nullptr;

		comp->master_ = this;
		Component* ptr = comp.get();
		components_.push_back(std::move(comp));
		return ptr;
	}

	// 型 T のコンポーネントを探してポインタを返すテンプレート関数
	template <typename T>
	T* GetComponent() {
		for (auto& comp : components_) {
			// dynamic_cast でキャストを試み、成功すればその型のポインタを返す
			if (auto* ptr = dynamic_cast<T*>(comp.get())) {
				return ptr;
			}
		}
		return nullptr; // 見つからなかったら nullptr
	}

	void OnCollision(DynamicObject* other, const Vector3& pushOut);

/////////////////////////////
///// 
///// ステータス関係
/////
////////////////////////////
//public:
//	StatusComponent& GetStatus() { return status_; }
//
//protected:
//	StatusComponent status_;
//
};

