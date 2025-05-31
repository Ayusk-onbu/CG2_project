#pragma once
#include "Structures.h"
/// <summary>
/// デバッグカメラ
/// </summary>
class DebugCamera
{
private:
	// x,y,z軸周りのローカル回転角
	Vector3 rotation_ = {0,0,0};
	// ローカル座標
	Vector3 translation_ = { 0,0,-50 };
	// ビュー行列
	Matrix4x4 viewPortMatrix_ = viewPortMatrix_.Identity();
	// 射影行列
	Matrix4x4 projectionMatrix_ = projectionMatrix_.Identity();
public:
#pragma region 初期化(Initialize)
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();
#pragma endregion
#pragma region 更新(UpData)
	/// <summary>
	/// 更新
	/// </summary>
	void UpData();
#pragma endregion
};

