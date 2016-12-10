//
// Created by octopussy on 10/12/2016.
//

#include <Urho3D/Scene/Scene.h>
#include <Urho3D/Urho2D/RigidBody2D.h>
#include <Urho3D/Urho2D/CollisionChain2D.h>
#include <Urho3D/IO/Log.h>
#include "Level.h"

Level::Level() {
}

void Level::Init(class Scene *root) {
    InitRawData();

    Node *geometry = root->CreateChild("Geometry");

    for (int i = 0; i < rawData.size(); ++i) {
        geometry->CreateComponent<RigidBody2D>();
        CollisionChain2D *chain = geometry->CreateComponent<CollisionChain2D>();
        chain->SetVertices(rawData[i]);
        chain->SetLoop(true);
    }
}

static const float radiansToDegrees = 180.0f / M_PI;

float angle (double x, double y) {
    float angle = (float)atan2(y, x) * radiansToDegrees;
    if (angle < 0) angle += 360;
    return angle;
}

float angle(const Vector2& v) {
    return angle(v.x_, v.y_);
}

class SortByAngleComparator {
    const Vector2 &center;

public:
    SortByAngleComparator(const Vector2 &c) : center(c) {}

    inline bool operator()(const Vector2 &l, const Vector2 &r) {
        const Vector2 &v1 = l - center;
        const Vector2 &v2 = r - center;
        float a1 = angle(v1);
        float a2 = angle(v2);
        return a1 < a2;
    }
};


static bool AAAA = false;

void Level::GetVisPoints(const Vector2 &center, std::vector<Vector2> &out) {
    // TODO: запилить отсечение невидимой геометрии
    for (int i = 0; i < rawData.size(); ++i) {
        PODVector<Vector2> &v = rawData[i];
        // тут я чето с копированием затупил :( хз как правильно
        for (int j = 0; j < v.Size(); ++j) {
            const Vector2 &value = v[j];
            out.push_back(value);
        }
    }

    std::sort(out.begin(), out.end(), SortByAngleComparator(center));

/*
    if (!AAAA) {
        for (unsigned long i = 0; i < out.size(); ++i) {
            URHO3D_LOGRAWF("%f\n", angle(out.at(i) - center));
        }
        AAAA = true;
    }
*/
}
