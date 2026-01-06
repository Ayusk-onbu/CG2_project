#pragma once
#include "ModelObject.h"

class Ground
{
public:
	void Initialize(Fngine* fngine);
	void Update();
	void Draw();
private:
	std::unique_ptr<ModelObject>obj_ = nullptr;
};

