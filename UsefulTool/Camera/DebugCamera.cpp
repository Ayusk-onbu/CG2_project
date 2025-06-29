#include "DebugCamera.h"
#include "InputManager.h"
#include "ImGuiManager.h"

#include "Math.h"

void DebugCamera::Initialize() {
	camera_.scale_ = { 1.0f,1.0f,1.0f };
	camera_.rotation_ = { 0.0f,0.0f,0.0f };
	camera_.matRot_ = {
	1.0f,0.0f,0.0f,0.0f,
	0.0f,1.0f,0.0f,0.0f, 
	0.0f,0.0f,1.0f,0.0f, 
	0.0f,0.0f,0.0f,1.0f, };
	camera_.translation_ = { 0.0f,1.9f,-6.49f };

	projection_.fovY = 0.45f;
	projection_.nearClip = 0.1f;
	projection_.farClip = 100.0f;
	projection_.aspectRatio = 1280.0f / 720.0f;

	IsPivot_ = true;
	speed_ = 0.1f;
	rotateSpeed_ = { 0.0f,0.0f };
	sensitivity_ = 0.01f;
}

void DebugCamera::UpData() {

	rotateSpeed_.x = InputManager::GetMouse().getDelta().x * sensitivity_;
	rotateSpeed_.y = InputManager::GetMouse().getDelta().y * sensitivity_ * 0.5f;

	Zoom();


	MoveUp();
	MoveDown();
	MoveRight();
	MoveLeft();
	RotatePitch();
	RotateYaw();
	RotateRoll();

	ImGui::Begin("Camera");
	ImGui::Checkbox("IsPivot", &IsPivot_);
	ImGuiManager::CreateImGui("scale", camera_.scale_, -1.0f, 1.0f);
	ImGuiManager::CreateImGui("rotate", camera_.rotation_, -36.0f, 36.0f);
	ImGuiManager::CreateImGui("translate", camera_.translation_, -15.0f, 15.0f);
	ImGuiManager::CreateImGui("matRotate", camera_.matRot_, -360.0f, 360.0f);
	ImGuiManager::CreateImGui("fovY", projection_.fovY, 0.45f, 1.0f);
	ImGuiManager::CreateImGui("nearClip", projection_.nearClip, 0.1f, 1.0f);

	ImGuiManager::CreateImGui("sensitivity", sensitivity_, -1.0f, 1.0f);
	ImGui::End();

	if (IsPivot_) {
		viewProjectionMatrix_ = Matrix4x4::Inverse(Matrix4x4::Multiply(Matrix4x4::Multiply(Matrix4x4::Make::Scale(camera_.scale_),Matrix4x4::Make::Translate(camera_.translation_)), camera_.matRot_));
	}
	if (!IsPivot_) {
		viewProjectionMatrix_ = Matrix4x4::Inverse(Matrix4x4::Make::Affine(camera_.scale_, camera_.rotation_, camera_.translation_));
	}
	viewProjectionMatrix_ = Matrix4x4::Multiply(viewProjectionMatrix_, Matrix4x4::Make::PerspectiveFov(projection_.fovY, projection_.aspectRatio, projection_.nearClip, projection_.farClip));
}

Matrix4x4 DebugCamera::DrawCamera(const Matrix4x4& world) {
	Matrix4x4 WVP = Matrix4x4::Multiply(world, viewProjectionMatrix_);
	ImGui::Begin("WVP");
	ImGuiManager::CreateImGui("WVP", WVP, -1.0f, 10.0f);
	ImGui::End();
	return WVP;
}

Matrix4x4 DebugCamera::DrawMirrorCamera(const Matrix4x4& world, Vector3 mirrorPos, Vector3 MirrorNormal) {
	Vector3 pos;
	pos = ChangeMirror(camera_.translation_, mirrorPos, MirrorNormal);
	Vector3 rotate;
	rotate = ChangeMirror(camera_.rotation_, mirrorPos, MirrorNormal);
	//rotate = camera_.rotation_;
	//rotate.y *= -1.0f;
	//rotate.y *= -1.0f;
	//rotate.z *= -1.0f;

	Matrix4x4 mirrorMatRot;
	mirrorMatRot = camera_.matRot_;

	// Y軸180度回転の回転行列を直接代入
	mirrorMatRot.m[0][0] *=  1.0f; mirrorMatRot.m[0][1] *= 1.0f; mirrorMatRot.m[0][2] *=  1.0f; mirrorMatRot.m[0][3] *= 1.0f;
	mirrorMatRot.m[1][0] *=  1.0f; mirrorMatRot.m[1][1] *= -1.0f; mirrorMatRot.m[1][2] *=  1.0f; mirrorMatRot.m[1][3] *= 1.0f;
	mirrorMatRot.m[2][0] *=  1.0f; mirrorMatRot.m[2][1] *= 1.0f; mirrorMatRot.m[2][2] *= 1.0f; mirrorMatRot.m[2][3] *= 1.0f;
	mirrorMatRot.m[3][0] *=  1.0f; mirrorMatRot.m[3][1] *= 1.0f; mirrorMatRot.m[3][2] *=  1.0f; mirrorMatRot.m[3][3] *= 1.0f;

	Vector3 scale;
	//scale = ChangeMirror(camera_.scale_, mirrorPos, MirrorNormal);
	scale = camera_.scale_;
	scale.x *= -1.0f;
	//scale.y *= -1.0f;
	//scale.z *= -1.0f;
	/*if (abs(MirrorNormal.x) > 0.9f) scale.x *= -1.0f;
	if (abs(MirrorNormal.y) > 0.9f) scale.y *= -1.0f;
	if (abs(MirrorNormal.z) > 0.9f) scale.z *= -1.0f;*/
	Matrix4x4 mirrorViewProjectionMatrix;
	if (IsPivot_) {
		//mirrorViewProjectionMatrix = Matrix4x4::Inverse(Matrix4x4::Multiply(Matrix4x4::Make::Translate(camera_.translation_), mirrorMatRot));
		mirrorViewProjectionMatrix = Matrix4x4::Inverse((Matrix4x4::Multiply(Matrix4x4::Make::Translate(camera_.translation_), mirrorMatRot)));
	}
	if (!IsPivot_) {
		mirrorViewProjectionMatrix = Matrix4x4::Inverse(Matrix4x4::Make::Affine(scale, rotate, pos));
	}
	mirrorViewProjectionMatrix = Matrix4x4::Multiply(mirrorViewProjectionMatrix, Matrix4x4::Make::PerspectiveFov(projection_.fovY, projection_.aspectRatio, projection_.nearClip, projection_.farClip));

	Matrix4x4 WVP = Matrix4x4::Multiply(world, mirrorViewProjectionMatrix);
	ImGui::Begin("MirrorWVP");
	ImGuiManager::CreateImGui("WVP", WVP, -1.0f, 10.0f);
	ImGui::End();
	ImGui::Begin("MirrorCamera");
	ImGuiManager::CreateImGui("scale", scale, -1.0f, 1.0f);
	ImGuiManager::CreateImGui("rotate", rotate, -1.0f, 1.0f);
	ImGuiManager::CreateImGui("matRotate", mirrorMatRot, -360.0f, 360.0f);
	ImGuiManager::CreateImGui("translate", pos, -100.0f, 100.0f);
	ImGui::End();
	return WVP;
}

void DebugCamera::Zoom() {
	camera_.translation_.z += InputManager::GetMouse().GetWheel() * 0.01f;
}

void DebugCamera::MoveRight() {
	if (InputManager::GetKey().PressKey(DIK_RIGHT)) {
		camera_.translation_.x -= speed_;
	}
}

void DebugCamera::MoveLeft() {
	if (InputManager::GetKey().PressKey(DIK_LEFT)) {
		camera_.translation_.x += speed_;
	}
}

void DebugCamera::MoveUp() {
	if (InputManager::GetKey().PressKey(DIK_UP)) {
		camera_.translation_.y -= speed_;
	}
}

void DebugCamera::MoveDown() {
	if (InputManager::GetKey().PressKey(DIK_DOWN)) {
		camera_.translation_.y += speed_;
	}
}

void DebugCamera::RotatePitch() {
	if (InputManager::GetMouse().IsButtonPress(2)) {
		if (IsPivot_) { GetRotateDelta(rotateSpeed_.y, 0.0f); }
		if (!IsPivot_) { camera_.rotation_.x -= rotateSpeed_.y; }
	}
}

void DebugCamera::RotateRoll() {
	if (InputManager::GetMouse().IsButtonPress(2)) {
		if (IsPivot_) { GetRotateDelta(0.0f,rotateSpeed_.x); }
		if (!IsPivot_) { camera_.rotation_.y += rotateSpeed_.x; }
	}
}


void DebugCamera::RotateYaw() {
	if (InputManager::GetMouse().IsButtonPress(0)) {
		//camera_.rotation_.z += rotateSpeed_.x;
	}
}

Vector3 ChangeMirror(Vector3 pos, Vector3 mirrorPos, Vector3 MirrorNormal) {
	Vector3 ret;
	ret = Subtract(pos,Multiply(2,Multiply(Dot(Subtract(pos, mirrorPos), MirrorNormal),MirrorNormal)));
	return ret;
}