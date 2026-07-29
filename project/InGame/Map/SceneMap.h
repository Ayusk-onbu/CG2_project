#pragma once
#include "DynamicObject.h"

#include <vector>
#include <memory>

class SceneMap {
public:
    SceneMap() = default;
    ~SceneMap() = default;

    // オブジェクトの追加・削除
    DynamicObject* SpawnObject(const std::string& name = "New Object") {
        auto newObj = std::make_unique<DynamicObject>();
        DynamicObject* ptr = newObj.get();
        
        ptr->SetName(name);

        objects_.push_back(std::move(newObj));
        return ptr;
    }

    void RemoveObject(DynamicObject* target) {
        std::erase_if(objects_, [target](const auto& obj) {
            return obj.get() == target;
        });
    }

    // 毎フレームの更新・描画
    void Update(float deltaTime) {
        for (auto& obj : objects_) {
            obj->Update(deltaTime);
        }
    }

    // エディタへ生ポインタのリストを渡す用[cite: 7]
    std::vector<DynamicObject*> GetRawObjects() {
        std::vector<DynamicObject*> rawList;
        for (auto& obj : objects_) {
            rawList.push_back(obj.get());
        }
        return rawList;
    }

private:
    std::vector<std::unique_ptr<DynamicObject>> objects_;
};