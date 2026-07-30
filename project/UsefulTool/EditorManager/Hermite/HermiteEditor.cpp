#include "HermiteEditor.h"
#include "ImGuiManager.h"
#include "DrawManager.h"
#include "MathUtils.h"

// ToDo
// 軌道だけ見える機能が欲しい

void HermiteEditor::Update() {
#ifdef USE_IMGUI
    auto& nodes_ = targetObjects_;

    if (ImGui::IsMouseClicked(0) && !ImGui::GetIO().WantCaptureMouse) {

        // マウスの画面座標を取得
        ImVec2 mousePos = ImGui::GetMousePos();

        // 画面サイズ・カメラの逆行列・カメラ位置を取得
        float windowWidth = 1280.0f;  // 実際の画面幅
        float windowHeight = 720.0f; // 実際の画面高
        Matrix4x4 invProjView = Matrix4x4::Inverse(CameraSystem::GetInstance()->GetActiveCamera()->GetViewProjectionMatrix());
        Vector3 camPos = CameraSystem::GetInstance()->GetActiveCamera()->GetTranslation();

        // ③ マウス座標から3D空間のレイを生成！
        Ray ray = MathUtils::CalculateRayFromScreen(
            mousePos.x, mousePos.y,
            windowWidth, windowHeight,
            invProjView, camPos
        ); //

        // 判定用のワーク変数
        int closestNode = -1;
        int closestElement = 0;     // 0: Position, 1: TangentIn, 2: TangentOut
        float minDistance = FLT_MAX; // 一番近い距離を記録する用
        float hitDist = 0.0f;

        // 点のクリック判定の大きさ（ギズモの見かけの大きさに合わせて調整してください）
        float clickRadius = 0.5f;

        // 2. 全てのノードに対して球判定を行う
        for (int i = 0; i < (int)nodes_.size(); ++i) {
            auto* node = nodes_[i]; // 前回のコードを元にポインタ配列と仮定
            if (!node) continue;

            // --- 通過点 (Position) の判定 ---
            if (MathUtils::IntersectRaySphere(ray, node->position, clickRadius, &hitDist)) { //
                if (hitDist < minDistance) {
                    minDistance = hitDist;
                    closestNode = i;
                    closestElement = 0; // Position
                }
            }

            // ---ハンドル (Tangent) の判定 ---
            if (i == selectedNodeIndex_) {
                // TangentIn の判定
                if (MathUtils::IntersectRaySphere(ray, node->tangentIn, clickRadius, &hitDist)) { //
                    if (hitDist < minDistance) {
                        minDistance = hitDist;
                        closestNode = i;
                        closestElement = 1; // TangentIn
                    }
                }
                // TangentOut の判定
                if (MathUtils::IntersectRaySphere(ray, node->tangentOut, clickRadius, &hitDist)) { //
                    if (hitDist < minDistance) {
                        minDistance = hitDist;
                        closestNode = i;
                        closestElement = 2; // TangentOut
                    }
                }
            }
        }

        // 3. 最終結果をエディタのメンバ変数に反映
        if (closestNode != -1) {
            selectedNodeIndex_ = closestNode; //
            selectedElement_ = closestElement; //
        }
        else {
            // 何もない空間をクリックしたら選択を解除したい場合はここを有効に
            selectedNodeIndex_ = -1;
        }
    }

    // 描画用の処理
    for (const auto& obj : targetObjects_) {
        PrimitiveSphereData data;
        data.worldTransform.Initialize();
        data.worldTransform.set_.Translation(obj->position);
        data.worldTransform.set_.Scale(scale_);
        data.worldTransform.LocalToWorld();
        data.color = { 1.0f,1.0f,1.0f,1.0f };
        DrawManager::GetInstance()->GetSphere()->AddInstance(data);
        data.worldTransform.set_.Translation(obj->tangentIn);
        data.worldTransform.LocalToWorld();
        data.color = { 1.0f,0.0f,0.0f,1.0f };
        DrawManager::GetInstance()->GetSphere()->AddInstance(data);
        data.worldTransform.set_.Translation(obj->tangentOut);
        data.worldTransform.LocalToWorld();
        data.color = { 0.0f,1.0f,0.0f,1.0f };
        DrawManager::GetInstance()->GetSphere()->AddInstance(data);

        DrawManager::GetInstance()->GetLine()->AddInstance({ obj->position, obj->tangentIn, { 1.0f,0.0f,0.0f,1.0f } });
        DrawManager::GetInstance()->GetLine()->AddInstance({ obj->position, obj->tangentOut, { 0.0f,1.0f,0.0f,1.0f } });
   }

    std::vector<MathUtils::Spline::Node<Vector3>> rawNodes;
    for (auto* ptr : nodes_) {
        if (ptr) rawNodes.push_back(*ptr);
    }

    if (rawNodes.size() >= 2) {
        // --- 曲線の軌跡を描画 ---
        const int splitCount = 50; // 曲線の滑らかさ（分割数）

        // t = 0.0 の最初の点を取得
        Vector3 prevPoint = MathUtils::Spline::GetPointSpline(rawNodes, 0.0f);

        for (int i = 1; i <= splitCount; ++i) {
            float t = (float)i / splitCount;
            Vector3 currentPoint = MathUtils::Spline::GetPointSpline(rawNodes, t);

            PrimitiveLineData lineData;
            lineData.startPoint = prevPoint;
            lineData.endPoint = currentPoint;
            lineData.color = { 1.0f, 1.0f, 1.0f, 1.0f }; // 曲線の色は白
            
            DrawManager::GetInstance()->GetLine()->AddInstance(lineData);
            prevPoint = currentPoint;
        }
    }
#endif
}

void HermiteEditor::DrawUI() {
#ifdef USE_IMGUI
    // targetObjects_ のあだ名（参照）を作る
    // これの型は std::vector<SplineNode*>& になります
    auto& nodes_ = targetObjects_;

    ImGui::DragFloat3("Scale", &scale_.x,0.01f,0.01f);

    // ---------------------------------------------------
    // 全体操作：ノードの追加ボタン
    // ---------------------------------------------------
    if (ImGui::Button("Add Node 追加")) {
        SplineNode newNode({ 0.0f, 0.0f, 0.0f });

        // もしすでにノードがあるなら、最後のノードのちょっと右に生やす
        if (!nodes_.empty()) {
            // nodes_.back() はポインタなので、-> でアクセス
            newNode.position = nodes_.back()->position;
            newNode.position.x += 2.0f;
            newNode.tangentIn = newNode.position;
            newNode.tangentOut = newNode.position;
        }

        // BaseEditor の ExecuteCommand をそのまま呼ぶ！
        // 引数には、本物の配列のアドレスである「&targetObjects_」を渡す
        ExecuteCommand(std::make_unique<SplineNodeAddCommand>(&targetObjects_, newNode));
    }

    ImGui::Separator();

    // =================================================================
    // 【セーブ処理】
    // =================================================================
    if (ImGui::Button("Save Spline...")) {
        std::vector<MathUtils::Spline::Node<Vector3>> saveContainer;
        for (auto* nodePtr : nodes_) {
            if (nodePtr) saveContainer.push_back(*nodePtr);
        }

        // 最初からある関数で、先にjsonオブジェクトに変換しちゃう！
        json splineJson = MathUtils::Spline::SerializeNodes(saveContainer);

        FileSystem::SaveWithDialog(splineJson);
    }

    ImGui::SameLine();

    // =================================================================
    //  【ロード処理】
    // =================================================================
    if (ImGui::Button("Load Spline...")) {
        json loadedJson;

        // まずはjson型としてファイルからそのまま読み込む
        if (FileSystem::LoadWithDialog(loadedJson)) {

            // 最初からある関数で、jsonからノード配列を一発復元！
            auto loadedContainer = MathUtils::Spline::DeserializeNodes<Vector3>(loadedJson);

                // --- あとは古いポインタを消してnewし直すだけ ---
            for (auto* nodePtr : nodes_) { delete nodePtr; }
            nodes_.clear();

            for (const auto& nodeData : loadedContainer) {
                nodes_.push_back(new MathUtils::Spline::Node<Vector3>(nodeData));
            }
            selectedNodeIndex_ = -1;
        }
    }

    // 2. 連動フラグ (isBroken) の編集
    ImGui::Checkbox("Break Handles", &isBroken_);

    // ---------------------------------------------------
    // 各ノードのループ処理
    // ---------------------------------------------------
    for (int i = 0; i < nodes_.size(); ++i) {
        ImGui::PushID(i);

        // 削除ボタン
        if (ImGui::Button("X")) {
            ExecuteCommand(std::make_unique<SplineNodeDeleteCommand>(&targetObjects_, i));
            ImGui::PopID();
            break; // 配列のサイズが変わったので即ループを抜ける
        }
        ImGui::SameLine();

        if (ImGui::TreeNode(("Node " + std::to_string(i)).c_str())) {
            if (ImGui::RadioButton("Edit with Gizmo", selectedNodeIndex_ == i)) {
                selectedNodeIndex_ = i;
                selectedElement_ = 0; // デフォルトはPositionを選択
            }
            if (selectedNodeIndex_ == i) {
                ImGui::SameLine(); ImGui::RadioButton("Pos", &selectedElement_, 0);
                ImGui::SameLine(); ImGui::RadioButton("In", &selectedElement_, 1);
                ImGui::SameLine(); ImGui::RadioButton("Out", &selectedElement_, 2);
            }
            ImGui::Separator();

            // nodes_[i] はポインタなので、* をつけて「実体」を丸ごとバックアップ
            SplineNode oldState = *nodes_[i];
            bool isEdited = false;

            Vector3 oldPos = nodes_[i]->position;
            if (ImGui::DragFloat3("Position", &nodes_[i]->position.x, 0.1f)) {
                Vector3 delta = {
                    nodes_[i]->position.x - oldPos.x,
                    nodes_[i]->position.y - oldPos.y,
                    nodes_[i]->position.z - oldPos.z
                };
                nodes_[i]->tangentIn.x += delta.x;  nodes_[i]->tangentIn.y += delta.y;  nodes_[i]->tangentIn.z += delta.z;
                nodes_[i]->tangentOut.x += delta.x; nodes_[i]->tangentOut.y += delta.y; nodes_[i]->tangentOut.z += delta.z;
            }
            if (ImGui::IsItemDeactivatedAfterEdit()) { isEdited = true; }

            // 3. 入力ハンドル (tangent In) の編集
            if (ImGui::DragFloat3("tangent In", &nodes_[i]->tangentIn.x, 0.1f)) {
                if (!isBroken_) {
                    nodes_[i]->tangentOut.x = nodes_[i]->position.x + (nodes_[i]->position.x - nodes_[i]->tangentIn.x);
                    nodes_[i]->tangentOut.y = nodes_[i]->position.y + (nodes_[i]->position.y - nodes_[i]->tangentIn.y);
                    nodes_[i]->tangentOut.z = nodes_[i]->position.z + (nodes_[i]->position.z - nodes_[i]->tangentIn.z);
                }
            }
            if (ImGui::IsItemDeactivatedAfterEdit()) { isEdited = true; }

            // 4. 出力ハンドル (tangent Out) の編集
            if (ImGui::DragFloat3("tangent Out", &nodes_[i]->tangentOut.x, 0.1f)) {
                if (!isBroken_) {
                    nodes_[i]->tangentIn.x = nodes_[i]->position.x + (nodes_[i]->position.x - nodes_[i]->tangentOut.x);
                    nodes_[i]->tangentIn.y = nodes_[i]->position.y + (nodes_[i]->position.y - nodes_[i]->tangentOut.y);
                    nodes_[i]->tangentIn.z = nodes_[i]->position.z + (nodes_[i]->position.z - nodes_[i]->tangentOut.z);
                }
            }
            if (ImGui::IsItemDeactivatedAfterEdit()) { isEdited = true; }

            // 確定処理
            if (isEdited) {
                SplineNode newState = *nodes_[i]; // 編集後の実体をコピー
                *nodes_[i] = oldState;            // 一旦 ImGui の自動書き換えを過去に戻す

                // コマンドを発行して履歴付きで適用
                ExecuteCommand(std::make_unique<HermiteEditorCommand>(&targetObjects_, i, oldState, newState));
            }

            ImGui::TreePop();
        }
        ImGui::PopID();
    }

    // ノードが選択されていなければ何もしない
    if (selectedNodeIndex_ < 0 || selectedNodeIndex_ >= nodes_.size()) return;

    float viewMatrix[16];
    float projectionMatrix[16];
    auto camera = CameraSystem::GetInstance()->GetActiveCamera();
    // ビュー行列の詰め替え
    auto viewMat = camera->GetViewMatrix();
    for (int row = 0; row < 4; ++row) {
        for (int col = 0; col < 4; ++col) {
            viewMatrix[row * 4 + col] = viewMat.m[row][col];
        }
    }

    // 2. プロジェクション行列の詰め替え
    auto projMat = camera->GetProjectionMatrix();
    for (int row = 0; row < 4; ++row) {
        for (int col = 0; col < 4; ++col) {
            projectionMatrix[row * 4 + col] = projMat.m[row][col];
        }
    }

    // 現在選択されている座標へのポインタを決める
    Vector3* targetTargetPos = nullptr;
    if (selectedElement_ == 0) targetTargetPos = &nodes_[selectedNodeIndex_]->position;
    if (selectedElement_ == 1) targetTargetPos = &nodes_[selectedNodeIndex_]->tangentIn;
    if (selectedElement_ == 2) targetTargetPos = &nodes_[selectedNodeIndex_]->tangentOut;

    if (targetTargetPos) {
        // 1. 今の座標からギズモ用の変換行列（Matrix）を作る
        float gizmoMatrix[16] = {
            1, 0, 0, 0,
            0, 1, 0, 0,
            0, 0, 1, 0,
            targetTargetPos->x, targetTargetPos->y, targetTargetPos->z, 1
        };

        ImGuizmo::BeginFrame();
        // 画面全体にギズモを表示（ビューポートのサイズに合わせて調整してください）
        ImGuiIO& io = ImGui::GetIO();
        ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);

        // ギズモを画面に描画・操作可能にする
        ImGuizmo::Manipulate(viewMatrix, projectionMatrix, ImGuizmo::TRANSLATE, ImGuizmo::WORLD, gizmoMatrix);

        // 2. ギズモが「今まさにグリグリ動かされている最中」の処理
        if (ImGuizmo::IsUsing()) {
            // 操作が始まった最初の1フレーム目なら、元の状態をバックアップ！
            if (!isGizmoUsingLastFrame_) {
                gizmoOldState_ = *nodes_[selectedNodeIndex_];
            }

            // ギズモの行列から、移動後の新しい座標を引っこ抜く
            Vector3 newPos = { gizmoMatrix[12], gizmoMatrix[13], gizmoMatrix[14] };

            // 数値スライダーの時と「全く同じ連動ロジック」を適用する
            if (selectedElement_ == 0) { // Positionが動いた場合
                Vector3 delta = { newPos.x - targetTargetPos->x, newPos.y - targetTargetPos->y, newPos.z - targetTargetPos->z };
                nodes_[selectedNodeIndex_]->position = newPos;
                nodes_[selectedNodeIndex_]->tangentIn.x += delta.x;  nodes_[selectedNodeIndex_]->tangentIn.y += delta.y;  nodes_[selectedNodeIndex_]->tangentIn.z += delta.z;
                nodes_[selectedNodeIndex_]->tangentOut.x += delta.x; nodes_[selectedNodeIndex_]->tangentOut.y += delta.y; nodes_[selectedNodeIndex_]->tangentOut.z += delta.z;
            }
            else if (selectedElement_ == 1) { // tangent Inが動いた場合
                nodes_[selectedNodeIndex_]->tangentIn = newPos;
                if (!isBroken_) {
                    nodes_[selectedNodeIndex_]->tangentOut.x = nodes_[selectedNodeIndex_]->position.x + (nodes_[selectedNodeIndex_]->position.x - nodes_[selectedNodeIndex_]->tangentIn.x);
                    nodes_[selectedNodeIndex_]->tangentOut.y = nodes_[selectedNodeIndex_]->position.y + (nodes_[selectedNodeIndex_]->position.y - nodes_[selectedNodeIndex_]->tangentIn.y);
                    nodes_[selectedNodeIndex_]->tangentOut.z = nodes_[selectedNodeIndex_]->position.z + (nodes_[selectedNodeIndex_]->position.z - nodes_[selectedNodeIndex_]->tangentIn.z);
                }
            }
            else if (selectedElement_ == 2) { // tangent Outが動いた場合
                nodes_[selectedNodeIndex_]->tangentOut = newPos;
                if (!isBroken_) {
                    nodes_[selectedNodeIndex_]->tangentIn.x = nodes_[selectedNodeIndex_]->position.x + (nodes_[selectedNodeIndex_]->position.x - nodes_[selectedNodeIndex_]->tangentOut.x);
                    nodes_[selectedNodeIndex_]->tangentIn.y = nodes_[selectedNodeIndex_]->position.y + (nodes_[selectedNodeIndex_]->position.y - nodes_[selectedNodeIndex_]->tangentOut.y);
                    nodes_[selectedNodeIndex_]->tangentIn.z = nodes_[selectedNodeIndex_]->position.z + (nodes_[selectedNodeIndex_]->position.z - nodes_[selectedNodeIndex_]->tangentOut.z);
                }
            }

            isGizmoUsingLastFrame_ = true;
        }
        else {
            // 3. 「さっきまで触っていたのに、今フレームで手を離した瞬間」＝確定！！
            if (isGizmoUsingLastFrame_) {
                SplineNode gizmoNewState = *nodes_[selectedNodeIndex_]; // 変化後の最終状態
                *nodes_[selectedNodeIndex_] = gizmoOldState_;          // 一旦、触る前の状態に戻す

                // すでに作ってある「HermiteEditorCommand」をそのまま再利用して歴史に刻む！
                ExecuteCommand(std::make_unique<HermiteEditorCommand>(&targetObjects_, selectedNodeIndex_, gizmoOldState_, gizmoNewState));

                isGizmoUsingLastFrame_ = false; // フラグをリセット
            }
        }
    }
#endif
}