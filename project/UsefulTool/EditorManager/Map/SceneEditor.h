#pragma once
#include "../IEditor.h"
#include "SceneEditorCommand.h"

class SceneEditor :
    public BaseEditor<DynamicObject>
{
public:
    SceneEditor() : BaseEditor("Scene Make Editor") {}
    // Editorの基本情報
public:
    void Update()override;
    void DrawUI()override;
private:
    
};