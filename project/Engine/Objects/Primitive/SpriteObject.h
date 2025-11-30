#pragma once
#include "ObjectBase.h"

enum class SPRITE_ANCHOR_TYPE {
	LeftTop,
	RightTop,
	LeftBottom,
	RightBottom,
	LeftMiddle,
	RightMiddle,
	Middle
};

enum class SPRITE_VIEW_TYPE {
	UI,
	Object
};

class SpriteObject
	:public ObjectBase
{
public:
	SpriteObject(Fngine* fngine) { fngine_ = fngine; };
	~SpriteObject() = default;
public:
	void Initialize(int textureHandle,SPRITE_ANCHOR_TYPE type = SPRITE_ANCHOR_TYPE::Middle);
	void Draw(SPRITE_VIEW_TYPE type = SPRITE_VIEW_TYPE::UI);
private:
	
public:
	// -----------------------
	// 設定と取得関数
	// -----------------------

	// アンカーポイントについて
	void SetAnchorPoint(const Vector2& point) { anchorPoint_ = point; }
	Vector2 GetAnchorPoint()const { return anchorPoint_; }

	/// <summary>
	/// フリップ
	/// </summary>
	/// <param name="x">Object : false</param>
	/// <param name="y">Object : true</param>
	void SetFlip(bool x, bool y) { isFlipX_ = x;isFlipY_ = y; }

	/// <summary>
	/// 抜き出すサイズの調整
	/// </summary>
	/// <param name="leftTop">抜き出したい画像の左上</param>
	/// <param name="size">抜き出す画像の高さと幅</param>
	void SetTextureSize(const Vector2& leftTop, const Vector2& size);
private:
	void Draw(TheOrderCommand& command, PSO& pso, DirectionLight& light, Texture& tex);
	void Draw2(TheOrderCommand& command, PSO& pso, DirectionLight& light, D3D12_GPU_DESCRIPTOR_HANDLE& tex);
	void SetWVPData(Matrix4x4 WVP);

	void InitializeResource(D3D12System& d3d12);

	void InitializeData();

	void InitializeVertex(float width,float height);

	void InitializeVertex();

private:
	Vector2 anchorPoint_ = { 0.0f,0.0f };
	SPRITE_ANCHOR_TYPE anchorType_;
	bool isFlipX_ = false;
	bool isFlipY_ = false;
	// テクスチャ左上座標
	Vector2 textureLeftTop_ = { 0.0f,0.0f };
	// テクスチャ切り出しサイズ
	Vector2 textureSize_ = { 0.0f,0.0f };
};

