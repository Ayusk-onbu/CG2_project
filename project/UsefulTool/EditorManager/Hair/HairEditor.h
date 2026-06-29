#pragma once
#include "../IEditor.h"
#include "Hair/IHair.h"
#include "HairEditorCommand.h"

struct Ray {
    Vector3 origin;
    Vector3 direction;
};

class HairGuideEditor :
	public BaseEditor<GuideCurve::ControllerPoint>
{
private:
    Hair* hairSystem_ = nullptr;
    int selectedGuideIndex_ = 0; // とりあえず最初のガイドをいじる用
    int selectedPointIndex_ = 0; // とりあえず最初のポイントをいじる用
    // ドラッグ＆Undo管理用のフラグ
    bool wasGizmoUsing_ = false;
    Vector3 dragStartPos_ = { 0, 0, 0 };
public:
    // コンストラクタで編集対象のHairシステムを受け取る
    HairGuideEditor(Hair* hair) : BaseEditor("Hair Guide Editor"), hairSystem_(hair) {}

    void Update() override;

    void DrawUI() override;

    int PickControlPointFromMouse();

    void GetEngineCameraMatrices(float* view, float* proj) {
        // 1. 一度ローカル変数（左辺値）に受けて実体を作る
        auto viewMatrix = CameraSystem::GetInstance()->GetActiveCamera()->GetViewMatrix();
        auto projMatrix = CameraSystem::GetInstance()->GetActiveCamera()->GetProjection();

        // 2. 実体化した変数のアドレスを渡す
        std::memcpy(view, &viewMatrix, sizeof(float) * 16);
        std::memcpy(proj, &projMatrix, sizeof(float) * 16);
    }

    bool CheckRaySphereIntersection(const Ray& ray, const Vector3& sphereCenter, float sphereRadius, float* outDist);

    Ray CalculateRayFromScreen(float mouseX, float mouseY, float windowWidth, float windowHeight, const Matrix4x4& invProjView, const Vector3& camPos);

    // データをJSON形式のテキストとして書き出す関数
    void SaveHairDataToJson(const std::string& filePath, GuideCurve::ControllerPoint* data, uint32_t count) {
        if (filePath.empty() || !data || count == 0) return;

        std::ofstream ofs(filePath);
        if (!ofs.is_open()) return;

        // 浮動小数点が「1.234567」のように程よい精度で出力されるように設定
        ofs << std::setprecision(6) << std::fixed;

        // JSONの配列開始
        ofs << "[\n";

        for (uint32_t i = 0; i < count; ++i) {
            const auto& p = data[i];

            ofs << "  {\n";
            ofs << "    \"position\": {\"x\": " << p.position.x << ", \"y\": " << p.position.y << ", \"z\": " << p.position.z << "},\n";
            ofs << "    \"radius\": " << p.radius << ",\n";
            ofs << "    \"color\": {\"x\": " << p.color.x << ", \"y\": " << p.color.y << ", \"z\": " << p.color.z << "},\n";
            ofs << "    \"nextToLength\": " << p.nextToLength << ",\n";
            ofs << "    \"homePosition\": {\"x\": " << p.homePosition.x << ", \"y\": " << p.homePosition.y << ", \"z\": " << p.homePosition.z << "},\n";
            ofs << "    \"physicsWeight\": " << p.physicsWeight << "\n"; // オブジェクト内の最後の要素にはカンマをつけない

            ofs << "  }";

            // 配列の最後の要素以外にはカンマをつけるルール
            if (i < count - 1) {
                ofs << ",";
            }
            ofs << "\n";
        }

        // JSONの配列終了
        ofs << "]\n";

        ofs.close();
    }

    // JSONテキストファイルを解析して、Mappedメモリに流し込む関数
    void LoadHairDataFromJson(const std::string& filePath, GuideCurve::ControllerPoint* data, uint32_t maxCount, Hair* hairSystem) {
        if (filePath.empty() || !data || maxCount == 0) return;

        std::ifstream ifs(filePath);
        if (!ifs.is_open()) return;

        std::string line;
        uint32_t index = 0;
        bool inObject = false;

        // 読み込み中の一時テンポラリ
        GuideCurve::ControllerPoint tempPoint = {};

        while (std::getline(ifs, line) && index < maxCount) {

            // 1. オブジェクト（ { ）の開始判定
            // ※positionやcolorの行にある { を誤認しないようガードをかけています
            if (line.find("{") != std::string::npos &&
                line.find("\"position\"") == std::string::npos &&
                line.find("\"color\"") == std::string::npos &&
                line.find("\"homePosition\"") == std::string::npos) {

                inObject = true;
                tempPoint = {}; // データをリセットして新しい要素のパースを始める
            }

            // 2. 各メンバ変数のパース（インデントのスペース数が変わっても対応できる検索方式）
            if (inObject) {
                size_t pos_pos = line.find("\"position\":");
                if (pos_pos != std::string::npos) {
                    float x = 0, y = 0, z = 0;
                    if (sscanf_s(line.c_str() + pos_pos, "\"position\": {\"x\": %f, \"y\": %f, \"z\": %f}", &x, &y, &z) == 3) {
                        tempPoint.position = { x, y, z };
                    }
                }

                size_t pos_rad = line.find("\"radius\":");
                if (pos_rad != std::string::npos) {
                    float r = 0;
                    if (sscanf_s(line.c_str() + pos_rad, "\"radius\": %f", &r) == 1) {
                        tempPoint.radius = r;
                    }
                }

                size_t pos_col = line.find("\"color\":");
                if (pos_col != std::string::npos) {
                    float x = 0, y = 0, z = 0;
                    if (sscanf_s(line.c_str() + pos_col, "\"color\": {\"x\": %f, \"y\": %f, \"z\": %f}", &x, &y, &z) == 3) {
                        tempPoint.color = { x, y, z };
                    }
                }

                size_t pos_len = line.find("\"nextToLength\":");
                if (pos_len != std::string::npos) {
                    float len = 0;
                    if (sscanf_s(line.c_str() + pos_len, "\"nextToLength\": %f", &len) == 1) {
                        tempPoint.nextToLength = len;
                    }
                }

                size_t pos_home = line.find("\"homePosition\":");
                if (pos_home != std::string::npos) {
                    float x = 0, y = 0, z = 0;
                    if (sscanf_s(line.c_str() + pos_home, "\"homePosition\": {\"x\": %f, \"y\": %f, \"z\": %f}", &x, &y, &z) == 3) {
                        tempPoint.homePosition = { x, y, z };
                    }
                }

                size_t pos_weight = line.find("\"physicsWeight\":");
                if (pos_weight != std::string::npos) {
                    float w = 0;
                    if (sscanf_s(line.c_str() + pos_weight, "\"physicsWeight\": %f", &w) == 1) {
                        tempPoint.physicsWeight = w;
                    }
                }
            }

            // 3. オブジェクト（ } ）の終了判定
            if (line.find("}") != std::string::npos &&
                line.find("\"position\"") == std::string::npos &&
                line.find("\"color\"") == std::string::npos &&
                line.find("\"homePosition\"") == std::string::npos) {

                if (inObject) {
                    // 解析が終わった1つの制御点を、Mappedメモリ（配列）に直接書き戻す！
                    data[index] = tempPoint;
                    index++;
                    inObject = false;
                }
            }
        }

        ifs.close();

        // 4. 重要：ロードが完了したら、一発だけGPUへデータをフル転送する要求を出す！
        if (index > 0) {
            hairSystem->RequestNotifyUpdate();
        }
    }
};