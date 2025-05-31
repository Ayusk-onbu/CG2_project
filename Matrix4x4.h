#pragma once

class Matrix4x4 {
public:
	float m[4][4];
public:
#pragma region 単位化行列の関数(Identity)
	/// <summary>
	/// 単位化行列を返す関数
	/// </summary>
	/// <returns></returns>
	Matrix4x4 Identity() {
		Matrix4x4 ret = 
		{1,0,0,0,
		 0,1,0,0,
		 0,0,1,0,
		 0,0,0,1
		};return ret;
	}
#pragma endregion
};