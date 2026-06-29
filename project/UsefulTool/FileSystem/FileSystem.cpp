#include "FileSystem.h"
#include "Windows.h"

namespace FileSystem {
    std::string ShowSaveFileDialogJson() {
        // 文字を入力する器
        char filename[MAX_PATH] = "";
        // ダイアログの設定
        OPENFILENAMEA ofn;
        ZeroMemory(&ofn, sizeof(ofn));

        ofn.lStructSize = sizeof(ofn);
        ofn.hwndOwner = NULL;
        // フィルターを JSON に指定
        ofn.lpstrFilter = "JSON Files (*.json)\0*.json\0All Files (*.*)\0*.*\0";
        ofn.lpstrFile = filename;
        ofn.nMaxFile = MAX_PATH;
        ofn.Flags = OFN_EXPLORER | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT;
        ofn.lpstrDefExt = "json";

        if (GetSaveFileNameA(&ofn)) {
            return std::string(filename);
        }
        return "";
    }

    std::string ShowOpenFileDialogJson() {
        char filename[MAX_PATH] = "";
        OPENFILENAMEA ofn;
        ZeroMemory(&ofn, sizeof(ofn));

        ofn.lStructSize = sizeof(ofn);
        ofn.hwndOwner = NULL;
        // フィルターを JSON に指定
        ofn.lpstrFilter = "JSON Files (*.json)\0*.json\0All Files (*.*)\0*.*\0";
        ofn.lpstrFile = filename;
        ofn.nMaxFile = MAX_PATH;
        // 存在するファイルのみ選択可能にするフラグを追加
        ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
        ofn.lpstrDefExt = "json";

        if (GetOpenFileNameA(&ofn)) {
            return std::string(filename);
        }
        return "";
    }
}