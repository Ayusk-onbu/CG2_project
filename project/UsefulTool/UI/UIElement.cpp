#include "UIElement.h"
#include "UIContainer.h"

void UIElement::Initialize(Fngine* engine) {
	engine_ = engine;
	obj_ = std::make_unique<SpriteObject>(engine);
	obj_->Initialize("GridLine");
}

void UIElement::Draw() {

	if (isVisible_) {
		obj_->worldTransform_.isDirty_ = true;
		if (parent_) {
			parentMat_ = parent_->obj_->worldTransform_.mat_;
			Vector3 parentPos = Normalize({ parent_->obj_->worldTransform_.mat_.m[0][0],parent_->obj_->worldTransform_.mat_.m[0][1],parent_->obj_->worldTransform_.mat_.m[0][2] });
			parentMat_.m[0][0] = parentPos.x;
			parentMat_.m[0][1] = parentPos.y;
			parentMat_.m[0][2] = parentPos.z;
			parentPos = Normalize({ parent_->obj_->worldTransform_.mat_.m[1][0],parent_->obj_->worldTransform_.mat_.m[1][1],parent_->obj_->worldTransform_.mat_.m[1][2] });
			parentMat_.m[1][0] = parentPos.x;
			parentMat_.m[1][1] = parentPos.y;
			parentMat_.m[1][2] = parentPos.z;
			parentPos = Normalize({ parent_->obj_->worldTransform_.mat_.m[2][0],parent_->obj_->worldTransform_.mat_.m[2][1],parent_->obj_->worldTransform_.mat_.m[2][2] });
			parentMat_.m[2][0] = parentPos.x;
			parentMat_.m[2][1] = parentPos.y;
			parentMat_.m[2][2] = parentPos.z;
			obj_->worldTransform_.parent_ = &parentMat_;
		}
		obj_->Draw();
	}
}

void UIElement::ApplyTransform() {
	//// 1. 親（大きな箱）のサイズを取得 (例: 1280x720)
	//Vector2 screenSize = { 1280.0f, 720.0f };
	////アンカーを計算
	//Vector2 anchorPoint = { screenSize.x * anchor_.x, screenSize.y * anchor_.y };
	////ピボットによるずれを計算
	//Vector2 pivotOffset = { size_.x * pivot_.x, size_.y * pivot_.y };
	//screenPos_ = anchorPoint + relativePos_ - pivotOffset;
}

void UIElement::PlayAnimation(const std::string& name, bool loop) {
	playState_.isPlaying = true;
	playState_.currentTime = 0.0f;
	playState_.isLoop = loop;
	playState_.currentAnimName = name;
	playState_.duration = UIHAnimationManager::GetInstance()->GetAnimation(playState_.currentAnimName).GetDuration();
}

// *** 遷移する機能が未完成 ***

void UIElement::UpdateAnimation(float deltaTime) {
	// [ 実行可能なのか確認する ]
	if (playState_.isPlaying == false)return;
	// [ 時間を進める :
	playState_.currentTime += deltaTime;

	auto& anim = UIHAnimationManager::GetInstance()->GetAnimation(playState_.currentAnimName);

	anim.Apply(this, playState_.currentTime);

	if (playState_.currentTime > playState_.duration) {
		// [ ループするのか確認する ]
		if (playState_.isLoop) {
			playState_.currentTime = 0.0f;
		}
		else {
			playState_.currentTime = playState_.duration;
			playState_.isPlaying = false;
			anim.Apply(this, playState_.duration);
		}
	}

	ApplyTransform();
}

void UIElement::Save(json& j)const {
	// 保存
	// [ 名前 ]
	j["name"] = name_;
	// [ Objの情報 ]
	j["pos"] = { obj_->worldTransform_.transform_.translation_.x,
		obj_->worldTransform_.transform_.translation_.y,
		obj_->worldTransform_.transform_.translation_.z };
	j["scale"] = { obj_->worldTransform_.transform_.scale_.x,
		obj_->worldTransform_.transform_.scale_.y,
		obj_->worldTransform_.transform_.scale_.z };
	j["rot"] = { obj_->worldTransform_.transform_.rotation_.x,
		obj_->worldTransform_.transform_.rotation_.y,
		obj_->worldTransform_.transform_.rotation_.z };
	// [*** Matarialの情報 ***]
	// [ 色 ]
	j["color"] = { obj_->materialData_->color.x,
		obj_->materialData_->color.y,
		obj_->materialData_->color.z,
		obj_->materialData_->color.w };
	// [ テクスチャ名 ]
	j["texture"] = obj_->textureName_;
	// [ 視認 ]
	j["isVisible"] = isVisible_;
	
	j["relativePos"] = { relativePos_.x, relativePos_.y, relativePos_.z };
	j["anchor"] = { anchor_.x, anchor_.y };
}

void UIElement::Load(const json& j) {
	// [ 名前 ]
	name_ = j.value("name", "NewElement");

	// [ Objの情報 - Transform ]
	if (j.contains("pos")) {
		obj_->worldTransform_.transform_.translation_ = { j["pos"][0], j["pos"][1], j["pos"][2] };
	}
	if (j.contains("scale")) {
		obj_->worldTransform_.transform_.scale_ = { j["scale"][0], j["scale"][1], j["scale"][2] };
	}
	if (j.contains("rot")) {
		obj_->worldTransform_.transform_.rotation_ = { j["rot"][0], j["rot"][1], j["rot"][2] };
	}

	// [*** Materialの情報 ***]
	// [ 色 ]
	if (j.contains("color")) {
		obj_->materialData_->color = { j["color"][0], j["color"][1], j["color"][2], j["color"][3] };
	}
	// [ テクスチャ名 ]
	obj_->textureName_ = j.value("texture", "GridLine.png"); // デフォルトのテクスチャ名

	// [ 視認 ]
	isVisible_ = j.value("isVisible", true);

	// [ 座標計算補助 ]
	if (j.contains("relativePos")) {
		relativePos_ = { j["relativePos"][0], j["relativePos"][1], j["relativePos"][2] };
	}
	if (j.contains("anchor")) {
		anchor_ = { j["anchor"][0], j["anchor"][1] };
	}

}

void UIElement::DrawImGuiElement() {
#ifdef USE_IMGUI

	static char nameBuffer[128];
	ImGui::InputText("Name", nameBuffer, sizeof(nameBuffer));
	ImGui::SameLine();
	if (ImGui::Button("Apply")) {
		name_ = nameBuffer;
	}

	if (ImGui::Button("Visible")) {
		isVisible_ = !isVisible_;
	}
	ImGui::SameLine();
	if (isVisible_)ImGui::Text("True");
	else { ImGui::Text("False"); }

	// Objの情報
	if (ImGui::TreeNode("Obj")) {
		ImGui::DragFloat3("Scale", &obj_->worldTransform_.transform_.scale_.x);
		Vector3 rot = obj_->worldTransform_.transform_.rotation_;
		ImGui::DragFloat3("Rot", &rot.x);
		obj_->worldTransform_.set_.Rotation(rot);
		ImGui::DragFloat3("Pos", &obj_->worldTransform_.transform_.translation_.x);


		ImGui::TreePop();
	}

	// Materialの情報
	if (ImGui::TreeNode("Material")) {

		ImGui::ColorEdit4("Color", &obj_->materialData_->color.x);
		
		// 一覧の表示（スクロール可能な領域にすると便利）
		if (ImGui::BeginChild("ListRegion", ImVec2(0, 200), true)) {
			for (auto const& [key, value] : TextureManager::GetInstance()->GetData()) {
				// 現在のキーが選択されているかチェック
				bool isSelected = (selectTexture_ == key);

				// Selectableを表示。クリックされたら true を返す
				if (ImGui::Selectable(key.c_str(), isSelected)) {
					selectTexture_ = key;
				}

				// 最初に表示されたときにフォーカスを当てるなどの処理（任意）
				if (isSelected) {
					ImGui::SetItemDefaultFocus();
				}
			}
			ImGui::EndChild();
		}

		ImGui::Separator();

		// 現在の選択状況を表示
		if (!selectTexture_.empty()) {
			ImGui::Text("Selected: %s", selectTexture_.c_str());
			obj_->textureName_ = selectTexture_;
		}
		else {
			ImGui::Text("Select an item from the list.");
			selectTexture_ = obj_->textureName_;
		}

		ImGui::TreePop();
	}

	auto names = UIHAnimationManager::GetInstance()->GetAnimationNames();

	// 現在選択されているアニメーション名を表示するプレースホルダ
	static std::string currentName = "None";
	if (ImGui::BeginCombo("Animations", currentName.c_str())) {
		for (const auto& name : names) {
			bool isSelected = (currentName == name);
			if (ImGui::Selectable(name.c_str(), isSelected)) {
				currentName = name;
				PlayAnimation(currentName,true);
				playState_.isPlaying = false;
			}
		}
		ImGui::EndCombo();
	}

	if (ImGui::Button(playState_.isPlaying ? "Stop" : "Play")) {
		playState_.isPlaying = !playState_.isPlaying;
	}

	if (ImGui::TreeNode("Debug")) {
		for (int i = 0; i < 4; ++i) {
			ImGui::DragFloat4(
				(std::string("Matrix") + " [" + std::to_string(i) + "]").c_str(),
				&obj_->worldTransform_.mat_.m[i][0]
			);
		}
		ImGui::TreePop();
	}
#endif//USE_IMGUI
}

void UIElement::DrawImGui() {
#ifdef USE_IMGUI
	if (ImGui::TreeNode(("UIElement : " + name_).c_str())) {

		DrawImGuiElement();
		
		ImGui::TreePop();
	}
#endif//USE_IMGUI
}