#include "SceneMap.h"
#include "Chronos.h"

void SceneMap::Initialize() {

}

void SceneMap::Update() {
	float deltaTime = Chronos::GetInstance()->GetGameDeltaTime();
	for (auto& obj : objects_) {
		obj->Update(deltaTime);
	}
}