//
// Created by octopussy on 10/12/2016.
//

#pragma once

#include <Urho3D/Math/Vector2.h>
#include <Urho3D/Container/Ptr.h>
#include <vector>

using namespace Urho3D;

class Level {
    std::vector<PODVector<Vector2> > rawData;

    Context *context_;

    Scene *scene_;

    class CustomGeometry *visMapGeometry;

    SharedPtr<Material> renderMaterial;

public:
    Level();

    void Init(class Context* context, class Scene *scene);

    virtual ~Level() {}

    void GetVisPoints(const Vector2 &vector2, std::vector<Vector2>& out);

    void PostRender(DebugRenderer *debugRenderer);

private:

    void InitRawData() {
        PODVector<Vector2> o;
        o.Push(Vector2(-300, -300));
        o.Push(Vector2(300, -300));
        o.Push(Vector2(200, -100));
        o.Push(Vector2(300, 300));
        o.Push(Vector2(-300, 300));
        rawData.push_back(o);

        float r = 65.0f;
        float x = 100.0f;
        float y = 100.0f;
        o = PODVector<Vector2>();
        for (int s = 0; s < 10; ++s) {
            float step =  (s / 10.0f) * M_PI * 2;
            o.Push(Vector2(cosf(step) * r + x, sinf(step) * r + y));
        }
        rawData.push_back(o);

        r = 45.0f;
        x = -100.0f;
        y = -100.0f;
        o = PODVector<Vector2>();
        for (int s = 0; s < 10; ++s) {
            float step =  (s / 10.0f) * M_PI * 2;
            o.Push(Vector2(cosf(step) * r + x, sinf(step) * r + y));
        }
        rawData.push_back(o);
    }
};