#pragma once
#include "Trigonometric.h"
#include "Vector3.h"

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
		static Matrix4x4 Identity() {
			Matrix4x4 ret =
			{ 1,0,0,0,
			 0,1,0,0,
			 0,0,1,0,
			 0,0,0,1
			};return ret;
		}
#pragma endregion

#pragma region Scaleの行列の生成(MakeScale)
		/// <summary>
		/// Scaleの行列を作成する関数
		/// </summary>
		/// <param name="scale"></param>
		/// <returns></returns>
		static Matrix4x4 Scale(const Vector3& scale) {
			Matrix4x4 ret = {
				scale.x,0,0,0,
				0,scale.y,0,0,
				0,0,scale.z,0,
				0,0,0,1
			};
			return ret;
		}
#pragma endregion

#pragma region X軸回転の行列の作成(MakeRotateX)
		/// <summary>
		/// 
		/// </summary>
		/// <param name="radian"></param>
		/// <returns></returns>
		static Matrix4x4 RotateX(float radian) {
			Matrix4x4 ret = {
				1,0,0,0,
				0,std::cos(radian),std::sin(radian),0,
				0,-std::sin(radian),std::cos(radian),0,
				0,0,0,1
			};
			return ret;
		}
#pragma endregion

#pragma region Y軸回転の行列の作成(MakeRotateY)
		/// <summary>
		/// 
		/// </summary>
		/// <param name="radian"></param>
		/// <returns></returns>
		static Matrix4x4 RotateY(float radian) {
			Matrix4x4 ret = {
				std::cos(radian),0,-std::sin(radian),0,
				0,1,0,0,
				std::sin(radian),0,std::cos(radian),0,
				0,0,0,1
			};
			return ret;
		}
#pragma endregion

#pragma region Z軸回転の行列の作成(MakeRotateZ)
		/// <summary>
		/// 
		/// </summary>
		/// <param name="radian"></param>
		/// <returns></returns>
		static Matrix4x4 RotateZ(float radian) {
			Matrix4x4 ret = {
				std::cos(radian),std::sin(radian),0,0,
				-std::sin(radian),std::cos(radian),0,0,
				0,0,1,0,
				0,0,0,1
			};
			return ret;
		}
#pragma endregion

#pragma region 3軸回転の行列の作成(MakeRotateXYZ)
		/// <summary>
		/// 
		/// </summary>
		/// <param name="radianX"></param>
		/// <param name="radianY"></param>
		/// <param name="radianZ"></param>
		/// <returns></returns>
		static Matrix4x4 RotateXYZ(const float& radianX, const float& radianY, const float& radianZ) {
			Matrix4x4 ret;
			ret = Multiply(Make::RotateX(radianX), Multiply(Make::RotateY(radianY), Make::RotateZ(radianZ)));
			return ret;
		}
		/// <summary>
		/// 
		/// </summary>
		/// <param name="radianX"></param>
		/// <param name="radianY"></param>
		/// <param name="radianZ"></param>
		/// <returns></returns>
		static Matrix4x4 RotateXYZ(const Vector3& radian) {
			Matrix4x4 ret;
			ret = Multiply(Make::RotateX(radian.x), Multiply(Make::RotateY(radian.y), Make::RotateZ(radian.z)));
			return ret;
		}
#pragma endregion

#pragma region 平行移動の行列の作成(MakeTranslate)
		/// <summary>
		/// 
		/// </summary>
		/// <param name="translate"></param>
		/// <returns></returns>
		static Matrix4x4 Translate(const Vector3& translate) {
			Matrix4x4 ret = {
				1,0,0,0,
				0,1,0,0,
				0,0,1,0,
				translate.x,translate.y,translate.z,1
			};
			return ret;
		}
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
		static Matrix4x4 OrthoGraphic(float left, float top, float right, float bottom, float nearClip, float farClip) {
			Matrix4x4 ret;
			ret.m[0][0] = 2.0f / (right - left);
			ret.m[0][1] = 0;
			ret.m[0][2] = 0;
			ret.m[0][3] = 0;

			ret.m[1][0] = 0;
			ret.m[1][1] = 2.0f / (top - bottom);
			ret.m[1][2] = 0;
			ret.m[1][3] = 0;

			ret.m[2][0] = 0;
			ret.m[2][1] = 0;
			ret.m[2][2] = 1.0f / (farClip - nearClip);
			ret.m[2][3] = 0;

			ret.m[3][0] = (right + left) / (left - right);
			ret.m[3][1] = (top + bottom) / (bottom - top);
			ret.m[3][2] = nearClip / (nearClip - farClip);
			ret.m[3][3] = 1.0f;
			return ret;
		}
#pragma endregion

#pragma region アフィン変換行列の作成(MakwAffine)
		/// <summary>
		/// アフィン変換行列の作成
		/// </summary>
		/// <param name="scale"></param>
		/// <param name="rotate"></param>
		/// <param name="translate"></param>
		/// <returns></returns>
		static Matrix4x4 Affine(const Vector3& scale, const Vector3& rotate, const Vector3& translate) {
			//SRT
			// - [ S ] - //
			Matrix4x4 SRTMatrix = Multiply(Make::Scale(scale), Make::RotateXYZ(rotate));
			SRTMatrix = Multiply(SRTMatrix, Make::Translate(translate));
			return SRTMatrix;
		}
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
		static Matrix4x4 PerspectiveFov(float fovY, float aspectRatio, float nearClip, float farClip) {
			Matrix4x4 ret = {
				cot(fovY / 2.0f) / aspectRatio, 0, 0, 0,
				0, cot(fovY / 2.0f), 0, 0,
				0, 0, farClip / (farClip - nearClip), 1,
				0, 0, (-nearClip * farClip) / (farClip - nearClip), 0
			};
			return ret;
		}
#pragma endregion

#pragma region ビューポート変換行列(MakeViewport)
		
		static Matrix4x4 Viewport(float left, float top, float width, float height, float minDepth, float maxDepth) {
			Matrix4x4 ret = {
				width / 2.0f, 0, 0, 0,
				0, -height / 2.0f, 0, 0,
				0, 0, maxDepth - minDepth, 0,
				left + width / 2.0f, top + height / 2.0f, minDepth, 1
			};
			return ret;
		}
#pragma endregion
	};
#pragma endregion

#pragma region 行列の乗算(Multiply)
	/// <summary>
	/// 行列同士の掛け算の値を返す関数
	/// </summary>
	/// <param name="m1"></param>
	/// <param name="m2"></param>
	/// <returns></returns>
	static Matrix4x4 Multiply(const Matrix4x4& m1, const Matrix4x4 m2) {
		float a11 = m1.m[0][0], a12 = m1.m[0][1], a13 = m1.m[0][2], a14 = m1.m[0][3],
			a21 = m1.m[1][0], a22 = m1.m[1][1], a23 = m1.m[1][2], a24 = m1.m[1][3],
			a31 = m1.m[2][0], a32 = m1.m[2][1], a33 = m1.m[2][2], a34 = m1.m[2][3],
			a41 = m1.m[3][0], a42 = m1.m[3][1], a43 = m1.m[3][2], a44 = m1.m[3][3];
		float b11 = m2.m[0][0], b12 = m2.m[0][1], b13 = m2.m[0][2], b14 = m2.m[0][3],
			b21 = m2.m[1][0], b22 = m2.m[1][1], b23 = m2.m[1][2], b24 = m2.m[1][3],
			b31 = m2.m[2][0], b32 = m2.m[2][1], b33 = m2.m[2][2], b34 = m2.m[2][3],
			b41 = m2.m[3][0], b42 = m2.m[3][1], b43 = m2.m[3][2], b44 = m2.m[3][3];
		Matrix4x4 ret = {
			a11 * b11 + a12 * b21 + a13 * b31 + a14 * b41,  a11 * b12 + a12 * b22 + a13 * b32 + a14 * b42,  a11 * b13 + a12 * b23 + a13 * b33 + a14 * b43,  a11 * b14 + a12 * b24 + a13 * b34 + a14 * b44,
			a21 * b11 + a22 * b21 + a23 * b31 + a24 * b41,  a21 * b12 + a22 * b22 + a23 * b32 + a24 * b42,  a21 * b13 + a22 * b23 + a23 * b33 + a24 * b43,  a21 * b14 + a22 * b24 + a23 * b34 + a24 * b44,
			a31 * b11 + a32 * b21 + a33 * b31 + a34 * b41,  a31 * b12 + a32 * b22 + a33 * b32 + a34 * b42,  a31 * b13 + a32 * b23 + a33 * b33 + a34 * b43,  a31 * b14 + a32 * b24 + a33 * b34 + a34 * b44,
			a41 * b11 + a42 * b21 + a43 * b31 + a44 * b41,  a41 * b12 + a42 * b22 + a43 * b32 + a44 * b42,  a41 * b13 + a42 * b23 + a43 * b33 + a44 * b43,  a41 * b14 + a42 * b24 + a43 * b34 + a44 * b44
		};
		return ret;
	}
#pragma endregion

#pragma region 逆行列の作成(Inverse)
	/// <summary>
	/// 逆行列の作成
	/// </summary>
	/// <param name="m"></param>
	/// <returns></returns>
	static Matrix4x4 Inverse(const Matrix4x4& m) {
		float a11 = m.m[0][0], a12 = m.m[0][1], a13 = m.m[0][2], a14 = m.m[0][3],
			a21 = m.m[1][0], a22 = m.m[1][1], a23 = m.m[1][2], a24 = m.m[1][3],
			a31 = m.m[2][0], a32 = m.m[2][1], a33 = m.m[2][2], a34 = m.m[2][3],
			a41 = m.m[3][0], a42 = m.m[3][1], a43 = m.m[3][2], a44 = m.m[3][3];
		float A = a11 * a22 * a33 * a44 + a11 * a23 * a34 * a42 + a11 * a24 * a32 * a43
			- a11 * a24 * a33 * a42 - a11 * a23 * a32 * a44 - a11 * a22 * a34 * a43
			- a12 * a21 * a33 * a44 - a13 * a21 * a34 * a42 - a14 * a21 * a32 * a43
			+ a14 * a21 * a33 * a42 + a13 * a21 * a32 * a44 + a12 * a21 * a34 * a43
			+ a12 * a23 * a31 * a44 + a13 * a24 * a31 * a42 + a14 * a22 * a31 * a43
			- a14 * a23 * a31 * a42 - a13 * a22 * a31 * a44 - a12 * a24 * a31 * a43
			- a12 * a23 * a34 * a41 - a13 * a24 * a32 * a41 - a14 * a22 * a33 * a41
			+ a14 * a23 * a32 * a41 + a13 * a22 * a34 * a41 + a12 * a24 * a33 * a41;
		Matrix4x4 ret = {
			(a22 * a33 * a44 + a23 * a34 * a42 + a24 * a32 * a43 - a24 * a33 * a42 - a23 * a32 * a44 - a22 * a34 * a43) / A,
			(-a12 * a33 * a44 - a13 * a34 * a42 - a14 * a32 * a43 + a14 * a33 * a42 + a13 * a32 * a44 + a12 * a34 * a43) / A,
			(a12 * a23 * a44 + a13 * a24 * a42 + a14 * a22 * a43 - a14 * a23 * a42 - a13 * a22 * a44 - a12 * a24 * a43) / A,
			(-a12 * a23 * a34 - a13 * a24 * a32 - a14 * a22 * a33 + a14 * a23 * a32 + a13 * a22 * a34 + a12 * a24 * a33) / A,

			(-a21 * a33 * a44 - a23 * a34 * a41 - a24 * a31 * a43 + a24 * a33 * a41 + a23 * a31 * a44 + a21 * a34 * a43) / A,
			(a11 * a33 * a44 + a13 * a34 * a41 + a14 * a31 * a43 - a14 * a33 * a41 - a13 * a31 * a44 - a11 * a34 * a43) / A,
			(-a11 * a23 * a44 - a13 * a24 * a41 - a14 * a21 * a43 + a14 * a23 * a41 + a13 * a21 * a44 + a11 * a24 * a43) / A,
			(a11 * a23 * a34 + a13 * a24 * a31 + a14 * a21 * a33 - a14 * a23 * a31 - a13 * a21 * a34 - a11 * a24 * a33) / A,

			(a21 * a32 * a44 + a22 * a34 * a41 + a24 * a31 * a42 - a24 * a32 * a41 - a22 * a31 * a44 - a21 * a34 * a42) / A,
			(-a11 * a32 * a44 - a12 * a34 * a41 - a14 * a31 * a42 + a14 * a32 * a41 + a12 * a31 * a44 + a11 * a34 * a42) / A,
			(a11 * a22 * a44 + a12 * a24 * a41 + a14 * a21 * a42 - a14 * a22 * a41 - a12 * a21 * a44 - a11 * a24 * a42) / A,
			(-a11 * a22 * a34 - a12 * a24 * a31 - a14 * a21 * a32 + a14 * a22 * a31 + a12 * a21 * a34 + a11 * a24 * a32) / A,

			(-a21 * a32 * a43 - a22 * a33 * a41 - a23 * a31 * a42 + a23 * a32 * a41 + a22 * a31 * a43 + a21 * a33 * a42) / A,
			(a11 * a32 * a43 + a12 * a33 * a41 + a13 * a31 * a42 - a13 * a32 * a41 - a12 * a31 * a43 - a11 * a33 * a42) / A,
			(-a11 * a22 * a43 - a12 * a23 * a41 - a13 * a21 * a42 + a13 * a22 * a41 + a12 * a21 * a43 + a11 * a23 * a42) / A,
			(a11 * a22 * a33 + a12 * a23 * a31 + a13 * a21 * a32 - a13 * a22 * a31 - a12 * a21 * a33 - a11 * a23 * a32) / A
		};
		return ret;
	}
#pragma endregion
};