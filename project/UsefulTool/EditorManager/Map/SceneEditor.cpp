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

		// 画面座標からレイを生成
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

			// 球体交差判定でクリックしたオブジェクトを探す
			if (MathUtils::IntersectRaySphere(ray, objPos, selectRadius_, &hitDist)) {

				if (hitDist < minDistance) {

					minDistance = hitDist;
					closestObjIndex = i;
				}
			}
		}

		selectedObjectIndex_ = closestObjIndex; // 選択インデックスを更新
	}

	// ---------------------------------------------------
	// 選択オブジェクト・デバッグ枠の描画
	// ---------------------------------------------------
	for (int i = 0; i < (int)objects_.size(); ++i) {

		auto* obj = objects_[i];
		if (!obj) continue;

		PrimitiveSphereData data;
		data.worldTransform.Initialize();
		data.worldTransform.set_.Translation(obj->GetTransform().get_.Translation());
		data.worldTransform.set_.Scale({ 0.5f, 0.5f, 0.5f });
		data.worldTransform.LocalToWorld();

		// 選択中のオブジェクトは黄色、その他は白で強調描画
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
	if (!sceneMap_) return;

	// UI描画前に最新のオブジェクトリストに同期
	SyncFromSceneMap();
	// targetObjects_ の参照（BaseEditorのメンバ）
	auto& objects_ = targetObjects_;

	// ---------------------------------------------------
	// 全体操作：新規オブジェクト生成ボタン
	// ---------------------------------------------------
	if (ImGui::Button("Add Object 追加")) {
		// SceneMap を渡してコマンドを発行
		ExecuteCommand(std::make_unique<SceneObjectAddCommand>(sceneMap_, "New Object"));
		SyncFromSceneMap(); // 即時同期
	}

	ImGui::SameLine();
	if (ImGui::Button("Undo")) { Undo(); SyncFromSceneMap(); }
	ImGui::SameLine();
	if (ImGui::Button("Redo")) { Redo(); SyncFromSceneMap(); }

	ImGui::Separator();

	// ---------------------------------------------------
	// セーブ・ロード処理 (JSON)
	// ---------------------------------------------------
	if (ImGui::Button("Load Scene...")) {
		json loadedJson;
		if (FileSystem::LoadWithDialog(loadedJson)) {
			// 生ポインタの delete はせず、SceneMap に完全クリアさせる
			sceneMap_->Clear();

			for (const auto& item : loadedJson) {
				// SceneMap 経由で生成
				auto* newObj = sceneMap_->SpawnObject("LoadedObject");
				newObj->SetPosition({ item["position"][0], item["position"][1], item["position"][2] });
				newObj->SetRotation({ item["rotation"][0], item["rotation"][1], item["rotation"][2] });
				newObj->SetScale({ item["scale"][0], item["scale"][1], item["scale"][2] });

				if (item.contains("components")) {
					for (const auto& compName : item["components"]) {
						newObj->AddComponent(compName.get<std::string>());
					}
				}
			}
			selectedObjectIndex_ = -1;
			SyncFromSceneMap();
		}
	}

	ImGui::Separator();

	// ギズモ操作モード切替ラジオボタン
	ImGui::RadioButton("Translate", &gizmoOperation_, 0); ImGui::SameLine();
	ImGui::RadioButton("Rotate", &gizmoOperation_, 1); ImGui::SameLine();
	ImGui::RadioButton("Scale", &gizmoOperation_, 2);

	ImGui::Separator();

	// ---------------------------------------------------
	// オブジェクト一覧（Hierarchy）と Transform / Component 編集
	// ---------------------------------------------------
	for (int i = 0; i < (int)objects_.size(); ++i) {


		ImGui::PushID(i);

		if (ImGui::Button("X")) {
			ExecuteCommand(std::make_unique<SceneObjectDeleteCommand>(sceneMap_, objects_[i]));ImGui::PopID();
			break;
		}
		ImGui::SameLine();

		if (ImGui::TreeNode(("Object " + std::to_string(i)).c_str())) {


			if (ImGui::Selectable("Select", selectedObjectIndex_ == i)) {

				selectedObjectIndex_ = i;
			}

			// --- 1. Transform 編集 ---
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

			// 数値ドラッグが離された瞬間に Command 発行して Undo 登録[cite: 8]
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

			// ---------------------------------------------------
			// --- 2. Component 管理セクション (★新規追加) ---
			// ---------------------------------------------------
			ImGui::Spacing();
			ImGui::Separator();
			ImGui::Text("Attached Components:");

			const auto& components = objects_[i]->GetComponents();
			for (size_t c = 0; c < components.size(); ++c) {
				Component* comp = components[c].get();
				if (!comp) continue;

				ImGui::PushID(static_cast<int>(c));

				// 型名を判定して表示（"class " を取り除いて綺麗に表示）
				std::string compTypeName = typeid(*comp).name();
				if (compTypeName.find("class ") == 0) compTypeName = compTypeName.substr(6);

				if (ImGui::TreeNode(compTypeName.c_str())) {
					// コンポーネントで設定したUIの表示
					comp->DrawUI();
					ImGui::TreePop();
				}
				ImGui::PopID();
			}

			ImGui::Spacing();

			// Component 追加ボタン & ドロップダウンポップアップ
			if (ImGui::Button("Add Component +")) {
				ImGui::OpenPopup("AddComponentPopup");
			}

			if (ImGui::BeginPopup("AddComponentPopup")) {
				auto registeredNames = ComponentFactory::GetInstance()->GetRegisteredNames();
				for (const auto& compName : registeredNames) {
					if (ImGui::Selectable(compName.c_str())) {
						// Command 経由でコンポーネントを追加 (Undo/Redo対応)
						ExecuteCommand(std::make_unique<AddComponentCommand>(objects_[i], compName));
					}
				}
				ImGui::EndPopup();
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
	Matrix4x4 worldMat = selectedObj->GetTransform().mat_; // ワールド行列[cite: 8]

	float gizmoMatrix[16];
	for (int r = 0; r < 4; ++r) {

		for (int c = 0; c < 4; ++c) {

			gizmoMatrix[r * 4 + c] = worldMat.m[r][c];
		}
	}

	ImGuizmo::BeginFrame();
	ImGuiIO& io = ImGui::GetIO();
	ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);

	// モード切り替え (Translate, Rotate, Scale)[cite: 8]
	ImGuizmo::OPERATION op = ImGuizmo::TRANSLATE;
	if (gizmoOperation_ == 1) op = ImGuizmo::ROTATE;
	if (gizmoOperation_ == 2) op = ImGuizmo::SCALE;

	ImGuizmo::Manipulate(viewMatrix, projectionMatrix, op, ImGuizmo::WORLD, gizmoMatrix);

	if (ImGuizmo::IsUsing()) {


		// 操作開始の1フレーム目：元の状態をバックアップ[cite: 8]
		if (!isGizmoUsingLastFrame_) {


			gizmoOldState_ = {
				selectedObj->GetTransform().get_.Translation(),
				selectedObj->GetTransform().get_.Rotation(),
				selectedObj->GetTransform().get_.Scale()
			};
		}

		// 行列から位置・回転・スケールを抽出してリアルタイム反映[cite: 8]
		Vector3 translation, rotation, scale;
		ImGuizmo::DecomposeMatrixToComponents(gizmoMatrix, &translation.x, &rotation.x, &scale.x);

		selectedObj->SetPosition(translation);
		selectedObj->SetRotation(rotation);
		selectedObj->SetScale(scale);

		isGizmoUsingLastFrame_ = true;
	}
	else {
		// 操作を終えて手を離した瞬間：Commandを発行して確定！[cite: 8]
		if (isGizmoUsingLastFrame_) {


			TransformState gizmoNewState = {
				selectedObj->GetTransform().get_.Translation(),
				selectedObj->GetTransform().get_.Rotation(),
				selectedObj->GetTransform().get_.Scale()
			};

			// 一旦触る前に戻してから Command 実行に任せる[cite: 8]
			selectedObj->SetPosition(gizmoOldState_.position);
			selectedObj->SetRotation(gizmoOldState_.rotation);
			selectedObj->SetScale(gizmoOldState_.scale);

			ExecuteCommand(std::make_unique<DynamicObjectTransformCommand>(selectedObj, gizmoOldState_, gizmoNewState));

			isGizmoUsingLastFrame_ = false;
		}
	}
}