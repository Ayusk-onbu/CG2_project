#include "SceneEditor.h"

void SceneEditor::Update() {

}

void SceneEditor::DrawUI() {
#ifdef USE_IMGUI
	if (ImGui::Button("Birth")) {
		ExecuteCommand(std::make_unique<TestCommand>());
	}
	if (ImGui::Button("Birth My Baby")) {
		ExecuteCommand(std::make_unique<TestCommandND>());
	}
	if (ImGui::Button("Particle Fire")) {
		EventManager::GetInstance()->FireEvent(GAMEEVENTID::OnPlayerAttack);
	}
	if (ImGui::Button("Undo")) {
		Undo();
	}
	ImGui::SameLine();
	if (ImGui::Button("Redo")) {
		Redo();
	}
#endif
}