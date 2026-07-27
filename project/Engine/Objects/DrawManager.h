#pragma once
#include "Primitive/Box/PrimitiveBox.h"
#include "Primitive/Ring/Ring.h"
#include "Primitive/Sphere/PrimitiveSphere.h"
#include "Primitive/Cylinder/Cylinder.h"
#include "Primitive/MagicPlane.h"
#include "Primitive/Line/Line.h"
#include "Primitive/Grass/Grass.h"
#include "Primitive/Column/Column.h"

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
	

	//   ========
	// 【 Cylinder 】
	//   ========
	std::unique_ptr<PrimitiveCylinder>cylinder_;


	//   ======
	// 【 Line 】
	//   ======
	std::unique_ptr<PrimitiveLine>line_;


	//   ======
	// 【 MagicCircle 】
	//   ======
	std::unique_ptr<MagicCircle>magicCircle_;


	//   =======
	// 【 Grass 】
	//   =======
	std::unique_ptr<Grass>grass_;


	//   =======
	// 【 Column 】
	//   =======
	std::unique_ptr<Column>column_;


	//=============
	//  Getの関数 
	//=============
public:
	PrimitiveSphere* GetSphere() { return sphere_.get(); }

	PrimitiveCylinder* GetCylinder() { return cylinder_.get(); }

	PrimitiveLine* GetLine() { return line_.get(); }

	MagicCircle* GetMagicCircle() { return magicCircle_.get(); }

	Grass* GetGrass() { return grass_.get(); }

	Column* GetColumn() { return column_.get(); }
};

