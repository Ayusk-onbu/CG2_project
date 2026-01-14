#pragma once
#include "Structures.h"
#include "VertexData.h"

class GJK {
	void Check(const Vector3& vertex1,const Vector3& vertex2);
	/// <summary>
	/// 頂点座標を受け取って任意の方向で一番遠い点を見つける
	/// </summary>
	void Support(VertexData* vertex,const Vector3& d);
};