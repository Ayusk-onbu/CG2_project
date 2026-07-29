#include "SceneEditor.h"
#include "DrawManager.h"
#include "FileSystem.h"
#include "MathUtils.h"

void SceneEditor::Update() {
	auto& objects_ = targetObjects_;

	// ---------------------------------------------------
	// 1. マウスクリックによる3Dオブジェクト選択（Raycast）
	// ---------------------------------------------------
	if (ImGui::IsMouseClicked(0) && !ImGui::GetIO().WantCaptureMouse) {

		ImVec2 mousePos = ImGui::GetMousePos();

		float windowWidth = 1280.0f;
		float windowHeight = 720.0f;
		Matrix4x4 invProjView = Matrix4x4::Inverse(CameraSystem::GetInstance()->GetActiveCamera()->GetViewProjectionMatrix());
		Vector3 camPos = CameraSystem::GetInstance()->GetActiveCamera()->GetTranslation();

		// 画面座標からレイを生成[cite: 8]
		Ray ray = MathUtils::CalculateRayFromScreen(
			mousePos.x, mousePos.y,
			windowWidth, windowHeight,
			invProjView, camPos
		);

		int closestObjIndex = -1;
		float minDistance = FLT_MAX;
		float hitDist = 0.0f;

		for (int i = 0; i < (int)objects_.size(); ++i) {

			auto* obj = objects_[i];
			if (!obj) continue;

			Vector3 objPos = obj->GetTransform().get_.Translation();

			// 球体交差判定でクリックしたオブジェクトを探す[cite: 8]
			if (MathUtils::IntersectRaySphere(ray, objPos, selectRadius_, &hitDist)) {

				if (hitDist < minDistance) {

					minDistance = hitDist;
					closestObjIndex = i;
				}
			}
		}

		selectedObjectIndex_ = closestObjIndex; // 選択インデックスを更新[cite: 8]
	}

	// ---------------------------------------------------
	// 2. 選択オブジェクト・デバッグ枠の描画
	// ---------------------------------------------------
	for (int i = 0; i < (int)objects_.size(); ++i) {

		auto* obj = objects_[i];
		if (!obj) continue;

		PrimitiveSphereData data;
		data.worldTransform.Initialize();
		data.worldTransform.set_.Translation(obj->GetTransform().get_.Translation());
		data.worldTransform.set_.Scale({ 0.5f, 0.5f, 0.5f });
		data.worldTransform.LocalToWorld();

		// 選択中のオブジェクトは黄色、その他は白で強調描画[cite: 8]
		if (i == selectedObjectIndex_) {
			data.color = { 1.0f, 1.0f, 0.0f, 1.0f };
		}
		else {
			data.color = { 1.0f, 1.0f, 1.0f, 0.3f };
		}
		DrawManager::GetInstance()->GetSphere()->AddInstance(data);
	}
}

void SceneEditor::DrawUI() {
	auto& objects_ = targetObjects_;

	// ---------------------------------------------------
	// 全体操作：新規オブジェクト生成ボタン
	// ---------------------------------------------------
	if (ImGui::Button("Add Object 追加")) {

		auto* newObj = new DynamicObject();
		if (!objects_.empty()) {
			Vector3 lastPos = objects_.back()->GetTransform().get_.Translation();
			newObj->SetPosition({ lastPos.x + 2.0f, lastPos.y, lastPos.z });
		}
		ExecuteCommand(std::make_unique<SceneObjectAddCommand>(&targetObjects_, newObj));
	}

	ImGui::SameLine();

	// Undo / Redo ボタン[cite: 3]
	if (ImGui::Button("Undo")) Undo();
	ImGui::SameLine();
	if (ImGui::Button("Redo")) Redo();

	ImGui::Separator();

	// ---------------------------------------------------
	// セーブ・ロード処理 (JSON)
	// ---------------------------------------------------
	if (ImGui::Button("Save Scene...")) {

		json sceneJson = json::array();
		for (auto* obj : objects_) {
			if (!obj) continue;
			json objJson;
			auto pos = obj->GetTransform().get_.Translation();
			auto rot = obj->GetTransform().get_.Rotation();
			auto scale = obj->GetTransform().get_.Scale();

			objJson["position"] = { pos.x, pos.y, pos.z };
			objJson["rotation"] = { rot.x, rot.y, rot.z };
			objJson["scale"] = { scale.x, scale.y, scale.z };
			sceneJson.push_back(objJson);
		}
		FileSystem::SaveWithDialog(sceneJson);
	}

	ImGui::SameLine();

	if (ImGui::Button("Load Scene...")) {

		json loadedJson;
		if (FileSystem::LoadWithDialog(loadedJson)) {

			for (auto* obj : objects_) { delete obj; }
			objects_.clear();

			for (const auto& item : loadedJson) {
				auto* newObj = new DynamicObject();
				newObj->SetPosition({ item["position"][0], item["position"][1], item["position"][2] });
				newObj->SetRotation({ item["rotation"][0], item["rotation"][1], item["rotation"][2] });
				newObj->SetScale({ item["scale"][0], item["scale"][1], item["scale"][2] });
				objects_.push_back(newObj);
			}
			selectedObjectIndex_ = -1;
		}
	}

	ImGui::Separator();

	// ギズモ操作モード切替ラジオボタン
	ImGui::RadioButton("Translate", &gizmoOperation_, 0); ImGui::SameLine();
	ImGui::RadioButton("Rotate", &gizmoOperation_, 1); ImGui::SameLine();
	ImGui::RadioButton("Scale", &gizmoOperation_, 2);

	ImGui::Separator();

	// ---------------------------------------------------
	// オブジェクト一覧（Hierarchy）と Transform 編集
	// ---------------------------------------------------
	for (int i = 0; i < (int)objects_.size(); ++i) {

		ImGui::PushID(i);

		if (ImGui::Button("X")) {

			ExecuteCommand(std::make_unique<SceneObjectDeleteCommand>(&targetObjects_, i));
			ImGui::PopID();
			break;
		}
		ImGui::SameLine();

		if (ImGui::TreeNode(("Object " + std::to_string(i)).c_str())) {

			if (ImGui::Selectable("Select", selectedObjectIndex_ == i)) {
				selectedObjectIndex_ = i;
			}

			TransformState oldState{
				objects_[i]->GetTransform().get_.Translation(),
				objects_[i]->GetTransform().get_.Rotation(),
				objects_[i]->GetTransform().get_.Scale()
			};
			bool isEdited = false;

			Vector3 pos = oldState.position;
			Vector3 rot = oldState.rotation;
			Vector3 scale = oldState.scale;

			if (ImGui::DragFloat3("Position", &pos.x, 0.1f)) { objects_[i]->SetPosition(pos); }
			if (ImGui::IsItemDeactivatedAfterEdit()) isEdited = true;

			if (ImGui::DragFloat3("Rotation", &rot.x, 0.1f)) { objects_[i]->SetRotation(rot); }
			if (ImGui::IsItemDeactivatedAfterEdit()) isEdited = true;

			if (ImGui::DragFloat3("Scale", &scale.x, 0.1f)) { objects_[i]->SetScale(scale); }
			if (ImGui::IsItemDeactivatedAfterEdit()) isEdited = true;

			// 数値ドラッグが離された瞬間に Command 発行して Undo 登録
			if (isEdited) {

				TransformState newState{
					objects_[i]->GetTransform().get_.Translation(),
					objects_[i]->GetTransform().get_.Rotation(),
					objects_[i]->GetTransform().get_.Scale()
				};
				objects_[i]->SetPosition(oldState.position);
				objects_[i]->SetRotation(oldState.rotation);
				objects_[i]->SetScale(oldState.scale);

				ExecuteCommand(std::make_unique<DynamicObjectTransformCommand>(objects_[i], oldState, newState));
			}

			ImGui::TreePop();
		}
		ImGui::PopID();
	}

	// ---------------------------------------------------
	// ImGuizmo の3D操作と確定検知ロジック
	// ---------------------------------------------------
	if (selectedObjectIndex_ < 0 || selectedObjectIndex_ >= (int)objects_.size()) return;

	auto camera = CameraSystem::GetInstance()->GetActiveCamera();
	auto viewMat = camera->GetViewMatrix();
	auto projMat = camera->GetProjectionMatrix();

	float viewMatrix[16], projectionMatrix[16];
	for (int r = 0; r < 4; ++r) {
		for (int c = 0; c < 4; ++c) {
			viewMatrix[r * 4 + c] = viewMat.m[r][c];
			projectionMatrix[r * 4 + c] = projMat.m[r][c];
		}
	}

	DynamicObject* selectedObj = objects_[selectedObjectIndex_];
	Matrix4x4 worldMat = selectedObj->GetTransform().mat_; // ワールド行列

	float gizmoMatrix[16];
	for (int r = 0; r < 4; ++r) {
		for (int c = 0; c < 4; ++c) {
			gizmoMatrix[r * 4 + c] = worldMat.m[r][c];
		}
	}

	ImGuizmo::BeginFrame();
	ImGuiIO& io = ImGui::GetIO();
	ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);

	// モード切り替え (Translate, Rotate, Scale)
	ImGuizmo::OPERATION op = ImGuizmo::TRANSLATE;
	if (gizmoOperation_ == 1) op = ImGuizmo::ROTATE;
	if (gizmoOperation_ == 2) op = ImGuizmo::SCALE;

	ImGuizmo::Manipulate(viewMatrix, projectionMatrix, op, ImGuizmo::WORLD, gizmoMatrix);

	if (ImGuizmo::IsUsing()) {

		// ★ 操作開始の1フレーム目：元の状態をバックアップ
		if (!isGizmoUsingLastFrame_) {

			gizmoOldState_ = {
				selectedObj->GetTransform().get_.Translation(),
				selectedObj->GetTransform().get_.Rotation(),
				selectedObj->GetTransform().get_.Scale()
			};
		}

		// 行列から位置・回転・スケールを抽出してリアルタイム反映
		Vector3 translation, rotation, scale;
		ImGuizmo::DecomposeMatrixToComponents(gizmoMatrix, &translation.x, &rotation.x, &scale.x);

		selectedObj->SetPosition(translation);
		selectedObj->SetRotation(rotation);
		selectedObj->SetScale(scale);

		isGizmoUsingLastFrame_ = true;
	}
	else {
		// ★ 操作を終えて手を離した瞬間：Commandを発行して確定！
		if (isGizmoUsingLastFrame_) {

			TransformState gizmoNewState = {
				selectedObj->GetTransform().get_.Translation(),
				selectedObj->GetTransform().get_.Rotation(),
				selectedObj->GetTransform().get_.Scale()
			};

			// 一旦触る前に戻してから Command 実行に任せる
			selectedObj->SetPosition(gizmoOldState_.position);
			selectedObj->SetRotation(gizmoOldState_.rotation);
			selectedObj->SetScale(gizmoOldState_.scale);

			ExecuteCommand(std::make_unique<DynamicObjectTransformCommand>(selectedObj, gizmoOldState_, gizmoNewState));

			isGizmoUsingLastFrame_ = false;
		}
	}
}