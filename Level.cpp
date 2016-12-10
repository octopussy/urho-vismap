//
// Created by octopussy on 10/12/2016.
//

#include <Urho3D/Scene/Scene.h>
#include <Urho3D/Urho2D/RigidBody2D.h>
#include <Urho3D/Urho2D/CollisionChain2D.h>
#include <Urho3D/Graphics/Material.h>
#include <Urho3D/Graphics/CustomGeometry.h>
#include <Urho3D/Graphics/Technique.h>
#include <Urho3D/Urho2D/PhysicsWorld2D.h>
#include <Urho3D/Resource/ResourceCache.h>
#include "Level.h"

Level::Level() {
}

void Level::Init(Context* context, class Scene *scene) {
    context_ = context;
    scene_ = scene;

    Node* lightNode = scene_->CreateChild("DirectionalLight");
    lightNode->SetDirection(Vector3(0.0, 0.0f, -0.8f)); // The direction vector does not need to be normalized
    Light* light = lightNode->CreateComponent<Light>();
    light->SetLightType(LIGHT_DIRECTIONAL);

    InitRawData();

    Node *b2geometry = scene->CreateChild("b2Geometry");

    for (int i = 0; i < rawData.size(); ++i) {
        b2geometry->CreateComponent<RigidBody2D>();
        CollisionChain2D *chain = b2geometry->CreateComponent<CollisionChain2D>();
        chain->SetVertices(rawData[i]);
        chain->SetLoop(true);
    }

    Node *visMap = scene->CreateChild("VisMap");
    visMapGeometry = visMap->CreateComponent<CustomGeometry>();
    visMapGeometry->SetDynamic(true);

    ResourceCache* cache = scene_->GetSubsystem<ResourceCache>();
    renderMaterial = new Material(context_);
    renderMaterial->SetTechnique(0, cache->GetResource<Technique>("Techniques/VisMap.xml"));
    renderMaterial->SetCullMode(CullMode::CULL_NONE);
    visMapGeometry->SetMaterial(renderMaterial);
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

    renderMaterial->SetShaderParameter("CenterPos", center);

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

        // иногда, результат трассировки конктретно в точку "point" возвращает пустой результат... не понятно почему
        // возможно из-за особенностей CollisionChain2D, луч какбы проскакивает междуотрезками. Этого наверно не случится,
        // если использовать классические шейпы, типа box, circle или convex-лабуда. Ну или использовать свои алгоритмы
        // кастинга и забить на box2d
        if (resultRight.body_ != nullptr && fabs(resultRight.distance_ - distanceToPoint) <= DISTANCE_EPSILON
            || result.body_ != nullptr && fabs(result.distance_ - distanceToPoint) <= DISTANCE_EPSILON
            || resultLeft.body_ != nullptr && fabs(resultLeft.distance_ - distanceToPoint) <= DISTANCE_EPSILON) {
            out.push_back(point);
        }

        if (resultLeft.body_ != nullptr && resultLeft.distance_ > distanceToPoint + DISTANCE_EPSILON) {
            out.push_back(resultLeft.position_);
        }

    }

    ResourceCache* cache = scene_->GetSubsystem<ResourceCache>();
    visMapGeometry->BeginGeometry(0, TRIANGLE_FAN);
    visMapGeometry->SetDynamic(true);
    visMapGeometry->DefineVertex(center);
    //visMapGeometry->DefineColor(Color::CYAN);

    for (int i = 0; i < out.size(); ++i) {
        visMapGeometry->DefineVertex(out.at((unsigned long) i));
       // visMapGeometry->DefineTexCoord(Vector2::ZERO);
        //visMapGeometry->DefineColor(Color::CYAN);
    }

    visMapGeometry->DefineVertex(out.front());

    visMapGeometry->Commit();
/*
        for (unsigned long i = 0; i < out.size(); ++i) {
            URHO3D_LOGRAWF("%f\n", angle(out.at(i) - center));
        }
*/
}

void Level::PostRender(DebugRenderer* debugRenderer) {
    //visMapGeometry->DrawDebugGeometry(debugRenderer, false);
}
