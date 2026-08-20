#pragma once
#include "Engine/Objects/DrawManager.h"
#include "TextureManager.h"
#include "WorldTransform.h"

class DynamicObject;

// --- 描画データのインターフェース ---
class IRenderProvider {
public:
    virtual ~IRenderProvider() = default;
    virtual void SendToDrawManager(DynamicObject* owner) = 0;
    virtual void DrawUI() {}
};

class GrassRenderProvider : public IRenderProvider {
public:
    GrassObjectData data;

    void SendToDrawManager(DynamicObject* owner) override;

    void DrawUI() override;
};

class CharacterRenderProvider : public IRenderProvider {
public:
    CharacterObjectData data;

    void SendToDrawManager(DynamicObject* owner) override;

    void DrawUI() override;
};