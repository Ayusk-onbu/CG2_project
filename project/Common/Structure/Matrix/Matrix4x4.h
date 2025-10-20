#pragma once
#include "Trigonometric.h"
#include "Vector3.h"
#include "Vector4.h"

class Matrix4x4 {
public:
	float m[4][4];
public:

#pragma region MakeUtils
	struct Make {
#pragma region 単位化行列の関数(MakeIdentity)
		/// <summary>
		/// 単位化行列を返す関数
		/// </summary>
		/// <returns></returns>
		static Matrix4x4 Identity();
#pragma endregion

#pragma region Scaleの行列の生成(MakeScale)
		/// <summary>
		/// Scaleの行列を作成する関数
		/// </summary>
		/// <param name="scale"></param>
		/// <returns></returns>
		static Matrix4x4 Scale(const Vector3& scale);
#pragma endregion

#pragma region X軸回転の行列の作成(MakeRotateX)
		/// <summary>
		/// 
		/// </summary>
		/// <param name="radian"></param>
		/// <returns></returns>
		static Matrix4x4 RotateX(float radian);
#pragma endregion

#pragma region Y軸回転の行列の作成(MakeRotateY)
		/// <summary>
		/// 
		/// </summary>
		/// <param name="radian"></param>
		/// <returns></returns>
		static Matrix4x4 RotateY(float radian);
#pragma endregion

#pragma region Z軸回転の行列の作成(MakeRotateZ)
		/// <summary>
		/// 
		/// </summary>
		/// <param name="radian"></param>
		/// <returns></returns>
		static Matrix4x4 RotateZ(float radian);
#pragma endregion

#pragma region 3軸回転の行列の作成(MakeRotateXYZ)
		/// <summary>
		/// 
		/// </summary>
		/// <param name="radianX"></param>
		/// <param name="radianY"></param>
		/// <param name="radianZ"></param>
		/// <returns></returns>
		static Matrix4x4 RotateXYZ(const float& radianX, const float& radianY, const float& radianZ);
		/// <summary>
		/// 
		/// </summary>
		/// <param name="radianX"></param>
		/// <param name="radianY"></param>
		/// <param name="radianZ"></param>
		/// <returns></returns>
		static Matrix4x4 RotateXYZ(const Vector3& radian);
#pragma endregion

#pragma region 平行移動の行列の作成(MakeTranslate)
		/// <summary>
		/// 
		/// </summary>
		/// <param name="translate"></param>
		/// <returns></returns>
		static Matrix4x4 Translate(const Vector3& translate);
#pragma endregion

#pragma region (MakeOrthoGraphic)
		/// <summary>
		/// 
		/// </summary>
		/// <param name="left"></param>
		/// <param name="top"></param>
		/// <param name="right"></param>
		/// <param name="bottom"></param>
		/// <param name="nearClip"></param>
		/// <param name="farClip"></param>
		/// <returns></returns>
		static Matrix4x4 OrthoGraphic(float left, float top, float right, float bottom, float nearClip, float farClip);
#pragma endregion

#pragma region アフィン変換行列の作成(MakwAffine)
		/// <summary>
		/// アフィン変換行列の作成
		/// </summary>
		/// <param name="scale"></param>
		/// <param name="rotate"></param>
		/// <param name="translate"></param>
		/// <returns></returns>
		static Matrix4x4 Affine(const Vector3& scale, const Vector3& rotate, const Vector3& translate);
#pragma endregion

#pragma region 透視投影行列の作成(MakePerspectiveFov)
		/// <summary>
		/// 透視投影行列の作成
		/// </summary>
		/// <param name="fovY"></param>
		/// <param name="aspectRatio"></param>
		/// <param name="nearClip"></param>
		/// <param name="farClip"></param>
		/// <returns></returns>
		static Matrix4x4 PerspectiveFov(float fovY, float aspectRatio, float nearClip, float farClip);
#pragma endregion

#pragma region ビューポート変換行列(MakeViewport)

		static Matrix4x4 Viewport(float left, float top, float width, float height, float minDepth, float maxDepth);
#pragma endregion

		static Matrix4x4 LookAt(const Vector3& eye, const Vector3& target, const Vector3& up);
		static Matrix4x4 LookAt(const Vector3& eye, const Vector3& target, const Vector3& up,Vector3& xAxis,Vector3& yAxis,Vector3& zAxis);
	};
#pragma endregion

#pragma region 行列の乗算(Multiply)
	/// <summary>
	/// 行列同士の掛け算の値を返す関数
	/// </summary>
	/// <param name="m1"></param>
	/// <param name="m2"></param>
	/// <returns></returns>
	static Matrix4x4 Multiply(const Matrix4x4& m1, const Matrix4x4& m2);
#pragma endregion

#pragma region 逆行列の作成(Inverse)
	/// <summary>
	/// 逆行列の作成
	/// </summary>
	/// <param name="m"></param>
	/// <returns></returns>
	static Matrix4x4 Inverse(const Matrix4x4& m);
#pragma endregion

	static Vector3 Transform(const Vector3& v, const Matrix4x4& m);
	static Vector4 Transform(const Matrix4x4& m, const Vector4& v);

public:
	Matrix4x4 operator+=(const Matrix4x4 M);
};

Matrix4x4 operator*(const Matrix4x4& m1, const Matrix4x4& m2);