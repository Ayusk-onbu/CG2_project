#pragma once
#include "Transform.h"
#include "Matrix4x4.h"

class WorldTransform
{
public:
	struct Get {
		WorldTransform* parent_;
		Vector3& Scale();
		Vector3& Rotation();
		Vector3& Translation();
	};
	struct Set {
		WorldTransform* parent_;
		void Scale(const Vector3& scale);
		void Rotation(const Vector3& rotation);
		void Translation(const Vector3& translation);
	};
public:
	void Initialize();
	void LocalToWorld();
	const Vector3 GetWorldPos()const;
public:
	Get get_{this};
	Set set_{this};

	Transform transform_;
	Matrix4x4 mat_;
	bool isDirty_;
};