#include "ModelData.h"
#include "ImGuiManager.h"

int32_t Skeleton::CreateJoint(const Node& node, const std::optional<int32_t>& parent, std::vector<Joint>& joints) {
	Joint joint;
	joint.name = node.name;
	joint.transform.Initialize();
	joint.transform.mat_ = node.transform.mat_;
	joint.transform.set_.Scale(node.transform.transform_.scale_);
	joint.transform.set_.Rotation(node.transform.transform_.rotation_);
	joint.transform.set_.Quaternion(node.transform.quaternion_);
	joint.transform.set_.Translation(node.transform.transform_.translation_);
	joint.skeletonSpaceMatrix = Matrix4x4::Make::Identity();
	// 現在登録されている数をIndexにする
	joint.index = int32_t(joints.size());
	joint.parent = parent;
	// スケルトンのJoint列に追加
	joints.push_back(joint);
	for (const Node& child : node.children) {
		// 子ジョイントを作成し、そのIndexを登録
		int32_t childIndex = CreateJoint(child, joint.index, joints);
		joints[joint.index].children.push_back(childIndex);
	}
	// 自身のIndexを返す
	return joint.index;
}

void Skeleton::CreateSkeleton(const Node& rootNode) {
	root_ = CreateJoint(rootNode, {}, joints_);
	// 名前のindexのマッピングを行いアクセスしやすくする
	for (const Joint& joint : joints_) {
		jointMap_.emplace(joint.name, joint.index);
	}
}

void Skeleton::Update() {
	// 全てのJointを更新。親が若いので通常ループで処理可能
	for (Joint& joint : joints_) {
		// Matrixを生成
		joint.transform.LocalToWorld();
		// 親がいるかいないかの判断
		if (joint.parent) {
			// [ 親が存在したら親の行列を掛ける ]
			joint.skeletonSpaceMatrix = joint.transform.mat_ * joints_[*joint.parent].skeletonSpaceMatrix;
		}
		else {
			// [ 親が存在しなければlocalMatrixとskeletonSpaceMatrixは一致する ]
			joint.skeletonSpaceMatrix = joint.transform.mat_;
		}
	}
}