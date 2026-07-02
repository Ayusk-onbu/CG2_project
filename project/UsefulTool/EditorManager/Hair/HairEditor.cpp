#include "HairEditor.h"
#include "FileSystem.h"
#include "MathUtils.h"
#include "DrawManager.h"

void GenerateDefaultSphereHair(GuideCurve::ControllerPoint* data, uint32_t totalCount,float headRadius,
    float segmentLength,Vector3 headCenter, Vector3 rootColor,
    Vector3 tipColor) {
    const int POINTS_PER_GUIDE = 16; // Test.jsonの構造に合わせる
    int totalGuides = totalCount / POINTS_PER_GUIDE;

    // --- スケール調整用のパラメータ（環境に合わせて調整してください） ---
    //float headRadius = 0.3f;                   // 頭部（球体）の半径
   // Vector3 headCenter = { 0.0f, -0.01f, 0.0f }; // 頭部の中心座標
    //float segmentLength = 0.04f;               // 制御点と点の間の長さ（15間隔で約0.6fの長さに）

    for (int g = 0; g < totalGuides; ++g) {

        // 1. フィボナッチ球面により、球の上半分～側面に1000本の根元を完全に均等配置する
        // yの範囲を 1.0（真上） から -0.3（うなじ・耳の下あたり）まで綺麗に分散させる
        float t_geo = (float)g / (float)(totalGuides - 1);
        float y_local = 1.0f - t_geo * 1.3f;

        // 高さに応じた円の半径を計算
        float radiusAtY = std::sqrt((std::max)(0.0f, 1.0f - y_local * y_local));

        // 黄金角（約137.5度 = 2.39996322ラジアン）ずつ回転させて密を作る
        float goldenAngle = 2.39996322f;
        float theta = g * goldenAngle;

        float x_local = std::cos(theta) * radiusAtY;
        float z_local = std::sin(theta) * radiusAtY;

        // 根元の法線方向（頭の中心から外側へ広がるベクトル）
        Vector3 normal = { x_local, y_local, z_local };
        float n_len = std::sqrt(normal.x * normal.x + normal.y * normal.y + normal.z * normal.z);
        if (n_len > 0.0f) { normal.x /= n_len; normal.y /= n_len; normal.z /= n_len; }

        // 根元（Point 0）の座標を決定
        Vector3 rootPos = {
            headCenter.x + normal.x * headRadius,
            headCenter.y + normal.y * headRadius,
            headCenter.z + normal.z * headRadius
        };

        Vector3 currentPos = rootPos;

        // 2. 1本のガイドに紐づく16個の制御点の形状を計算
        for (int p = 0; p < POINTS_PER_GUIDE; ++p) {
            int targetIndex = (g * POINTS_PER_GUIDE) + p;

            // 根元から毛先への進捗度（0.0 ～ 1.0）
            float progress = (float)p / (float)(POINTS_PER_GUIDE - 1);

            if (p == 0) {
                data[targetIndex].position = rootPos;
            }
            else {
                // 【超重要】毛先に向かうにつれて、法線方向（外向き）から徐々に「真下（重力）」へ流れるベクトルにブレンドする
                Vector3 dir;
                float curveInfluence = progress * progress; // 後半（毛先）ほど急に垂れ下がるカーブ

                dir.x = normal.x * (1.0f - curveInfluence * 0.6f);
                dir.y = normal.y * (1.0f - progress) - curveInfluence * 1.3f; // 下方向への引力を強く
                dir.z = normal.z * (1.0f - curveInfluence * 0.6f);

                // ベクトルの正規化
                float d_len = std::sqrt(dir.x * dir.x + dir.y * dir.y + dir.z * dir.z);
                if (d_len > 0.0f) { dir.x /= d_len; dir.y /= d_len; dir.z /= d_len; }

                // 前のポイントから計算した方向へ一定距離進める（骨組みの維持）
                currentPos.x += dir.x * segmentLength;
                currentPos.y += dir.y * segmentLength;
                currentPos.z += dir.z * segmentLength;

                data[targetIndex].position = currentPos;
            }

            // 3. 面倒な周辺パラメータも最高の設定で自動セット！
            data[targetIndex].homePosition = data[targetIndex].position;

            // 太さ：根元は太く（0.004f）、毛先にかけて徐々に細くテーパリング
            data[targetIndex].radius = 0.0035f * (1.0f - progress * 0.8f) + 0.0005f;
            data[targetIndex].nextToLength = segmentLength;

            // 物理演算ウェイト：根元はアニメーションに固定(0.0)、毛先ほど物理で揺れる(1.0)
            data[targetIndex].physicsWeight = progress * progress;

            //// デバッグカラー：写真のように綺麗に見えるよう、ガイドの位置や進捗でグラデーションをかける
            //data[targetIndex].color.x = 0.2f + progress * 0.6f;                       // R
            //data[targetIndex].color.y = 0.15f + (1.0f - progress) * 0.3f;              // G
            //data[targetIndex].color.z = 0.4f + std::sin((float)g * 0.05f) * 0.2f + progress * 0.4f; // B

            data[targetIndex].color.x = rootColor.x * (1.0f - progress) + tipColor.x * progress;
            data[targetIndex].color.y = rootColor.y * (1.0f - progress) + tipColor.y * progress;
            data[targetIndex].color.z = rootColor.z * (1.0f - progress) + tipColor.z * progress;
        }
    }
}

void GenerateDefaultShortHair(GuideCurve::ControllerPoint* data, uint32_t totalCount, float headRadius,
    float bangLength, float backLength, Vector3 headCenter, Vector3 rootColor,
    Vector3 tipColor) {
    const int POINTS_PER_GUIDE = 16;
    int totalGuides = totalCount / POINTS_PER_GUIDE;

    float bangSegLen = bangLength / (POINTS_PER_GUIDE - 1);
    float backSegLen = backLength / (POINTS_PER_GUIDE - 1);

    bool isZPlusFront = true;
    float forwardSign = isZPlusFront ? 1.0f : -1.0f;

    for (int g = 0; g < totalGuides; ++g) {

        float t_geo = (float)g / (float)(totalGuides - 1);
        float y_local = 1.0f - t_geo * 1.3f;

        float radiusAtY = std::sqrt((std::max)(0.0f, 1.0f - y_local * y_local));
        float goldenAngle = 2.39996322f;
        float theta = g * goldenAngle;

        float x_local = std::cos(theta) * radiusAtY;
        float z_local = std::sin(theta) * radiusAtY;

        Vector3 normal = { x_local, y_local, z_local };
        float n_len = std::sqrt(normal.x * normal.x + normal.y * normal.y + normal.z * normal.z);
        if (n_len > 0.0f) { normal.x /= n_len; normal.y /= n_len; normal.z /= n_len; }

        // -----------------------------------------------------------
        // 【修正1】ワンレンボブ・ぱっつん前髪用の長さ計算
        // -----------------------------------------------------------
        // 毛先を水平に揃えるため、頭頂部(normal.yが1)の毛を長く、下の方の毛を短くします。
        float heightFactor = 0.3f + (std::max)(0.0f, normal.y) * 0.7f;
        float currentBackSegLen = backSegLen * heightFactor;

        // 前髪も同様に、上から生える毛ほど長くして毛先を揃えます
        float currentBangSegLen = bangSegLen * (0.5f + (std::max)(0.0f, normal.y) * 0.5f);

        float localSegmentLength = currentBackSegLen;
        bool isFrontArea = isZPlusFront ? (normal.z > 0.0f) : (normal.z < 0.0f);
        bool isBang = false;

        if (isFrontArea && normal.y > 0.0f) {
            float blend = std::abs(normal.z);

            if (blend > 0.5f) {
                localSegmentLength = currentBangSegLen; // 修正した長さを適用
                isBang = true;
            }
            else {
                float t = blend / 0.5f;
                localSegmentLength = currentBackSegLen * (1.0f - t) + currentBangSegLen * t;
            }
        }

        Vector3 rootPos = {
            headCenter.x + normal.x * headRadius,
            headCenter.y + normal.y * headRadius,
            headCenter.z + normal.z * headRadius
        };

        Vector3 currentPos = rootPos;

        for (int p = 0; p < POINTS_PER_GUIDE; ++p) {
            int targetIndex = (g * POINTS_PER_GUIDE) + p;
            float progress = (float)p / (float)(POINTS_PER_GUIDE - 1);

            if (p == 0) {
                data[targetIndex].position = rootPos;
            }
            else {
                Vector3 dir;
                float curveInfluence = progress * progress;

                if (isBang) {
                    float volumeBoost = std::sin(progress * 3.141592f);

                    // 【修正2】横幅(X)を絞りすぎない。0.8から0.2に変更。
                    // これで顔の中心に寄らず、額の幅を保ったまま下に落ちます。
                    dir.x = normal.x * (1.0f - progress * 0.2f);
                    dir.y = -0.2f - curveInfluence * 1.5f;
                    dir.z = normal.z * (1.0f - progress * 0.2f) + (forwardSign * volumeBoost * 0.15f);
                }
                else {
                    // 後ろ・サイドも同様に横幅をキープ
                    dir.x = normal.x * (1.0f - progress * 0.2f);
                    dir.y = -0.2f - curveInfluence * 2.0f;
                    dir.z = normal.z * (1.0f - progress * 0.2f);
                }

                float d_len = std::sqrt(dir.x * dir.x + dir.y * dir.y + dir.z * dir.z);
                if (d_len > 0.0f) { dir.x /= d_len; dir.y /= d_len; dir.z /= d_len; }

                currentPos.x += dir.x * localSegmentLength;
                currentPos.y += dir.y * localSegmentLength;
                currentPos.z += dir.z * localSegmentLength;

                data[targetIndex].position = currentPos;
            }

            data[targetIndex].homePosition = data[targetIndex].position;
            data[targetIndex].radius = 0.0035f * (1.0f - progress * 0.8f) + 0.0005f;
            data[targetIndex].nextToLength = localSegmentLength;
            data[targetIndex].physicsWeight = progress * progress;

            data[targetIndex].color.x = rootColor.x * (1.0f - progress) + tipColor.x * progress;
            data[targetIndex].color.y = rootColor.y * (1.0f - progress) + tipColor.y * progress;
            data[targetIndex].color.z = rootColor.z * (1.0f - progress) + tipColor.z * progress;
        }
    }
}

void GenerateDefaultShortHair2(GuideCurve::ControllerPoint* data, uint32_t totalCount, float headRadius,
    float bangLength, float sideLength, float backLength, Vector3 headCenter, Vector3 rootColor,
    Vector3 tipColor) {
    const int POINTS_PER_GUIDE = 16;
    int totalGuides = totalCount / POINTS_PER_GUIDE;

    float bangSegLen = bangLength / (POINTS_PER_GUIDE - 1);
    float sideSegLen = sideLength / (POINTS_PER_GUIDE - 1);
    float backSegLen = backLength / (POINTS_PER_GUIDE - 1);

    bool isZPlusFront = true;
    float forwardSign = isZPlusFront ? 1.0f : -1.0f;

    for (int g = 0; g < totalGuides; ++g) {

        float t_geo = (float)g / (float)(totalGuides - 1);
        float y_local = 1.0f - t_geo * 1.3f;

        float radiusAtY = std::sqrt((std::max)(0.0f, 1.0f - y_local * y_local));
        float goldenAngle = 2.39996322f;
        float theta = g * goldenAngle;

        float x_local = std::cos(theta) * radiusAtY;
        float z_local = std::sin(theta) * radiusAtY;

        Vector3 normal = { x_local, y_local, z_local };
        float n_len = std::sqrt(normal.x * normal.x + normal.y * normal.y + normal.z * normal.z);
        if (n_len > 0.0f) { normal.x /= n_len; normal.y /= n_len; normal.z /= n_len; }

        // -----------------------------------------------------------
        // 毛先を水平に揃えるための高さ補正(頭頂部ほど長め)
        // -----------------------------------------------------------
        float heightFactor = 0.3f + (std::max)(0.0f, normal.y) * 0.7f;
        float currentBackSegLen = backSegLen * heightFactor;
        float currentSideSegLen = sideSegLen * heightFactor;
        float currentBangSegLen = bangSegLen * (0.5f + (std::max)(0.0f, normal.y) * 0.5f);

        bool isFrontArea = isZPlusFront ? (normal.z > 0.0f) : (normal.z < 0.0f);
        bool isBang = false;

        // デフォルトは襟足(短い)
        float localSegmentLength = currentBackSegLen;

        if (isFrontArea) {
            if (normal.y > 0.0f) {
                // --- おでこ〜頭上部: 前髪 or サイドへのブレンド ---
                float blend = std::abs(normal.z); // 0=横(サイド) ~ 1=正面(前髪センター)

                if (blend > 0.5f) {
                    // 真正面に近いエリア → ぱっつん前髪
                    localSegmentLength = currentBangSegLen;
                    isBang = true;
                }
                else {
                    // 前髪の脇 → サイドの長い毛束へ滑らかに移行
                    float t = blend / 0.5f;
                    localSegmentLength = currentSideSegLen * (1.0f - t) + currentBangSegLen * t;
                }
            }
            else {
                // --- もみあげ〜頬の高さ: 顔まわりの長いサイド毛束 ---
                // z が 0 を超えた瞬間から素早く sideLength に立ち上げる
                // (= 襟足のすぐ前から顔まわりは長くなる、グラデーションボブの境界線)
                float t = (std::min)(1.0f, std::abs(normal.z) / 0.4f);
                localSegmentLength = currentBackSegLen * (1.0f - t) + currentSideSegLen * t;
            }
        }

        Vector3 rootPos = {
            headCenter.x + normal.x * headRadius,
            headCenter.y + normal.y * headRadius,
            headCenter.z + normal.z * headRadius
        };

        Vector3 currentPos = rootPos;

        for (int p = 0; p < POINTS_PER_GUIDE; ++p) {
            int targetIndex = (g * POINTS_PER_GUIDE) + p;
            float progress = (float)p / (float)(POINTS_PER_GUIDE - 1);

            if (p == 0) {
                data[targetIndex].position = rootPos;
            }
            else {
                Vector3 dir;
                float curveInfluence = progress * progress;

                // 毛先側でわずかに外ハネさせる(写真のボブの「外に流れる毛先」用)
                float flickOut = 0.0f;
                if (progress > 0.65f) {
                    float ft = (progress - 0.65f) / 0.35f;
                    flickOut = ft * ft * 0.35f;
                }

                if (isBang) {
                    float volumeBoost = std::sin(progress * 3.141592f);

                    // 横幅(X)を絞りすぎない。額の幅を保ったまま下に落とす。
                    dir.x = normal.x * (1.0f - progress * 0.2f);
                    dir.y = -0.2f - curveInfluence * 1.5f;
                    dir.z = normal.z * (1.0f - progress * 0.2f) + (forwardSign * volumeBoost * 0.15f);
                }
                else {
                    // 後ろ・サイドも横幅をキープしつつ、毛先だけわずかに外ハネ
                    dir.x = normal.x * (1.0f - progress * 0.2f) + normal.x * flickOut;
                    dir.y = -0.2f - curveInfluence * 2.0f;
                    dir.z = normal.z * (1.0f - progress * 0.2f);
                }

                float d_len = std::sqrt(dir.x * dir.x + dir.y * dir.y + dir.z * dir.z);
                if (d_len > 0.0f) { dir.x /= d_len; dir.y /= d_len; dir.z /= d_len; }

                currentPos.x += dir.x * localSegmentLength;
                currentPos.y += dir.y * localSegmentLength;
                currentPos.z += dir.z * localSegmentLength;

                data[targetIndex].position = currentPos;
            }

            data[targetIndex].homePosition = data[targetIndex].position;
            data[targetIndex].radius = 0.0035f * (1.0f - progress * 0.8f) + 0.0005f;
            data[targetIndex].nextToLength = localSegmentLength;
            data[targetIndex].physicsWeight = progress * progress;

            data[targetIndex].color.x = rootColor.x * (1.0f - progress) + tipColor.x * progress;
            data[targetIndex].color.y = rootColor.y * (1.0f - progress) + tipColor.y * progress;
            data[targetIndex].color.z = rootColor.z * (1.0f - progress) + tipColor.z * progress;
        }
    }
}
void HairGuideEditor::Update(){
    EventManager::GetInstance()->FireEvent(GAMEEVENTID::HairEditor);
    auto* cpuData = hairSystem_->GetCPUGuideData();

    if (ImGui::IsMouseClicked(0) && !ImGui::GetIO().WantCaptureMouse) {

        ImVec2 mousePos = ImGui::GetMousePos();

        // 画面サイズ・カメラの逆行列・カメラ位置を取得
        float windowWidth = 1280.0f;  // 実際の画面幅に合わせてください
        float windowHeight = 720.0f;  // 実際の画面高に合わせてください

        Matrix4x4 invProjView = Matrix4x4::Inverse(CameraSystem::GetInstance()->GetActiveCamera()->GetViewProjectionMatrix());
        Vector3 camPos = CameraSystem::GetInstance()->GetActiveCamera()->GetTranslation();

        // マウス座標から3D空間のレイを生成
        Ray ray = MathUtils::CalculateRayFromScreen(
            mousePos.x, mousePos.y,
            windowWidth, windowHeight,
            invProjView, camPos
        );

        int closestPoint = -1;
        float minDistance = FLT_MAX;
        float hitDist = 0.0f;
        float clickRadius = 0.005f; // 点の当たり判定の大きさ（細かければ調整）

        // CPU側のガイドデータを取得
        uint32_t count = hairSystem_->GetCPUGuideCount();

        // 全ての制御点に対してレイ判定を行う
        for (uint32_t i = 0; i < count; ++i) {
            Vector3 localPos = cpuData[i].position;
            Vector4 localPos4 = { localPos.x, localPos.y, localPos.z, 1.0f };
            Vector4 worldPos4 = Matrix4x4::Transform(hairSystem_->characterMatrix_, localPos4);
            Vector3 worldPos = { worldPos4.x / worldPos4.w, worldPos4.y / worldPos4.w, worldPos4.z / worldPos4.w };

            if (MathUtils::IntersectRaySphere(ray, worldPos, clickRadius, &hitDist)) {
                if (hitDist < minDistance) {
                    minDistance = hitDist;
                    closestPoint = i;
                }
            }
        }

        // 選択結果を反映（何もない場所をクリックしたら -1 にして選択解除）
        selectedPointIndex_ = closestPoint;
    }

    float scale = 0.005f;
    for (uint32_t i = 0; i < hairSystem_->GetCPUGuideCount(); ++i) {
        PrimitiveSphereData data;
        data.worldTransform.Initialize();

        Vector3 localPos = cpuData[i].position;
        Vector4 localPos4 = { localPos.x, localPos.y, localPos.z, 1.0f };
        Vector4 worldPos4 = Matrix4x4::Transform(hairSystem_->characterMatrix_, localPos4);
        Vector3 worldPos = { worldPos4.x / worldPos4.w, worldPos4.y / worldPos4.w, worldPos4.z / worldPos4.w };

        data.worldTransform.set_.Translation(worldPos);
        data.worldTransform.set_.Scale({ scale,scale,scale });
        data.worldTransform.LocalToWorld();
        data.color = { 1.0f,1.0f,1.0f,1.0f };
        DrawManager::GetInstance()->GetSphere()->AddInstance(data);
    }
}

////////////////////////////////
//
// 【 UIに関する関数 】
//
////////////////////////////////
void HairGuideEditor::DrawUI(){
    uint32_t count = hairSystem_->GetCPUGuideCount();
    auto* cpuData = hairSystem_->GetCPUGuideData();

    // ---------------------------------------------------
    // 🎛️ 2. ImGui によるパラメータ編集 UI
    // ---------------------------------------------------
    if (selectedPointIndex_ >= 0 && selectedPointIndex_ < (int)count) {
        ImGui::Text("Selected Point: %d", selectedPointIndex_);
        ImGui::Separator();

        auto& point = cpuData[selectedPointIndex_];
        bool isEdited = false;

        // --- 個別のパラメータ編集（変更があればGPU更新を投げる） ---
        if (ImGui::SliderFloat("Radius", &point.radius, 0.01f, 1.0f)) isEdited = true;
        if (ImGui::ColorEdit3("Color", &point.color.x)) isEdited = true;
        if (ImGui::SliderFloat("Physics Weight", &point.physicsWeight, 0.0f, 1.0f)) isEdited = true;

        if (isEdited) {
            hairSystem_->RequestNotifyUpdate();
        }
    }
    else {
        ImGui::Text("No control point selected.");
    }

    // =================================================================
    // 🛠️ 3. 3Dギズモ（ImGuizmo）による座標操作と Undo/Redo 対応
    // =================================================================
    if (selectedPointIndex_ >= 0 && selectedPointIndex_ < (int)count) {
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
        /*Vector3* targetPos = &cpuData[selectedPointIndex_].position;*/
        Vector3 localPos = cpuData[selectedPointIndex_].position;
        Vector4 localPos4 = { localPos.x, localPos.y, localPos.z, 1.0f };
        Vector4 worldPos4 = Matrix4x4::Transform(hairSystem_->characterMatrix_, localPos4);
        Vector3 targetPos = { worldPos4.x / worldPos4.w, worldPos4.y / worldPos4.w, worldPos4.z / worldPos4.w };

        // 今の座標からギズモ用の変換行列を作る
        float gizmoMatrix[16] = {
            1, 0, 0, 0,
            0, 1, 0, 0,
            0, 0, 1, 0,
            targetPos.x, targetPos.y, targetPos.z, 1
        };

        ImGuizmo::BeginFrame();
        ImGuiIO& io = ImGui::GetIO();
        ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);
        ImGuizmo::Manipulate(viewMatrix, projectionMatrix, ImGuizmo::TRANSLATE, ImGuizmo::WORLD, gizmoMatrix);

        // ギズモ操作中
        if (ImGuizmo::IsUsing()) {
            if (!isGizmoUsingLastFrame_) {
                // 操作が始まった瞬間に、今の座標をバックアップ
                gizmoOldPosition_ = cpuData[selectedPointIndex_].position;
            }

            // 新しい座標を取得して、一時的に直接書き換える（リアルタイムプレビューのため）
            Vector3 newWorldPos = { gizmoMatrix[12], gizmoMatrix[13], gizmoMatrix[14] };
            Vector4 newWorldPos4 = { newWorldPos.x, newWorldPos.y, newWorldPos.z, 1.0f };
            Vector4 newLocalPos4 = Matrix4x4::Transform(Matrix4x4::Inverse(hairSystem_->characterMatrix_), newWorldPos4);
            Vector3 newLocalPos = { newLocalPos4.x / newLocalPos4.w, newLocalPos4.y / newLocalPos4.w, newLocalPos4.z / newLocalPos4.w };

            cpuData[selectedPointIndex_].position = newLocalPos;
            cpuData[selectedPointIndex_].homePosition = newLocalPos;
            hairSystem_->RequestNotifyUpdate(); // GPUにリアルタイム送信

            isGizmoUsingLastFrame_ = true;
        }
        else {
            // 操作を終えて手を離した瞬間（確定！）
            if (isGizmoUsingLastFrame_) {
                Vector3 newPos = cpuData[selectedPointIndex_].position;

                // 一旦、触る前の状態に戻す（コマンドの中で書き換えてもらうため）
                cpuData[selectedPointIndex_].position = gizmoOldPosition_;
                cpuData[selectedPointIndex_].homePosition = gizmoOldPosition_;

                // 既に用意されている完璧なコマンドを発行！
                ExecuteCommand(std::make_unique<HairGuideMoveCommand>(
                    hairSystem_, selectedPointIndex_, gizmoOldPosition_, newPos
                ));

                isGizmoUsingLastFrame_ = false;
            }
        }
    }
    ImGui::Separator();

    ImGui::TextColored(ImVec4(0.3f, 0.8f, 1.0f, 1.0f), "--- Auto Generation Parameters ---");

    // クラスのメンバ変数にするのが理想ですが、手軽に試せるよう static 変数にしています
    static float headRadius = 0.3f;       // 頭部（球体）の半径
    static float segmentLength = 0.04f;   // 制御点の間隔（髪の長さに影響）
    static Vector3 headCenter = {0.0f,0.0f,0.0f};   // 髪の原点
    static float bangLength = 0.02f;  // 前髪の長さ（短め）
    static float sideLength = 0.08f;  // 再度の長さ（短め）
    static float backLength = 0.08f;  // 後ろ髪の長さ（長め）
    // マウスドラッグで数値を微調整できるようにする（増減スピードを設定）
    ImGui::DragFloat("Head Radius", &headRadius, 0.0005f, 0.005f, 2.0f, "%.3f");
    ImGui::DragFloat("Segment Length", &segmentLength, 0.001f, 0.005f, 0.5f, "%.4f");
    ImGui::DragFloat3("HeadCenter", &headCenter.x, 0.001f, -0.5f, 0.5f, "%.4f");
    ImGui::DragFloat("Bang Length (前髪)", &bangLength, 0.001f, 0.005f, 0.5f, "%.4f");
    ImGui::DragFloat("Side Length", &sideLength, 0.001f, 0.01f, 1.0f, "%.4f");
    ImGui::DragFloat("Back/Side Length (後ろ髪)", &backLength, 0.001f, 0.01f, 1.0f, "%.4f");

    static ImVec4 rootColor = ImVec4(0.1f, 0.08f, 0.08f, 1.0f); // 根元の色
    static ImVec4 tipColor = ImVec4(0.25f, 0.18f, 0.12f, 1.0f);  // 毛先の色

    ImGui::ColorEdit3("Root Color (根元)", &rootColor.x);
    ImGui::ColorEdit3("Tip Color (毛先)", &tipColor.x);

    // 現在の設定値での全体の髪の長さの目安を表示
    ImGui::Text("Estimated Total Length: %.2f units", segmentLength * (16 - 1));

    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.6f, 0.2f, 1.0f)); // 目立つように緑色にする
    if (ImGui::Button("Reset to Photo Sphere Layout", ImVec2(-1, 35))) {

        Vector3 cRoot = { rootColor.x, rootColor.y, rootColor.z };
        Vector3 cTip = { tipColor.x,  tipColor.y,  tipColor.z };

        // 関数を呼び出して16000点を一瞬で再計算
        GenerateDefaultSphereHair(cpuData, count, headRadius, segmentLength, headCenter, cRoot, cTip);

        // 即座にGPUバッファに全転送
        hairSystem_->RequestNotifyUpdate();
    }
    if (ImGui::Button("Reset to Photo Sphere Layout 2", ImVec2(-1, 35))) {

        Vector3 cRoot = { rootColor.x, rootColor.y, rootColor.z };
        Vector3 cTip = { tipColor.x,  tipColor.y,  tipColor.z };

        // 関数を呼び出して16000点を一瞬で再計算
        GenerateDefaultShortHair(cpuData, count, headRadius, bangLength, backLength, headCenter, cRoot, cTip);

        // 即座にGPUバッファに全転送
        hairSystem_->RequestNotifyUpdate();
    }
    if (ImGui::Button("Reset to Photo Sphere Layout 3", ImVec2(-1, 35))) {

        Vector3 cRoot = { rootColor.x, rootColor.y, rootColor.z };
        Vector3 cTip = { tipColor.x,  tipColor.y,  tipColor.z };

        // 関数を呼び出して16000点を一瞬で再計算
        GenerateDefaultShortHair2(cpuData, count, headRadius, bangLength, sideLength,backLength, headCenter, cRoot, cTip);

        // 即座にGPUバッファに全転送
        hairSystem_->RequestNotifyUpdate();
    }
    
    ImGui::PopStyleColor();

    // UIのボタン
    if (ImGui::Button("Save to JSON")) {

        // 1. JSON用の保存ダイアログを開く
        std::string savePath = FileSystem::ShowSaveFileDialogJson();

        // 2. パスが取得できたらJSON保存を実行
        if (!savePath.empty()) {
            SaveHairDataToJson(savePath, cpuData, count);
        }
    }

    ImGui::SameLine(); // ← これを入れると次のボタンが右隣に並びます

    if (ImGui::Button("Load from JSON")) {
        // 1. ファイル選択ダイアログを開く
        std::string openPath = FileSystem::ShowOpenFileDialogJson();

        // 2. パスが取得できたら読み込み処理を実行
        if (!openPath.empty()) {
            LoadHairDataFromJson(openPath, cpuData, count, hairSystem_);
        }
    }

    /*ImGui::End();*/

    // 選択中の情報などを出すデバッグ用ImGuiウィンドウ
    ImGui::Begin("Hair Gizmo Controller");
    if (selectedPointIndex_ != -1) {
        ImGui::Text("Selected Point Index: %d", selectedPointIndex_);
    }
    else {
        ImGui::Text("Click a hair control point to select.");
    }
    ImGui::End();
}