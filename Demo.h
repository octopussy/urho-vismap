#pragma once

#include <Urho3D/Engine/Application.h>
#include <Urho3D/UI/Text.h>
#include "Level.h"

using namespace Urho3D;

class Demo : public Application {
    URHO3D_OBJECT(Demo, Application);

    SharedPtr<Scene> mainScene_;

    SharedPtr<DebugRenderer> debugRenderer_;

    SharedPtr<Node> cameraNode_;

    SharedPtr<Text> debugText_;

    Level* level_;

public:
    Demo(Context* context);

    virtual void Setup();

    virtual void Start();

private:
    void CreateUI();

    void HandleUpdate(StringHash eventType, VariantMap &eventData);

    void Render(StringHash eventType, VariantMap &eventData);

    void HandlePostRenderUpdate(StringHash eventType, VariantMap &eventData);

    void MoveCamera(float step);

    void CreateB2Geometry();
};