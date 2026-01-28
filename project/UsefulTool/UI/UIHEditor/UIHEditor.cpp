#include "UIHEditor.h"
#include <algorithm>

std::unique_ptr<UIEditor> UIEditor::instance_ = nullptr;
const char* UIEditor::easingNames[] = {
    "None", "InSine", "OutSine", "InOutSine", "InQuad", "OutQuad", "InOutQuad",
    "InCubic", "OutCubic", "InOutCubic", "InQuart", "OutQuart", "InOutQuart",
    "InQuint", "OutQuint", "InOutQuint", "InExpo", "OutExpo", "InOutExpo",
    "InCirc", "OutCirc", "InOutCirc", "InBack", "OutBack", "InOutBack",
    "InElastic", "OutElastic", "InOutElastic", "InBounce", "OutBounce", "InOutBounce"
};


void UIEditor::Update() {
#ifdef USE_IMGUI
    if (!targetElement_ || !activeAnimation_) return;
    // 再生ロジック
    if (isPlaying_) {
        // 前フレームからの経過時間を加算
        seekTime_ += ImGui::GetIO().DeltaTime * playbackSpeed_;

        float duration = activeAnimation_->GetDuration();
        if (seekTime_ > duration) {
            seekTime_ = 0.0f; // ループ再生
        }

        // 再生中は毎フレームUIにポーズを適用
        activeAnimation_->Apply(targetElement_, seekTime_);
    }

    ImGui::Begin("UI Editor", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
    // 選択中のUI名
    ImGui::Text("Editing: %s", targetElement_->name_.c_str());
    
    // 選択中のアニメーション名
    ImGui::Text("Animation: %s", activeAnimation_->name.c_str());
    if (ImGui::Button("All Animation Load")) {
        UIHAnimationManager::GetInstance()->Load();
    }
    DrawAnimationEditor();

    ImGui::End();
#endif//USE_IMGUI
}

void UIEditor::DrawAnimationEditor() {
#ifdef USE_IMGUI
    if (ImGui::TreeNode("Animation")) {
        ImGui::Text(activeAnimation_->name.c_str());
        // アニメーション取得
        auto allAnims = UIHAnimationManager::GetInstance()->GetAnimations();
        // コンボボックスで編集対象を選択
        std::string previewName = activeAnimation_ ? activeAnimation_->name : "Select Animation...";
        if (ImGui::BeginCombo("Editing Target", previewName.c_str())) {
            for (auto& [name, anim] : allAnims) {
                if (ImGui::Selectable(name.c_str(), activeAnimation_ == &anim)) {
                    // ⑤[ 値を更新 ]->⑥[ キーフレーム登録 ]
                    activeAnimation_->name = anim.name; // 編集対象を更新
					activeAnimation_->posTrack_ = anim.posTrack_;
					activeAnimation_->scaleTrack_ = anim.scaleTrack_;
					activeAnimation_->rotateTrack_ = anim.rotateTrack_;
                    activeAnimation_->colorTrack_ = anim.colorTrack_;
					inputAnimationName_ = anim.name; // 入力欄も更新
                }
            }
            ImGui::EndCombo();
        }

        // タイムライン
        float maxT = (std::max)(2.0f, activeAnimation_->GetDuration() + 0.5f);
        if (ImGui::SliderFloat("Seek Bar", &seekTime_, 0.0f, maxT)) {
            // スライダーを動かしたら即座にUIへ反映 (プレビュー)
            activeAnimation_->Apply(targetElement_, seekTime_);
        }
        ImGui::Separator();

        // イージングの選択
        ImGui::Combo("Easing", &selectedEase_, easingNames, IM_ARRAYSIZE(easingNames));

        //⑥[ キーフレーム登録 ]->キーフレーム済み->⑦[ Apply ]

		// 各トラックのキーフレームリスト表示
        if (ImGui::TreeNode("Keyframe List")) {
            // ここに各 Track の時間を ImGui::Text で出すだけでもデバッグが捗る
            DrawTrackInfo("Position", activeAnimation_->posTrack_);
            DrawTrackInfo("Scale", activeAnimation_->scaleTrack_);
            DrawTrackInfo("Rotate", activeAnimation_->rotateTrack_);
            DrawTrackInfo("Color", activeAnimation_->colorTrack_);
            
            
            ImGui::TreePop();
        }

        DrawTimelineTrack("Pos", activeAnimation_->posTrack_, maxT);
        DrawTimelineTrack("Scale", activeAnimation_->scaleTrack_, maxT);
        DrawTimelineTrack("Rotate", activeAnimation_->rotateTrack_, maxT);
        DrawTimelineTrack("Color", activeAnimation_->colorTrack_, maxT);

        // キーフレームを打ち込む
        if (ImGui::TreeNode("Keys")) {
            if (ImGui::Button("Positoin")) {
                activeAnimation_->SetPositionKey(seekTime_, targetElement_->GetObj().worldTransform_.transform_.translation_, static_cast<EASINGTYPE>(selectedEase_));
            }
            ImGui::SameLine();
            if (ImGui::Button("Scale")) {
                activeAnimation_->SetScaleKey(seekTime_, targetElement_->GetObj().worldTransform_.transform_.scale_, (EASINGTYPE)selectedEase_);
            }
            ImGui::SameLine();
            if (ImGui::Button("Rotate")) {
                activeAnimation_->SetRotateKey(seekTime_, targetElement_->GetObj().worldTransform_.transform_.rotation_, (EASINGTYPE)selectedEase_);
            }
            ImGui::SameLine();
            if (ImGui::Button("Color")) {
                activeAnimation_->SetColorKey(seekTime_, targetElement_->GetObj().materialData_->color, (EASINGTYPE)selectedEase_);
            }

            ImGui::TreePop();
        }
        ImGui::Separator();

        // 再生について
        if (ImGui::Button(isPlaying_ ? "Stop" : "Play")) {
            isPlaying_ = !isPlaying_;
        }
        ImGui::SameLine();
        if (ImGui::Button("Reset")) {
            seekTime_ = 0.0f;
            activeAnimation_->Apply(targetElement_, seekTime_);
        }

        // 保存
        char buf[128];
        strncpy_s(buf, sizeof(buf), inputAnimationName_.c_str(), _TRUNCATE);
        buf[sizeof(buf) - 1] = 0; // 確実にNULL終端
        if (ImGui::InputText("New Animation Name", buf, sizeof(buf))) {
            // 入力が変更されたらstd::stringを更新
            inputAnimationName_ = buf;
        }

        if (ImGui::Button("Apply Animation name")) {
			activeAnimation_->name = inputAnimationName_;
        }

        if (ImGui::Button("Save")) {
            activeAnimation_->Save();
        }

        ImGui::TreePop();
    }
#endif//USE_IMGUI
}

template<typename T>
void UIEditor::DrawTrackInfo(const char* label, UIHAnimationTrack<T>& track) {
#ifdef USE_IMGUI
    if (ImGui::TreeNode(label)) {
        // トラック内の全キーフレームをループ
        auto& keys = track.GetKeyframes(); // UIHAnimationTrackにGetterを追加しておく

        for (int i = 0; i < keys.size(); ++i) {
            ImGui::PushID(i);

            // キーフレームの時間と、その時間にジャンプするボタン
            char timeBuf[32];
            sprintf_s(timeBuf, "Time: %.2f", keys[i].time);
            if (ImGui::Button(timeBuf)) {
                seekTime_ = keys[i].time; // その時間にシーク
                activeAnimation_->Apply(targetElement_, seekTime_);
            }

            ImGui::SameLine();
            ImGui::Text("| Ease: %d", (int)keys[i].easeType);

            ImGui::SameLine();
            if (ImGui::Button("Delete")) {
                track.RemoveKeyframe(keys[i].time); // 時間指定で削除
            }

            ImGui::PopID();
        }
        ImGui::TreePop();
    }
#endif//USE_IMGUI
}

template<typename T>
void UIEditor::DrawTimelineTrack(const char* label, UIHAnimationTrack<T>& track, float maxTime) {
#ifdef USE_IMGUI
    ImGui::Text("%s", label);

    // タイムラインの背景描画（横棒）
    ImVec2 canvas_pos = ImGui::GetCursorScreenPos();
    ImVec2 canvas_size = ImVec2(ImGui::GetContentRegionAvail().x, 20.0f);
    ImDrawList* draw_list = ImGui::GetWindowDrawList();

    draw_list->AddRectFilled(canvas_pos, ImVec2(canvas_pos.x + canvas_size.x, canvas_pos.y + canvas_size.y), IM_COL32(50, 50, 50, 255));

    // キーフレーム（◆）の描画
    auto& keys = track.GetKeyframes();
    for (auto& k : keys) {
        float x_ratio = k.time / maxTime;
        float x_pos = canvas_pos.x + (x_ratio * canvas_size.x);

        // キーフレームをダイヤモンド型で描画
        draw_list->AddPolyline(std::vector<ImVec2>{
            {x_pos, canvas_pos.y + 5}, { x_pos + 5, canvas_pos.y + 10 },
            { x_pos, canvas_pos.y + 15 }, { x_pos - 5, canvas_pos.y + 10 }
        }.data(), 4, IM_COL32(255, 255, 0, 255), ImDrawFlags_Closed, 2.0f);
    }

    ImGui::Dummy(canvas_size); // 描画領域を確保
#endif//USE_IMGUI
}

