#include "TestScene.h"
#include "SceneDirector.h"
#include "ImGuiManager.h"
#include "CameraSystem.h"
#include "UIHEditor.h"


// ================================
// Override Functions
// ================================

TestScene::~TestScene() {
	
}

void TestScene::Initialize() {

	container_.Initialize(p_fngine_);
	

	// 初期化処理

	player_ = std::make_unique<Player3D>();
	player_->Initialize(p_fngine_);

	particle_ = std::make_unique<Particle>(p_fngine_);
	particle_->Initialize(1000,"water");

	ground_ = std::make_unique<ConvenienceModel>();
	ground_->Initialize(p_fngine_, "ground", "GridLine");
}

void TestScene::Update() {

	player_->Update();

	particle_->Update();

	if (InputManager::IsJump()) {
		hasRequestedNextScene_ = true;
	}

	if (hasRequestedNextScene_) {
		p_sceneDirector_->RequestChangeScene(new TitleScene());
	}
}

void TestScene::Draw() {
	
	ground_->Draw();
	particle_->Draw();
	player_->Draw();
}

void TestScene::PauseUpdate() {
	UIEditor::GetInstance()->SetTargetElement(&container_);
	UIEditor::GetInstance()->SetActiveAnimation(&animation_);
	UIEditor::GetInstance()->Update();

	container_.UpdateAnimation(1.0f / 60.0f);
}

void TestScene::PauseDraw() {
	container_.DrawImGui();

    NodeImGui();

	//Draw();
	container_.Draw();
}

void TestScene::NodeImGui() {
    if (ImGui::IsKeyPressed(ImGuiKey_P)) {
        nodes_.push_back(Spline::Node<Vector3>({ 0.0f,0.0f,0.0f }));
    }

    ImGui::Begin("Action Editor (Hermite Spline)");

    // --- 1. キャンバスの準備 ---
    ImDrawList* draw_list = ImGui::GetWindowDrawList();

    ImVec2 canvas_p0 = ImGui::GetCursorScreenPos();
    ImVec2 canvas_sz = ImGui::GetContentRegionAvail();
    if (canvas_sz.x < 50.0f) canvas_sz.x = 50.0f;
    if (canvas_sz.y < 50.0f) canvas_sz.y = 50.0f;
    ImVec2 canvas_p1 = ImVec2(canvas_p0.x + canvas_sz.x, canvas_p0.y + canvas_sz.y);

    draw_list->AddRectFilled(canvas_p0, canvas_p1, IM_COL32(50, 50, 50, 255));
    draw_list->AddRect(canvas_p0, canvas_p1, IM_COL32(255, 255, 255, 255));

    ImGui::InvisibleButton("canvas", canvas_sz);
    ImVec2 mouse_pos_in_canvas = ImVec2(ImGui::GetIO().MousePos.x - canvas_p0.x, ImGui::GetIO().MousePos.y - canvas_p0.y);
    bool is_active = ImGui::IsItemActive();
    bool is_hovered = ImGui::IsItemHovered();

    // --- 2. スプライン曲線の描画 ---
    if (nodes_.size() >= 2) {
        const int num_segments = 100;
        // キャンバス中心を使ってノード描画と同じ基準にする
        ImVec2 canvas_center = ImVec2(canvas_p0.x + canvas_sz.x * 0.5f, canvas_p0.y + canvas_sz.y * 0.5f);

        Vector3 prev_point = nodes_[0].position;
        for (int i = 1; i <= num_segments; ++i) {
            float t = (float)i / (float)num_segments;
            Vector3 current_point = MathUtils::Spline::GetPointSpline(nodes_, t);

            // 中心基準でスクリーン座標に変換
            ImVec2 p1 = ImVec2(canvas_center.x + prev_point.x, canvas_center.y + prev_point.y);
            ImVec2 p2 = ImVec2(canvas_center.x + current_point.x, canvas_center.y + current_point.y);

            draw_list->AddLine(p1, p2, IM_COL32(255, 200, 0, 255), 2.0f);
            prev_point = current_point;
        }
    }

    // --- 3. ノードとハンドルの操作＆描画 ---
    const float NODE_RADIUS = 6.0f;
    const float HANDLE_RADIUS = 4.0f;

    for (int i = 0; i < (int)nodes_.size(); ++i) {
        auto& node = nodes_[i];

        ImVec2 canvas_center = ImVec2(canvas_p0.x + canvas_sz.x * 0.5f, canvas_p0.y + canvas_sz.y * 0.5f);
        ImVec2 pos_screen = ImVec2(canvas_center.x + node.position.x, canvas_center.y + node.position.y);
        ImVec2 in_screen = ImVec2(canvas_center.x + node.TangentIn.x, canvas_center.y + node.TangentIn.y);
        ImVec2 out_screen = ImVec2(canvas_center.x + node.TangentOut.x, canvas_center.y + node.TangentOut.y);

        draw_list->AddLine(pos_screen, in_screen, IM_COL32(150, 150, 150, 200), 1.0f);
        draw_list->AddLine(pos_screen, out_screen, IM_COL32(150, 150, 150, 200), 1.0f);

        // --- クリック判定（左/右/中 を全てチェック） ---
        if (is_hovered && ImGui::IsMouseClicked(0)) {
            auto dist = [](ImVec2 a, ImVec2 b) { return (a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y); };
            ImVec2 mouse_p = ImGui::GetIO().MousePos;
            if (dist(mouse_p, pos_screen) < NODE_RADIUS * NODE_RADIUS * 4.0f) {
                draggedNodeIndex = i; draggedHandleType = 0; // left on node
            }
        }
        else if (is_hovered && ImGui::IsMouseClicked(1)) {
            auto dist = [](ImVec2 a, ImVec2 b) { return (a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y); };
            ImVec2 mouse_p = ImGui::GetIO().MousePos;
            if (dist(mouse_p, in_screen) < HANDLE_RADIUS * HANDLE_RADIUS * 4.0f) {
                draggedNodeIndex = i; draggedHandleType = 1; // right -> in
            }
        }
        else if (is_hovered && ImGui::IsMouseClicked(2)) {
            auto dist = [](ImVec2 a, ImVec2 b) { return (a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y); };
            ImVec2 mouse_p = ImGui::GetIO().MousePos;
            if (dist(mouse_p, out_screen) < HANDLE_RADIUS * HANDLE_RADIUS * 4.0f) {
                draggedNodeIndex = i; draggedHandleType = 2; // middle -> out
            }
        }

        draw_list->AddCircleFilled(in_screen, HANDLE_RADIUS, IM_COL32(100, 200, 100, 255));
        draw_list->AddCircleFilled(out_screen, HANDLE_RADIUS, IM_COL32(200, 100, 100, 255));
        draw_list->AddCircleFilled(pos_screen, NODE_RADIUS, IM_COL32(255, 255, 255, 255));
    }

    // --- 4. ドラッグ中の座標更新 ---
    if (draggedNodeIndex >= 0 && draggedHandleType >= 0 && ImGui::IsMouseDragging(draggedHandleType)) {
        auto& node = nodes_[draggedNodeIndex];
        ImVec2 delta = ImGui::GetIO().MouseDelta;

        bool isAltDown = ImGui::IsKeyDown(ImGuiKey_LeftAlt);
        node.isBroken = isAltDown;

        if (draggedHandleType == 0) {
            node.position.x += delta.x; node.position.y += delta.y;
            node.TangentIn.x += delta.x; node.TangentIn.y += delta.y;
            node.TangentOut.x += delta.x; node.TangentOut.y += delta.y;
        }
        else if (draggedHandleType == 1) {
            node.TangentIn.x += delta.x; node.TangentIn.y += delta.y;
            if (node.isBroken) {
                node.TangentOut.x = node.position.x + (node.position.x - node.TangentIn.x);
                node.TangentOut.y = node.position.y + (node.position.y - node.TangentIn.y);
            }
        }
        else if (draggedHandleType == 2) {
            node.TangentOut.x += delta.x; node.TangentOut.y += delta.y;
            if (node.isBroken) {
                node.TangentIn.x = node.position.x + (node.position.x - node.TangentOut.x);
                node.TangentIn.y = node.position.y + (node.position.y - node.TangentOut.y);
            }
        }
    }

    // ドラッグ解除（押していたボタンが離れたら解除）
    if (draggedNodeIndex >= 0) {
        if (!ImGui::IsMouseDown(draggedHandleType)) {
            draggedNodeIndex = -1;
            draggedHandleType = -1;
        }
    }
    int index = 0;
    if (ImGui::TreeNodeEx("Positions")) {
        // 参照渡しで実際のノードを編集する
        for (int idx = 0; idx < (int)nodes_.size(); ++idx) {
            if (ImGui::TreeNodeEx(std::to_string(index).c_str())) {
                auto& node = nodes_[idx];
                ImGui::DragFloat3((std::string("node") + std::to_string(index)).c_str(), &node.position.x);
                ImGui::DragFloat3((std::string("in") + std::to_string(index)).c_str(), &node.TangentIn.x);
                ImGui::DragFloat3((std::string("out") + std::to_string(index)).c_str(), &node.TangentOut.x);
                if (ImGui::Button("Delete")) {
					nodes_.erase(nodes_.begin() + idx);
                }
                index++;
				ImGui::TreePop();
            }
        }
        ImGui::TreePop();
    }

    ImGui::End();
}