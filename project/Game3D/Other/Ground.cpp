#include "Ground.h"

void Ground::Initialize(Fngine* fngine) {
	obj_ = std::make_unique<ModelObject>();
	obj_->modelName_ = "ground";
	obj_->textureName_ = "Legends_Ground";
	obj_->Initialize(fngine);
}

void Ground::Update() {

}

void Ground::Draw() {
	ImGuiManager::GetInstance()->DrawDrag("Ground:Shininess", obj_->materialData_->shininess);
	obj_->LocalToWorld();
	obj_->SetWVPData(CameraSystem::GetInstance()->GetActiveCamera()->DrawCamera(obj_->worldTransform_.mat_));
	obj_->Draw();
}

void ConvenienceModel::Initialize(Fngine* fngine,const std::string& modelName,const std::string& textureName) {
	obj_ = std::make_unique<ModelObject>();
	obj_->modelName_ = modelName.c_str();
	obj_->textureName_ = textureName.c_str();
	obj_->Initialize(fngine);
}

void ConvenienceModel::Update() {
	if (ImGui::TreeNode(obj_->modelName_.c_str())) {
		ImGui::DragFloat3("Scale", &obj_->worldTransform_.transform_.scale_.x);
		ImGui::DragFloat3("Rotate", &obj_->worldTransform_.transform_.rotation_.x);
		ImGui::DragFloat3("Pos", &obj_->worldTransform_.transform_.translation_.x);
		ImGui::TreePop();
	}
}

void ConvenienceModel::Draw() {
	obj_->worldTransform_.isDirty_ = true;
	obj_->LocalToWorld();
	obj_->SetWVPData(CameraSystem::GetInstance()->GetActiveCamera()->DrawCamera(obj_->worldTransform_.mat_));
	obj_->Draw();
}

void AnimModel::Initialize(Fngine* fngine, const std::string& modelName, const std::string& textureName, const std::string& filePath, const std::string& fileName) {
	obj_ = std::make_unique<ModelObject>();
	obj_->modelName_ = modelName.c_str();
	obj_->textureName_ = textureName.c_str();
	obj_->Initialize(fngine);

	animation_ = std::make_unique<Animation>();
	animation_->LoadAnimationFile(filePath.c_str(), fileName.c_str());
	animation_->SetIsLoop(true);

	skeleton_ = std::make_unique<Skeleton>();
	skeleton_->CreateSkeleton(obj_->GetNode());

	obj_->skinCluster_.Create(fngine, *skeleton_, obj_->GetModelData());
}

void AnimModel::Update() {
	obj_->worldTransform_.isDirty_ = true;
	animation_->TimeFlow();
	animation_->ApplyAnimation(*skeleton_.get());
	skeleton_->Update();
	obj_->skinCluster_.Update(*skeleton_);
}

void AnimModel::DrawImGui() {
	if (ImGui::TreeNode(obj_->modelName_.c_str())) {
		ImGui::DragFloat3("Scale", &obj_->worldTransform_.transform_.scale_.x);
		ImGui::DragFloat3("Rotate", &obj_->worldTransform_.transform_.rotation_.x);
		ImGui::DragFloat3("Pos", &obj_->worldTransform_.transform_.translation_.x);
		for (int i = 0; i < 4; ++i) {
			ImGui::DragFloat4(
				(std::string("Matrix") + " [" + std::to_string(i) + "]").c_str(),
				&obj_->worldTransform_.mat_.m[i][0]
			);
		}
		for (int i = 0; i < 4; ++i) {
			ImGui::DragFloat4(
				(std::string("Skeleton") + " [" + std::to_string(i) + "]").c_str(),
				&skeleton_->joints_[0].skeletonSpaceMatrix.m[i][0]
			);
		}
		ImGui::TreePop();
	}
}

void AnimModel::Draw() {
	obj_->worldTransform_.isDirty_ = true;
	obj_->LocalToWorld();
	Matrix4x4 result = obj_->worldTransform_.mat_;
	if (skeleton_ && !skeleton_->joints_.empty()) {
		for (const auto& joint : skeleton_->joints_) {
			result = Matrix4x4::Multiply(result, joint.skeletonSpaceMatrix);
		}
	}
	obj_->worldTransform_.mat_ = result;
	obj_->SetWVPData(CameraSystem::GetInstance()->GetActiveCamera()->DrawCamera(obj_->worldTransform_.mat_));
	obj_->Draw();
}

void Anim2Model::Initialize(Fngine* fngine, const std::string& modelName, const std::string& textureName, const std::string& filePath, const std::string& fileName) {
	obj_ = std::make_unique<ModelObject>();
	obj_->modelName_ = modelName.c_str();
	obj_->textureName_ = textureName.c_str();
	obj_->Initialize(fngine);

	animation_ = std::make_unique<Animation>();
	animation_->LoadAnimationFile(filePath.c_str(), fileName.c_str());
	animation_->SetIsLoop(true);

	skeleton_ = std::make_unique<Skeleton>();
	skeleton_->CreateSkeleton(obj_->GetNode());

	obj_->skinCluster_.Create(fngine, *skeleton_, obj_->GetModelData());
}

void Anim2Model::Update() {
	obj_->worldTransform_.isDirty_ = true;
	animation_->TimeFlow();
	animation_->ApplyAnimation(*skeleton_.get());
	skeleton_->Update();
	obj_->skinCluster_.Update(*skeleton_);
}

void Anim2Model::DrawImGui() {
	if (ImGui::TreeNode(obj_->modelName_.c_str())) {
		ImGui::DragFloat3("Scale", &obj_->worldTransform_.transform_.scale_.x);
		ImGui::DragFloat3("Rotate", &obj_->worldTransform_.transform_.rotation_.x);
		ImGui::DragFloat3("Pos", &obj_->worldTransform_.transform_.translation_.x);
		for (int i = 0; i < 4; ++i) {
			ImGui::DragFloat4(
				(std::string("Matrix") + " [" + std::to_string(i) + "]").c_str(),
				&obj_->worldTransform_.mat_.m[i][0]
			);
		}
		for (int i = 0; i < 4; ++i) {
			ImGui::DragFloat4(
				(std::string("Skeleton") + " [" + std::to_string(i) + "]").c_str(),
				&skeleton_->joints_[0].skeletonSpaceMatrix.m[i][0]
			);
		}
		ImGui::TreePop();
	}
}

void Anim2Model::Draw() {
	obj_->worldTransform_.isDirty_ = true;
	obj_->LocalToWorld();
	obj_->SetWVPData(CameraSystem::GetInstance()->GetActiveCamera()->DrawCamera(obj_->worldTransform_.mat_));
	obj_->Draw(ObjectDrawType::Animation);
}