#include "OBJ.h"

void OBJ::Initialize(ModelObject& model, CameraBase& camera) {
	model_ = &model;
	camera_ = &camera;
	worldTransform_.Initialize();
	uvWorldTransform_.Initialize();
}
void OBJ::Update() {

}
void OBJ::Draw(TheOrderCommand& command, PSO& pso, DirectionLight& light, Texture& tex) {
	worldTransform_.LocalToWorld();
	uvWorldTransform_.LocalToWorld();
	model_->SetWVPData(camera_->DrawCamera(worldTransform_.mat_), worldTransform_.mat_, uvWorldTransform_.mat_);
	model_->Draw(command, pso, light, tex);
}