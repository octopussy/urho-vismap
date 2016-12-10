#pragma once

#include <Urho3D/Engine/Application.h>

using namespace Urho3D;

class Demo : public Application {
    URHO3D_OBJECT(Demo, Application);

public:
    Demo(Context* context);

    void Setup() override;

    void Start() override;

private:
    void CreateUI() const;
};