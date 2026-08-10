#pragma once
#include "ISingleton.h"

#include <string>
#include <vector>
#include <cstdint>

class CollisionLayerManager : public ISingleton<CollisionLayerManager> {
    friend class ISingleton<CollisionLayerManager>;
private:
    CollisionLayerManager() {
        // 初期レイヤー（最大32個）
        layers_ = { "Default", "Player", "Enemy", "Attack", "StaticMap" };
    }

public:
    // エディタから文字入力で新しいカテゴリを追加
    bool AddLayer(const std::string& name) {
        if (layers_.size() >= 32) return false;
        for (const auto& layer : layers_) {
            if (layer == name) return false; // 重複禁止
        }
        layers_.push_back(name);
        return true;
    }

    // レイヤー名からビット値を取得（例: "Player" -> 0x0002）
    uint32_t GetBitByName(const std::string& name) const {
        for (size_t i = 0; i < layers_.size(); ++i) {
            if (layers_[i] == name) return (1u << i);
        }
        return 0;
    }

    std::string GetNameByBit(uint32_t bit) const {
        if (bit == 0) return "None";
        for (size_t i = 0; i < layers_.size(); ++i) {
            if ((1u << i) == bit) {
                return layers_[i];
            }
        }
        return "Unknown";
    }

    // エディタ（ImGui等）でレイヤー一覧を表示するための参照
    const std::vector<std::string>& GetLayers() const { return layers_; }

private:
    std::vector<std::string> layers_;
};