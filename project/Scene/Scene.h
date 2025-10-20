#pragma once
#include "Fngine.h"

class SceneDirector;
class Scene
{
public:
	virtual ~Scene() = default;
public:
	virtual void Initialize() = 0;
	virtual void Update() = 0;
	virtual void Draw() = 0;
	virtual void GetSceneDirector(SceneDirector* sceneDirector) { p_sceneDirector_ = sceneDirector; }
	virtual void FngineSetUp(Fngine& fngine) { p_fngine_ = &fngine; }
public:
	Fngine* p_fngine_;
	SceneDirector* p_sceneDirector_;
	bool hasRequestedNextScene_ = false;
};

