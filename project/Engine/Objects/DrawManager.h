#pragma once
#include "Primitive/Box/PrimitiveBox.h"
#include "Primitive/Ring/Ring.h"
#include "Primitive/Sphere/PrimitiveSphere.h"

class DrawManager :
	public ISingleton<DrawManager>
{
public:
	friend class ISingleton<DrawManager>;
	// 基本的な関数
public:
	void Initialize(Fngine* engine);
	void Draw();

private:
	Fngine* engine_;
	
	//------------------------
	//  Modelたち
	//------------------------
private:
	//   ========
	// 【 Sphere 】
	//   ========
	std::unique_ptr<PrimitiveSphere>sphere_;
	 
	//=============
	//  Getの関数 
	//=============
public:
	PrimitiveSphere* GetSphere() { return sphere_.get(); }
};

