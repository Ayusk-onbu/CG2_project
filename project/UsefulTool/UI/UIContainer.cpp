#include "UIContainer.h"

UIContainer::UIContainer() {
	name_ = "UIContainer";
}

void UIContainer::UpdateAnimation(float deltaTime) {
	UIElement::UpdateAnimation(deltaTime);
	for (auto& element : elements_) {
		element->UpdateAnimation(deltaTime);
	}
}

void UIContainer::Play(){
	UIElement::PlayAnimation("Test1");
	for (auto& element : elements_) {
		element->PlayAnimation("Test1");
	}
}

void UIContainer::Draw() {
	UIElement::Draw();
	for (auto& element : elements_) {
		element->Draw();
	}
}

void UIContainer::LoadbyFile(const std::string& name) {
	std::string filePath = "resources/Data/UI/" + name + ".json";
	std::ifstream file(filePath);
	if (!file.is_open()) return;

	json root;
	file >> root;

	elements_.clear();
	Load(root); // 再帰読み込みの開始
}

void UIContainer::Load(const json& j) {
	UIElement::Load(j); // 自分の座標などを復元

	if (j.contains("children")) {
		for (const auto& childJson : j["children"]) {
			std::unique_ptr<UIElement> child;
			if (childJson.value("type", "") == "Container") {
				child = std::make_unique<UIContainer>();
			}
			else {
				child = std::make_unique<UIElement>();
			}
			child->Initialize(engine_);
			child->Load(childJson); // 再帰ロード
			child->SetParent(this);
			elements_.push_back(std::move(child));
		}
	}
}

void UIContainer::SavetoFile() {
	json root;
	// 再帰保存の開始
	Save(root);
	std::string filepath = "resources/Data/UI/" + name_ + ".json";
	std::ofstream file(filepath);
	if (file.is_open()) {
		file << root.dump(4);
	}
}

void UIContainer::Save(json& j)const {
	// Elementの情報の保存
	UIElement::Save(j);
	j["type"] = "Container";

	// 子要素の保存
	json children = json::array();
	for (auto& element : elements_) {
		json childJson;
		element->Save(childJson);
		children.push_back(childJson);
	}
	j["children"] = children;
}

void UIContainer::DrawImGui() {
	if (ImGui::TreeNode(("UIContainer : " + name_).c_str())) {

		DrawImGuiElement();

		char buf[128];
		strncpy_s(buf, sizeof(buf), inputName_.c_str(), _TRUNCATE);
		buf[sizeof(buf) - 1] = 0; // 確実にNULL終端
		if (ImGui::InputText("New Element Name", buf, sizeof(buf))) {
			// 入力が変更されたらstd::stringを更新
			inputName_ = buf;
		}
		if (ImGui::Button("AddElement")) {
			AddElement();
		}
		ImGui::SameLine();
		if (ImGui::Button("Add Sub-Container")) {
			AddContainer();
		}

		if (ImGui::TreeNode("Element List")) {
			for (auto& element : elements_) {
				bool isSelected = (currentFocus_ == element.get());
				if (ImGui::Selectable(element->name_.c_str(), isSelected)) {
					currentFocus_ = element.get();
				}
			}
			ImGui::TreePop();
		}

		// --- 2. 選択中の要素の編集 ---
		if (currentFocus_) {
			//ImGui::Text("Currently Editing: %s", currentFocus_->name_.c_str());
			// 既存の座標編集など
			currentFocus_->DrawImGui();
		}

		if (ImGui::Button("Save")) {
			SavetoFile();
		}
		ImGui::SameLine();
		if (ImGui::Button("Load")) {
			LoadbyFile(inputName_);
		}
		ImGui::SameLine();
		if (ImGui::Button("Delete")) {
			elements_.clear();
		}

		ImGui::TreePop();
	}
}

void UIContainer::AddElement() {
	std::unique_ptr<UIElement>newElement = std::make_unique<UIElement>();
	newElement->Initialize(engine_);
	newElement->name_ = "element : " + inputName_;
	newElement->parent_ = this;
	elements_.push_back(std::move(newElement));
}

void UIContainer::AddContainer() {
	std::unique_ptr<UIContainer>newContainer = std::make_unique<UIContainer>();
	newContainer->Initialize(engine_);
	newContainer->name_ = "container : " + inputName_;
	newContainer->parent_ = this;
	elements_.push_back(std::move(newContainer));
}