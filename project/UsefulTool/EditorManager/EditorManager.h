#pragma once
#include "../ISingleton.h"
#include "../EventManager/EventManager.h"
#include "IEditor.h"

class EditorManager : public ISingleton<EditorManager> {
public:
    friend class ISingleton<EditorManager>;
private:
    std::vector<std::unique_ptr<IEditor>> editors_; // IEditorで全エディタを管理
    IEditor* activeEditor_ = nullptr;

public:
    void Initialize() {
        // Undo / Redo のイベント登録
        //EventManager::GetInstance()->RegisterAction(EVENTCATEGORY::UI, 1, this, &EditorManager::OnUndoEvent);
        //EventManager::GetInstance()->RegisterAction(EVENTCATEGORY::UI, 2, this, &EditorManager::OnRedoEvent);
    }

    // Factoryパターンの実装（生成する派生エディタの型を推論）
    template <typename EditorType, typename... Args>
    EditorType* CreateEditor(Args&&... args) {
        static_assert(std::is_base_of<IEditor, EditorType>::value, "EditorType must derive from IEditor");

        auto newEditor = std::make_unique<EditorType>(std::forward<Args>(args)...);
        EditorType* editorPtr = newEditor.get();

        editors_.push_back(std::move(newEditor));
        return editorPtr;
    }

    void SetActiveEditor(IEditor* editor) {
        activeEditor_ = editor;
    }

    void UpdateEditors() {
        if (activeEditor_) activeEditor_->Update();

#ifdef USE_IMGUI

        // 【1】 エディタ切り替え用ウィンドウの描画
        ImGui::Begin("Editor Manager");

        // コンボボックス（ドロップダウン）の表示
        // activeEditor_ があればその名前、なければ "None" を表示
        const char* currentActiveName = activeEditor_ ? activeEditor_->GetName() : "None";

        if (ImGui::BeginCombo("Active Editor", currentActiveName)) {

            // 登録されている全てのエディタをループで表示
            for (auto& editor : editors_) {
                // 今リストに描画しようとしているエディタが、現在アクティブなものかどうか
                bool isSelected = (activeEditor_ == editor.get());

                // ImGui::Selectable でリストの項目を描画。クリックされたら true が返る
                if (ImGui::Selectable(editor->GetName(), isSelected)) {
                    // ここでアクティブなエディタを切り替える！
                    activeEditor_ = editor.get();
                }

                // 選択されている項目に初期フォーカスを当てる（ImGuiのお作法）
                if (isSelected) {
                    ImGui::SetItemDefaultFocus();
                }
            }
            ImGui::EndCombo();
        }
        if (ImGui::Button("Undo") || (InputManager::GetKey().PressKey(DIK_LCONTROL) && InputManager::GetKey().PressedKey(DIK_Z))) {
            OnUndoEvent();
            //EventManager::GetInstance()->FireEvent(GAMEEVENTID::UndoClicked);
        }
        ImGui::SameLine();
        if (ImGui::Button("Redo") || (InputManager::GetKey().PressKey(DIK_LCONTROL) && InputManager::GetKey().PressedKey(DIK_Y))) {
            OnRedoEvent();
            //EventManager::GetInstance()->FireEvent(GAMEEVENTID::RedoClicked);
        }
        activeEditor_->DrawUI();

        ImGui::End(); // Tool Manager ウィンドウ終了

        
#endif// USE_IMGUI
    }

private:
    void OnUndoEvent() {
        if (activeEditor_) activeEditor_->Undo();
    }

    void OnRedoEvent() {
        if (activeEditor_) activeEditor_->Redo();
    }
};