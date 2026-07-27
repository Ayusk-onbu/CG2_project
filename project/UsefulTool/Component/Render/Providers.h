#pragma once
#include "Engine/Objects/DrawManager.h"
#include "WorldTransform.h"

class DynamicObject;

// --- 描画データのインターフェース ---
class IRenderProvider {
public:
    virtual ~IRenderProvider() = default;
    virtual void SendToDrawManager(const WorldTransform& transform) = 0;
};

class GrassRenderProvider : public IRenderProvider {
public:
    GrassObjectData data;

    void SendToDrawManager(const WorldTransform& transform) override {
        data.worldTransform = transform;
        DrawManager::GetInstance()->GetGrass()->AddInstance(data);
    }
};