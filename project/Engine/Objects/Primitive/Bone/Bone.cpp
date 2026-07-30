#include "Bone.h"
#include "ModelManager.h"
#include "CameraSystem.h"

void BoneDrawer::Initialize(Fngine* engine, uint32_t numInstance) {
    PrimitiveBaseModel::Initialize(engine, numInstance);

    // 八面体ボーンの3Dモデル名（ModelManager等に事前にロードしておく）
    useModelName_ = "BoneOctahedron";

    SetCommand = [this]() {
        auto commandList = p_engine_->GetCommand().GetList().GetList();
        auto object = ModelManager::GetInstance()->LoadObjectData(useModelName_);

        commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        commandList->SetGraphicsRootSignature(PSOManager::GetInstance()->GetPSO("Bone").GetRootSignature().GetRS().Get());
        commandList->SetPipelineState(PSOManager::GetInstance()->GetPSO("Bone").GetGPS().Get());

        commandList->IASetVertexBuffers(0, 1, &object.GetVertexBufferView());
        commandList->IASetIndexBuffer(&object.GetIndexBufferView());

        // インスタンシングバッファ（SRV）をセット
        commandList->SetGraphicsRootDescriptorTable(0, instancingBuffer_->GetSRVHandleGPU());

        // インスタンス一括描画
        commandList->DrawIndexedInstanced(object.GetIndexCount(), static_cast<UINT>(instanceDataList_.size()), 0, 0, 0);
        };
}

BoneForGPU BoneDrawer::ConvertToGPUData(const BoneObjectData& data) {
    BoneForGPU returnData{};
    // カメラの View-Projection とボーンの World 行列から WVP を算出
    returnData.WVP = CameraSystem::GetInstance()->GetActiveCamera()->DrawCamera(data.worldMatrix);
    returnData.World = data.worldMatrix;
    returnData.color = data.color;
    return returnData;
}

Matrix4x4 BoneDrawer::CalculateBoneMatrix(const Vector3& startPt, const Vector3& endPt) {
    Vector3 diff = Subtract(endPt, startPt);
    float length = Length(diff);

    if (length < 1e-4f) {
        return Matrix4x4::Make::Scale({ 0.01f, 0.01f, 0.01f }) * Matrix4x4::Make::Translate(startPt);
    }

    // 1. Y軸：ボーンの伸びる方向（Up）
    Vector3 yAxis = Normalize(diff);

    // 2. Z軸：仮の奥方向
    Vector3 tempForward = (std::abs(yAxis.z) < 0.999f) ? Vector3{ 0.0f, 0.0f, 1.0f } : Vector3{ 1.0f, 0.0f, 0.0f };

    // 3. 左手系の外積
    // 左手系: Y(Up) x Z(Front) = X(Right)
    Vector3 xAxis = Normalize(CrossProduct(yAxis, tempForward));
    // 左手系: X(Right) x Y(Up) = Z(Front)
    Vector3 zAxis = CrossProduct(xAxis, yAxis);

    // 4. スケール設定（Yが長さ）
    Vector3 scale = { length * 0.45f, length, length * 0.45f };

    // 5. 行列の組み立て
    Matrix4x4 boneMat = Matrix4x4::Make::Identity();

    boneMat.m[0][0] = xAxis.x * scale.x;  boneMat.m[0][1] = xAxis.y * scale.x;  boneMat.m[0][2] = xAxis.z * scale.x;
    boneMat.m[1][0] = yAxis.x * scale.y;  boneMat.m[1][1] = yAxis.y * scale.y;  boneMat.m[1][2] = yAxis.z * scale.y;
    boneMat.m[2][0] = zAxis.x * scale.z;  boneMat.m[2][1] = zAxis.y * scale.z;  boneMat.m[2][2] = zAxis.z * scale.z;
    boneMat.m[3][0] = startPt.x;          boneMat.m[3][1] = startPt.y;          boneMat.m[3][2] = startPt.z;

    return boneMat;
}

void BoneDrawer::AddSkeleton(const Skeleton& skeleton, const Matrix4x4& modelWorldMatrix, const Vector4& color) {
    for (const auto& joint : skeleton.joints_) {
        // Joint の skeletonSpaceMatrix にモデルのワールド行列を乗算
        Matrix4x4 parentWorldMat = joint.skeletonSpaceMatrix * modelWorldMatrix;

        // Matrix4x4::Transform で原点を変換し、親 Joint のワールド座標を取得
        Vector3 parentPos = Matrix4x4::Transform(Vector3{ 0.0f, 0.0f, 0.0f }, parentWorldMat);

        if (!joint.children.empty()) {
            // 子 Joint が存在する場合：親から各子 Joint へ向かうボーンを作成
            for (int32_t childIndex : joint.children) {
                const auto& childJoint = skeleton.joints_[childIndex];
                Matrix4x4 childWorldMat = childJoint.skeletonSpaceMatrix * modelWorldMatrix;
                Vector3 childPos = Matrix4x4::Transform(Vector3{ 0.0f, 0.0f, 0.0f }, childWorldMat);

                BoneObjectData data;
                data.worldMatrix = CalculateBoneMatrix(parentPos, childPos);
                data.color = color;
                AddInstance(data);
            }
        }
        else {
            // 子がない末端 Joint の場合：親の姿勢に準じた固定のオフセット方向へ描画
            Vector3 defaultOffset = { 0.0f, 0.0f, 0.2f }; // ローカルZ方向に少し伸びる
            Vector3 defaultEndPos = Matrix4x4::Transform(defaultOffset, parentWorldMat);

            BoneObjectData data;
            data.worldMatrix = CalculateBoneMatrix(parentPos, defaultEndPos);
            data.color = color;
            AddInstance(data);
        }
    }
}