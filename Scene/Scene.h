#pragma once
#include "Fngine.h"

class Scene
{
public:
	virtual void Initialize() = 0;
	virtual void Update() = 0;
	virtual void Draw() = 0;
	virtual const char* NextSceneRequest() { return nullptr; }
	virtual void FngineSetUp(Fngine& fngine) { p_fngine_ = &fngine; }
public:
	Fngine* p_fngine_;
};

