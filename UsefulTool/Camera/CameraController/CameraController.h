#pragma once
#include <type_traits>

class Camera;

// 基礎型をIntだと明示(DeftでInt)
enum class CameraType : int {
	None = 0,
	Normal = 1 << 0,
	Debug = 1 << 1,
	

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
	virtual CameraType GetType() { return CameraType::Normal; }
};

class DebugCameraController
	:public CameraController
{
public:
	void Update(Camera& camera) override;
	virtual CameraType GetType() { return CameraType::Debug; }
private:

};