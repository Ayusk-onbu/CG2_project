#pragma once
#include "OBJ.h"
class PlaneObj : public OBJ
{
public:
	void Initialize(ModelObject& model, CameraBase& camera);
	void Update();
	void Draw(TheOrderCommand& command, PSO& pso, DirectionLight& light, Texture& tex);
private:

};

