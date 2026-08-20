#pragma once
#include "../IEditor.h"
#include "Hair/IHair.h"
#include "HairEditorCommand.h"
#include "../Hermite/HermiteEditor.h"

class HairGuideEditor :
	public BaseEditor<GuideCurve::ControllerPoint>
{
private:
    Hair* hairSystem_ = nullptr;
    int selectedGuideIndices_[3] = {0,-1,-1};
	bool blendMode_ = false; // 3つのガイドを選択している場合に、Blendモードにするかどうか
    std::vector<int> selectedPointIndices_ = { 0 }; // とりあえず最初のポイントをいじる用
	float weights_[3] = { 1.0f, 0.0f, 0.0f }; // Blendモード時の重み（合計1.0になるように）
    
    // (ミラー用)現在選択中のガイドおよび頂点インデックス
    int selectedGuideIndex_ = -1;
    int selectedVertexIndexWithinGuide_ = -1;
    float mirrorAxisX_ = 0.0f; // ミラー面のX座標（標準は 0.0）

    // ドラッグ＆Undo管理用のフラグ
    bool isGizmoUsingLastFrame_ = false;
    std::map<int,Vector3> gizmoOldPositions_; // ギズモを触る前の位置

    Vector3 gizmoStartCenter_ = { 0, 0, 0 };
    bool isDrawPoint_ = true;


private:
    Vector3 newGuidePos = { 0.0f, 0.19f, 0.0f }; // 初期位置（頭上付近）
    Vector3 newGuideDir = { 0.0f, 0.0f, 1.0f }; // 初期方向（真横）
    float newGuideLength = 0.4f;
    float newGuideRootRad = 0.015f;
    float newGuideTipRad = 0.002f;
    float newGuideRootCol[3] = { 0.1f, 0.05f, 0.02f }; // 暗い茶色
    float newGuideTipCol[3] = { 0.5f, 0.3f,  0.15f }; // 明るい茶色

    //
    // 【 Guideを追加する機能 】
private:
    void AddGuide(const Vector3& rootPosition, const Vector3& direction, float totalLength,
        float rootRadius, float tipRadius, const Vector3& rootColor, const Vector3& tipColor);
    int addGuideNum_ = 8;// 追加するガイドのポイントの数

    void AddGuideFromSpline(const std::vector<MathUtils::Spline::Node<Vector3>*>& splineNodes,
        float rootRadius, float tipRadius, const Vector3& rootColor, const Vector3& tipColor);

    /// <summary>
	/// 指定したガイドをX軸でミラーコピーして新しいガイドを作成する
    /// </summary>
    /// <param name="sourceGuideIndex">複製元のガイドのインデックス</param>
    /// <param name="mirrorAxisX">ミラーコピーするX軸の位置</param>
    /// <returns>新しいガイドの作成に成功した場合は true、失敗した場合は false</returns>
    bool CreateMirrorGuide(uint32_t sourceGuideIndex, float mirrorAxisX = 0.0f);

    // リアルタイムスプライン編集用の変数
    std::unique_ptr<HermiteEditor> hermiteEditor_ = nullptr;
    bool isEditingSpline_ = false;

    // 
    // 【 Strandを追加する機能 】
    //
private:
    void AddChildStrand(int parentGuideIds[3],   // 追従する親ガイドのID
		                float parentWeights[3], // 追従する親ガイドの重み（合計1.0になるように）
                        const Vector2& offset,    // ガイドからの2Dオフセット（散らばり具合）
                        uint32_t vertexCount,     // この髪の毛1本の頂点数（例: 8や16など補間後の数）
                        float lengthScale,        // 長さの倍率
                        float twistAngle,         // ねじれ
                        float clumpForce          // 束感の強さ)
    );

public:
    // コンストラクタで編集対象のHairシステムを受け取る
    HairGuideEditor(Hair* hair) : BaseEditor("Hair Guide Editor"), hairSystem_(hair) {}

    void Update() override;
    void DrawUI() override;

    void GenerateDefaultSphereHair(GuideCurve::ControllerPoint* data, uint32_t totalCount, float headRadius,
        float segmentLength, Vector3 headCenter, Vector3 rootColor,
        Vector3 tipColor);

    void GenerateDefaultShortHair(GuideCurve::ControllerPoint* data, uint32_t totalCount, float headRadius,
        float bangLength, float backLength, Vector3 headCenter, Vector3 rootColor,
        Vector3 tipColor);

    void GenerateDefaultShortHair2(GuideCurve::ControllerPoint* data, uint32_t totalCount, float headRadius,
        float bangLength, float sideLength, float backLength, Vector3 headCenter, Vector3 rootColor,
        Vector3 tipColor);

    /**
     * @brief 髪の編集データをバイナリファイルとして保存する
     * @param filename 保存先のファイルパス
     * @param saveData 保存するデータ
     * @return 保存に成功した場合は true、失敗した場合は false
     */
    bool SaveHairSaveData(const std::string& filename, const Strands::HairSaveData& saveData);

    /**
     * @brief バイナリファイルから髪の編集データを読み込む
     * @param filename 読み込み元のファイルパス
     * @param outSaveData 読み込んだデータの格納先
     * @return 読み込みに成功した場合は true、失敗した場合は false
     */
    bool LoadHairSaveData(const std::string& filename, Strands::HairSaveData& outSaveData);

    /**
     * @brief 現在のエディタの髪データをバイナリファイルとして保存する
     * @param filename 保存先のファイルパス (.bin などの拡張子)
     * @return 成功したら true
     */
    bool SaveToFile(const std::string& filename);

    /**
     * @brief バイナリファイルからデータを読み込み、Hairシステムを再初期化する
     * @param filename 読み込み元のファイルパス
     * @param engine 初期化に必要なFngineポインタ
     * @return 成功したら true
     */
    bool LoadFromFile(const std::string& filename, Fngine* engine);

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