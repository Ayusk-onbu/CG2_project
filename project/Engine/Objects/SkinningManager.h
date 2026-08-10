#pragma once
#include "ModelData.h"
#include "ISingleton.h"
#include <unordered_map>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

class SkinningManager
	: public ISingleton<SkinningManager>
{
public:
	friend class ISingleton<SkinningManager>;

public:
    /// <summary>
        /// AssimpのSceneからスキニング静的データを解析・一括登録する
        /// </summary>
    void RegisterFromAssimp(const std::string& id, const aiScene* scene, uint32_t numVertices);

    /// <summary>
    /// ModelData の解析結果からスキニング静的データを変換・登録する
    /// </summary>
    void RegisterFromModelData(
        const std::string& id,
        const Node& rootNode,
        const std::map<std::string, JointWeightData>& skinClusterData,
        uint32_t numVertices
    );

    // データ取得関数
    const SkeletonTemplate* GetSkeletonTemplate(const std::string& id) const;
    const SkinningStaticData* GetSkinningStaticData(const std::string& id) const;
    bool HasSkinningData(const std::string& id) const;

private:
    // aiNode 階層から SkeletonTemplate を再帰構築する内部関数
    int32_t CreateJointTemplate(const aiNode* node, const std::optional<int32_t>& parent, SkeletonTemplate& outTemplate);
    int32_t CreateJointTemplateFromNode(const Node& node, const std::optional<int32_t>& parent, SkeletonTemplate& outTemplate);
private:
    std::unordered_map<std::string, SkeletonTemplate> skeletonTemplates_;
    std::unordered_map<std::string, SkinningStaticData> skinningStaticDatas_;

};