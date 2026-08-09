#pragma once
#include "DynamicObject.h"
#include "InputHandler.h"
#include "States.h"

class Character : public DynamicObject
{
public:
	Character() = default;
	~Character()override = default;

///////////////////////////
/// 
/// 基本的な存在
///
//////////////////////////
public:
	void Initialize(Fngine* engine, std::string modelName = "cube", std::string textureName = "GridLine")override {}
	/// <summary>
	/// DynamicObjectの更新処理に加えて、等の更新処理
	/// </summary>
	/// <param name="deltaTime"></param>
	void Update(float deltaTime)override {}
	void Draw()override {}
};

