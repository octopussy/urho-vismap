//
// Created by octopussy on 10/12/2016.
//

#include <Urho3D/Scene/Scene.h>
#include <Urho3D/Urho2D/RigidBody2D.h>
#include <Urho3D/Urho2D/CollisionChain2D.h>
#include <Urho3D/IO/Log.h>
#include <Urho3D/Urho2D/PhysicsWorld2D.h>
#include "Level.h"

Level::Level() {
}

void Level::Init(class Scene *scene) {
    scene_ = scene;

    InitRawData();

    Node *geometry = scene->CreateChild("Geometry");

    for (int i = 0; i < rawData.size(); ++i) {
        geometry->CreateComponent<RigidBody2D>();
        CollisionChain2D *chain = geometry->CreateComponent<CollisionChain2D>();
        chain->SetVertices(rawData[i]);
        chain->SetLoop(true);
    }
}

float angle(double x, double y) {
    float angle = (float) atan2(y, x);
    if (angle < 0) angle += 360;
    return angle;
}

float angle(const Vector2 &v) {
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

/** Rotates the Vector2 by the given angle, counter-clockwise assuming the y-axis points up.
 * @param radians the angle in radians */
Vector2 rotatedVectorRad(const Vector2 &v, float radians) {
    float cos = cosf(radians);
    float sin = sinf(radians);
    return Vector2(v.x_ * cos - v.y_ * sin, v.x_ * sin + v.y_ * cos);
}


// TODO: запилить отсечение невидимой геометрии, ну и прочая оптимизация не помешает
void Level::GetVisPoints(const Vector2 &center, std::vector<Vector2> &out) {
    std::vector<Vector2> points;

    PhysicsWorld2D *phWorld = scene_->GetComponent<PhysicsWorld2D>();

    for (int i = 0; i < rawData.size(); ++i) {
        PODVector<Vector2> &v = rawData[i];
        // тут я чето с копированием затупил :( хз как правильно
        for (int j = 0; j < v.Size(); ++j) {
            const Vector2 &value = v[j];
            points.push_back(value);
        }
    }

    std::sort(points.begin(), points.end(), SortByAngleComparator(center));

    PhysicsRaycastResult2D resultRight, result, resultLeft;

    const float MAX_DISTANCE = 1000.f;
    const float ANGLE_BIAS = 0.001f;
    const float DISTANCE_EPSILON = 2.5f;

    for (int i = 0; i < points.size(); ++i) {
        Vector2 point = points[i];
        Vector2 dirToPoint = (point - center);
        float distanceToPoint = dirToPoint.Length();

        dirToPoint.Normalize();

        phWorld->RaycastSingle(result, center, center + dirToPoint * MAX_DISTANCE, M_MAX_UNSIGNED);
        phWorld->RaycastSingle(resultRight, center, center + rotatedVectorRad(dirToPoint, -ANGLE_BIAS) * MAX_DISTANCE);
        phWorld->RaycastSingle(resultLeft, center, center + rotatedVectorRad(dirToPoint, ANGLE_BIAS) * MAX_DISTANCE);

        if (resultRight.body_ != nullptr && resultRight.distance_ > distanceToPoint + DISTANCE_EPSILON) {
            out.push_back(resultRight.position_);
        }

        if (resultRight.body_ != nullptr && fabs(resultRight.distance_ - distanceToPoint) <= DISTANCE_EPSILON
            || result.body_ != nullptr && fabs(result.distance_ - distanceToPoint) <= DISTANCE_EPSILON
            || resultLeft.body_ != nullptr && fabs(resultLeft.distance_ - distanceToPoint) <= DISTANCE_EPSILON) {
            out.push_back(point);
        }

        if (resultLeft.body_ != nullptr && resultLeft.distance_ > distanceToPoint + DISTANCE_EPSILON) {
            out.push_back(resultLeft.position_);
        }

    }

/*
        for (unsigned long i = 0; i < out.size(); ++i) {
            URHO3D_LOGRAWF("%f\n", angle(out.at(i) - center));
        }
*/
}
