#include "RenderComponent.h"
#include "DynamicObject.h"

void RenderComponent::Draw() {
    if (master_ && provider_) {
        provider_->SendToDrawManager(master_->GetTransform());
    }
}