//
// Created by octopussy on 10/12/2016.
//

#pragma once

#include <Urho3D/Math/Vector2.h>
#include <Urho3D/Container/Ptr.h>

using namespace Urho3D;

class Level {

    static const int OBJ_COUNT = 3;

    PODVector<Vector2> rawData[OBJ_COUNT];

public:
    Level();

    void Init(class Scene *root);

    virtual ~Level() {}

private:

    void InitRawData() {
        PODVector<Vector2> o = PODVector<Vector2>();
        o.Push(Vector2(-300, -300));
        o.Push(Vector2(300, -300));
        o.Push(Vector2(200, -100));
        o.Push(Vector2(300, 300));
        o.Push(Vector2(-300, 300));
        rawData[0] = o;

        float r = 65.0f;
        float x = 100.0f;
        float y = 100.0f;
        o = PODVector<Vector2>();
        for (int s = 0; s < 10; ++s) {
            float step =  (s / 10.0f) * M_PI * 2;
            o.Push(Vector2(cosf(step) * r + x, sinf(step) * r + y));
        }
        rawData[1] = o;

        r = 45.0f;
        x = -100.0f;
        y = -100.0f;
        o = PODVector<Vector2>();
        for (int s = 0; s < 10; ++s) {
            float step =  (s / 10.0f) * M_PI * 2;
            o.Push(Vector2(cosf(step) * r + x, sinf(step) * r + y));
        }
        rawData[2] = o;
    }
};