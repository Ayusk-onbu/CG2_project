#include "HairEditor.h"

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
    // -----------------------------------------------------------
    // A. マウスピッキングによる制御点の選択
    // -----------------------------------------------------------
    // 左クリックされた、かつ「ユーザーが今ギズモを触っていない」ときにピッキングを行う
    // ※ ImGuizmo::IsOver() チェックを入れないと、ギズモを掴んだ瞬間に選択が解除されてしまいます！
    //if (ImGui::IsMouseClicked(ImGuiMouseButton_Left) && !ImGuizmo::IsOver()) {

    //    // ピッキング関数を呼び出して、衝突した制御点のインデックスを取得
    //    selectedPointIndex_ = PickControlPointFromMouse();

    //    // 選択したポインタを BaseEditor の管理リストにも登録しておく（任意）
    //    if (selectedPointIndex_ != -1) {
    //        auto* cpuData = hairSystem_->GetCPUGuideData();
    //        SetTargetObjects({ &cpuData[selectedPointIndex_] });
    //    }
    //    else {
    //        // 何もない空間をクリックしたら選択解除
    //        SetTargetObjects({});
    //    }
    //}
}

void HairGuideEditor::DrawUI(){
    // 1. 生ポインタと全体の要素数を取得
    GuideCurve::ControllerPoint* cpuData = hairSystem_->GetCPUGuideData();
    uint32_t totalCount = hairSystem_->GetCPUGuideCount();

    if (!cpuData || totalCount == 0) return;

    // --- 設定値（環境に合わせて定数やHairクラスからの取得に変えてください） ---
    // 例: 1本のガイド（髪の束）に4つの制御点があると仮定
    const int POINTS_PER_GUIDE = 16;
    // 全体の要素数から、ガイドが何本あるかを逆算
    int totalGuides = totalCount / POINTS_PER_GUIDE;

    ImGui::Begin("Hair Guide Editor (All Points)");

    // 通知用のフラグ
    bool isAnyChanged = false;

    // 2. 全てのガイドをループで回す
    for (int g = 0; g < totalGuides; ++g) {

        // ImGui内部での識別IDをコンパイラに教える（これがないと全スライダーが連動してバグります）
        ImGui::PushID(g);

        // ガイドごとに折りたたみヘッダーを作成
        char guideLabel[64];
        sprintf_s(guideLabel, "Guide [%d] (Points %d - %d)", g, g * POINTS_PER_GUIDE, (g + 1) * POINTS_PER_GUIDE - 1);

        if (ImGui::CollapsingHeader(guideLabel)) {

            // インデントを下げて見やすくする
            ImGui::Indent(10.0f);

            // 3. そのガイドに所属する全制御点をループで並べる
            for (int p = 0; p < POINTS_PER_GUIDE; ++p) {
                ImGui::PushID(p);

                // 1次元配列上の本当のインデックスを計算
                int targetIndex = (g * POINTS_PER_GUIDE) + p;
                auto& targetPoint = cpuData[targetIndex];

                // 根元・中間・毛先が分かりやすいようにラベルを工夫
                const char* pointType = "Middle";
                if (p == 0) pointType = "Root (根元)";
                else if (p == POINTS_PER_GUIDE - 1) pointType = "Tip (毛先)";

                ImGui::Text("Point [%d] - %s", p, pointType);

                // 横一列に並べるために、ラベルを短くしてコンパクトに配置
                ImGui::SetNextItemWidth(250.0f); // 座標ドラッグの幅を固定
                if (ImGui::DragFloat3("Pos", &targetPoint.position.x, 0.02f)) {
                    isAnyChanged = true;
                }

                ImGui::SameLine();
                ImGui::SetNextItemWidth(80.0f); // 半径スライダーの幅を固定
                if (ImGui::DragFloat("Rad", &targetPoint.radius, 0.002f, 0.0f, 1.0f)) {
                    isAnyChanged = true;
                }

                ImGui::Separator(); // 点ごとの区切り線
                ImGui::PopID(); // 点のIDをポップ
            }

            ImGui::Unindent(10.0f); // インデントを戻す
        }

        ImGui::PopID(); // ガイドのIDをポップ
    }

    // 4. いずれかの点が変わっていたら、最後にまとめて1発だけGPU転送要求
    if (isAnyChanged) {
        hairSystem_->RequestNotifyUpdate();
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
    ImGui::Text("Estimated Total Length: %.2f units", segmentLength * (POINTS_PER_GUIDE - 1));

    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.6f, 0.2f, 1.0f)); // 目立つように緑色にする
    if (ImGui::Button("Reset to Photo Sphere Layout", ImVec2(-1, 35))) {

        Vector3 cRoot = { rootColor.x, rootColor.y, rootColor.z };
        Vector3 cTip = { tipColor.x,  tipColor.y,  tipColor.z };

        // 関数を呼び出して16000点を一瞬で再計算
        GenerateDefaultSphereHair(cpuData, totalCount, headRadius, segmentLength, headCenter, cRoot, cTip);

        // 即座にGPUバッファに全転送
        hairSystem_->RequestNotifyUpdate();
    }
    if (ImGui::Button("Reset to Photo Sphere Layout 2", ImVec2(-1, 35))) {

        Vector3 cRoot = { rootColor.x, rootColor.y, rootColor.z };
        Vector3 cTip = { tipColor.x,  tipColor.y,  tipColor.z };

        // 関数を呼び出して16000点を一瞬で再計算
        GenerateDefaultShortHair(cpuData, totalCount, headRadius, bangLength, backLength, headCenter, cRoot, cTip);

        // 即座にGPUバッファに全転送
        hairSystem_->RequestNotifyUpdate();
    }
    if (ImGui::Button("Reset to Photo Sphere Layout 3", ImVec2(-1, 35))) {

        Vector3 cRoot = { rootColor.x, rootColor.y, rootColor.z };
        Vector3 cTip = { tipColor.x,  tipColor.y,  tipColor.z };

        // 関数を呼び出して16000点を一瞬で再計算
        GenerateDefaultShortHair2(cpuData, totalCount, headRadius, bangLength, sideLength,backLength, headCenter, cRoot, cTip);

        // 即座にGPUバッファに全転送
        hairSystem_->RequestNotifyUpdate();
    }
    
    ImGui::PopStyleColor();

    // UIのボタン
    if (ImGui::Button("Save to JSON")) {

        // 1. JSON用の保存ダイアログを開く
        std::string savePath = ShowSaveFileDialogJson();

        // 2. パスが取得できたらJSON保存を実行
        if (!savePath.empty()) {
            SaveHairDataToJson(savePath, cpuData, totalCount);
        }
    }

    ImGui::SameLine(); // ← これを入れると次のボタンが右隣に並びます

    if (ImGui::Button("Load from JSON")) {
        // 1. ファイル選択ダイアログを開く
        std::string openPath = ShowOpenFileDialogJson();

        // 2. パスが取得できたら読み込み処理を実行
        if (!openPath.empty()) {
            LoadHairDataFromJson(openPath, cpuData, totalCount, hairSystem_);
        }
    }

    ImGui::End();





    // 選択中の情報などを出すデバッグ用ImGuiウィンドウ
    ImGui::Begin("Hair Gizmo Controller");
    if (selectedPointIndex_ != -1) {
        ImGui::Text("Selected Point Index: %d", selectedPointIndex_);
    }
    else {
        ImGui::Text("Click a hair control point to select.");
    }
    ImGui::End();

    //// -----------------------------------------------------------
    //// B. ImGuizmo による3Dギズモの描画と操作
    //// -----------------------------------------------------------
    //selectedPointIndex_ = 0;
    //if (selectedPointIndex_ == -1) return;

    ////auto* cpuData = hairSystem_->GetCPUGuideData();
    //auto& targetPoint = cpuData[selectedPointIndex_];

    //// 2. 自作エンジンから、現在のカメラの「View行列」と「Projection行列」を取得する
    //// ※お使いのエンジンのカメラシステムから float[16] で行列を取り出してください
    //Matrix4x4 viewMatrix = CameraSystem::GetInstance()->GetActiveCamera()->GetViewMatrix();
    //Matrix4x4 projMatrix = CameraSystem::GetInstance()->GetActiveCamera()->GetProjectionMatrix();

    //// ギズモを表示・操作を受け付ける (移動ツール: TRANSLATE, ワールド座標系: WORLD)
    //Vector3 scale = { 1.0f,1.0f,1.0f };
    //Vector3 rotation = { 0.0f,0.0f,0.0f };
    //ImGuiManager::GetInstance()->DrawGizmo(
    //    viewMatrix, projMatrix, targetPoint.position, rotation, scale, ImGuizmo::TRANSLATE, ImGuizmo::WORLD
    //);
    //// ギズモが現在ユーザーにドラッグされているか
    //bool isGizmoUsing = ImGuizmo::IsUsing();

    //// -----------------------------------------------------------
    //// C. ドラッグ開始・中・終了のハンドリング（Undoコマンド発行）
    //// -----------------------------------------------------------
    //if (isGizmoUsing) {
    //    // 【ドラッグ開始の瞬間】
    //    if (!wasGizmoUsing_) {
    //        // 移動前の座標を記録しておく
    //        dragStartPos_ = targetPoint.position;
    //    }

    //    // 【ドラッグ中のリアルタイム処理】
    //    // ギズモの行列から新しい座標を抜き出して、Mappedメモリに直接代入！
    //    /*targetPoint.position.x = gizmoMatrix.m[3][0];
    //    targetPoint.position.y = gizmoMatrix.m[3][1];
    //    targetPoint.position.z = gizmoMatrix.m[3][2];*/

    //    targetPoint.homePosition = targetPoint.position;

    //    // DragFloatの時と同様、毎フレームGPUへの転送を要求してリアルタイムに髪をうねうねさせる
    //    hairSystem_->RequestNotifyUpdate();
    //}
    //else {
    //    // 【ドラッグが離された（終了した）瞬間】
    //    if (wasGizmoUsing_) {
    //        // ドラッグ終了時の最終座標
    //        Vector3 dragEndPos = targetPoint.position;

    //        // ここで初めて「移動コマンド」を作って履歴に積む！
    //        auto cmd = std::make_unique<HairGuideMoveCommand>(
    //            hairSystem_, selectedPointIndex_, dragStartPos_, dragEndPos
    //        );
    //        ExecuteCommand(std::move(cmd));
    //    }
    //}

    //// フラグの状態を更新
    //wasGizmoUsing_ = isGizmoUsing;
}

int HairGuideEditor::PickControlPointFromMouse() {
    // 1. マウスの2D座標を取得
    ImGuiIO& io = ImGui::GetIO();
    float mouseX = io.MousePos.x;
    float mouseY = io.MousePos.y;

    // 画面（ビューポート）のサイズを取得
    float windowWidth = io.DisplaySize.x;
    float windowHeight = io.DisplaySize.y;

    auto camera = CameraSystem::GetInstance()->GetActiveCamera();

    // ★ ここで ray が生成されます！
    Ray ray = CalculateRayFromScreen(mouseX, mouseY, windowWidth, windowHeight, Matrix4x4::Inverse(camera->GetViewProjectionMatrix()), camera->GetTranslation());

    // 3. 髪の毛の全制御点と、飛ばしたRayの「交差判定」を行う
    auto* cpuData = hairSystem_->GetCPUGuideData();
    uint32_t count = hairSystem_->GetCPUGuideCount();

    int closestIndex = -1;
    float minDistance = FLT_MAX;

    for (uint32_t i = 0; i < count; ++i) {
        float dist = 0.0f;

        // 前回作成した最短距離判定を呼ぶ
        bool isHit = CheckRaySphereIntersection(ray, cpuData[i].position, cpuData[i].radius, &dist);

        if (isHit && dist < minDistance) {
            minDistance = dist;
            closestIndex = i;
        }
    }

    return closestIndex; // 衝突した一番近いポイントのインデックスを返す（何もなければ -1）
}

bool HairGuideEditor::CheckRaySphereIntersection(const Ray& ray, const Vector3& sphereCenter, float sphereRadius, float* outDist) {
    // エディタ操作用に、最低でもこの半径(メートル単位)の太さがあるものとして判定する
    // 画面を見ながら 0.05f ～ 0.2f 辺りで調整してください
    const float minClickRadius = 0.1f;
    float finalRadius = (std::max)(sphereRadius, minClickRadius);

    // 1. レイの始点から球の中心へのベクトル
    Vector3 v = sphereCenter - ray.origin;

    // 2. レイの方向に球の中心を投影し、レイに一番近い点までの距離 t を出す
    float t = v.x * ray.direction.x + v.y * ray.direction.y + v.z * ray.direction.z; // Dot(V, Direction)

    // レイの背後（カメラの後ろ）にある点なら除外
    if (t < 0.0f) return false;

    // 3. レイの直線上で、球の中心に「一番近い3D座標」を求める
    Vector3 closestPointOnRay = {
        ray.origin.x + ray.direction.x * t,
        ray.origin.y + ray.direction.y * t,
        ray.origin.z + ray.direction.z * t
    };

    // 4. その一番近い点と、球の中心との距離（の2乗）を計算する
    Vector3 diff = sphereCenter - closestPointOnRay;
    float distSq = diff.x * diff.x + diff.y * diff.y + diff.z * diff.z;

    // クリック許容半径の2乗より離れていれば不時着
    if (distSq > (finalRadius * finalRadius)) {
        return false;
    }

    // 衝突距離として t を返す
    if (outDist) {
        *outDist = t;
    }
    return true;
}

Ray HairGuideEditor::CalculateRayFromScreen(float mouseX, float mouseY, float windowWidth, float windowHeight, const Matrix4x4& invProjView, const Vector3& camPos) {
    // 1. マウス座標をスクリーン空間 [0 ～ Width] から NDC（正規化デバイス座標）空間 [-1 ～ 1] に変換
    // ※ DirectXは左上が原点で、Y軸は下方向がプラスですが、NDCは上がプラスなのでYを反転させます
    float ndcX = (2.0f * mouseX) / windowWidth - 1.0f;
    float ndcY = 1.0f - (2.0f * mouseY) / windowHeight;

    // 2. ニアプレーン（手前）とファープレーン（奥）の点をクリップ空間の座標(Vector4)として定義
    // 通常の射影バッファなら Z=0 が手前、Z=1 が奥（逆Zバッファ運用の場合は 1 と 0 が逆になります）
    Vector4 clipNear = { ndcX, ndcY, 0.0f, 1.0f };
    Vector4 clipFar = { ndcX, ndcY, 1.0f, 1.0f };

    // 3. 逆プロジェクションビュー行列（inverseProjView）を掛けて、ワールド空間の座標に逆変換する
    // ※自作エンジンにある「Vector4 と Matrix4x4 の乗算関数」に差し替えてください
    Vector4 worldNear = Matrix4x4::Transform(invProjView, clipNear);
    Vector4 worldFar = Matrix4x4::Transform(invProjView, clipFar);

    // 4. w成分で割って、通常の3D座標（パースペクティブ除算）にする
    if (worldNear.w != 0.0f) {
        worldNear.x /= worldNear.w; worldNear.y /= worldNear.w; worldNear.z /= worldNear.w;
    }
    if (worldFar.w != 0.0f) {
        worldFar.x /= worldFar.w; worldFar.y /= worldFar.w; worldFar.z /= worldFar.w;
    }

    // 5. レイを組み立てる
    Ray ray;
    ray.origin = camPos; // 始点はカメラの位置

    // 方向 ＝ 奥の点 － 手前の点
    Vector3 dir = { worldFar.x - worldNear.x, worldFar.y - worldNear.y, worldFar.z - worldNear.z };

    // 方向ベクトルを正規化（長さを1にする）
    float length = std::sqrt(dir.x * dir.x + dir.y * dir.y + dir.z * dir.z);
    if (length > 0.0f) {
        ray.direction = { dir.x / length, dir.y / length, dir.z / length };
    }
    else {
        ray.direction = { 0.0f, 0.0f, 1.0f };
    }

    return ray;
}