#pragma once
#include "Structures.h"
#include <type_traits>

class Camera;

// 基礎型をIntだと明示(DeftでInt)
enum class CameraType : int {
	None = 0,
	Normal = 1 << 0,
	Debug = 1 << 1,
	Game = 1 << 2,
};

inline CameraType operator|(CameraType lhs, CameraType rhs) {
	return static_cast<CameraType>(
		static_cast<std::underlying_type_t<CameraType>>(lhs) |
		static_cast<std::underlying_type_t<CameraType>>(rhs));
}

inline CameraType operator&(CameraType lhs, CameraType rhs) {
	return static_cast<CameraType>(
		static_cast<std::underlying_type_t<CameraType>>(lhs) &
		static_cast<std::underlying_type_t<CameraType>>(rhs));
}

class CameraController
{
public:
	virtual void Update(Camera& camera) = 0;
public:
	virtual CameraType GetType() = 0;
};

class NormalCameraController
	:public CameraController
{
public:
	void Update(Camera& camera) override;
	CameraType GetType()override { return CameraType::Normal; }
};

class DebugCameraController
	:public CameraController
{
public:
	void Update(Camera& camera) override;
	CameraType GetType()override { return CameraType::Debug; }
private:

};

class GameCameraController
	:public CameraController 
{
public:
	void Update(Camera& camera)override;
	CameraType GetType()override { return CameraType::Game; }
private:
	float panSpeed_ = 0.005f;
};