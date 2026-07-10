#pragma once
#include "Structures.h"
#include <json.hpp>
#include "Log.h"

using json = nlohmann::json;

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Vector2, x, y)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Vector3,x,y,z)

namespace FileSystem {
    //   ================
    // 【 ダイアログ表示 】
    //   ================
    // 【 保存 】
    std::string ShowSaveFileDialogJson();
    // 【 ロード 】
    std::string ShowOpenFileDialogJson();

    template <typename T>
    bool SaveToFile(const std::string& filePath, const T& data) {
        if (filePath.empty()) return false;
        try {
            json root = data; // ライブラリの魔法で自動的にJSON化される

            std::ofstream ofs(filePath);
            if (ofs.fail()) return false;

            ofs << std::setw(4) << root << std::endl; // インデント4で綺麗に保存
            return true;
        }
        catch (const std::exception& e) {
            Log::View(std::string(e.what()));
            return false;
        }
    }

    template <typename T>
    bool LoadFromFile(const std::string& filePath, T& outData) {
        if (filePath.empty()) return false;
        try {
            std::ifstream ifs(filePath);
            if (ifs.fail()) return false;

            nlohmann::json root;
            ifs >> root;

            outData = root.get<T>(); // 指定された型に自動で復元される
            return true;
        }
        catch ([[maybe_unused]]const std::exception& e) {
            return false;
        }
    }

    template <typename T>
    bool SaveWithDialog(const T& data) {
        std::string path = ShowSaveFileDialogJson(); // ダイアログを表示
        if (path.empty()) return false;              // キャンセルされたら終了
        return SaveToFile(path, data);               // そのまま保存
    }

    template <typename T>
    bool LoadWithDialog(T& outData) {
        std::string path = ShowOpenFileDialogJson(); // ダイアログを表示
        if (path.empty()) return false;              // キャンセルされたら終了
        return LoadFromFile(path, outData);          // そのままロード
    }
}