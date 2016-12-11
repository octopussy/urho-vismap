#include <Urho3D/Scene/Scene.h>
#include <Urho3D/Urho2D/RigidBody2D.h>
#include <Urho3D/Urho2D/CollisionChain2D.h>
#include <Urho3D/Graphics/Material.h>
#include <Urho3D/Graphics/RenderPath.h>
#include <Urho3D/Graphics/Texture2D.h>
#include <Urho3D/Graphics/CustomGeometry.h>
#include <Urho3D/Graphics/Technique.h>
#include <Urho3D/Urho2D/PhysicsWorld2D.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/Resource/XMLFile.h>
#include <Urho3D/Graphics/Graphics.h>
#include <Urho3D/Graphics/Octree.h>
#include <Urho3D/Graphics/Renderer.h>
#include "Level.h"

Level::Level() {
}

void Level::Init(Context *context, Viewport* viewport, Scene *mainScene, Camera *camera) {
    context_ = context;
    rttScene_ = new Scene(context);
    rttScene_->CreateComponent<Octree>();
    b2world_ = rttScene_->CreateComponent<PhysicsWorld2D>();

    InitRawData();

    Node *b2geometry = rttScene_->CreateChild("b2Geometry");

    for (int i = 0; i < rawData.size(); ++i) {
        b2geometry->CreateComponent<RigidBody2D>();
        CollisionChain2D *chain = b2geometry->CreateComponent<CollisionChain2D>();
        chain->SetVertices(rawData[i]);
        chain->SetLoop(true);
    }

    ResourceCache *cache = rttScene_->GetSubsystem<ResourceCache>();
    pass1Material_ = new Material(context_);
    pass1Material_->SetTechnique(0, cache->GetResource<Technique>("Techniques/VisMap.xml"));
    pass1Material_->SetCullMode(CULL_NONE);

    Node *rttVisGeom = rttScene_->CreateChild("rttVisGeometry");
    visMapGeometry_ = rttVisGeom->CreateComponent<CustomGeometry>();
    visMapGeometry_->SetDynamic(true);
    visMapGeometry_->SetMaterial(pass1Material_);

    SharedPtr<Texture2D> renderTexture(new Texture2D(context_));
    renderTexture->SetName("RTVisMap");
    cache->AddManualResource(renderTexture);

    renderTexture->SetSize(1024, 768, Graphics::GetRGBFormat(), TEXTURE_RENDERTARGET);
    renderTexture->SetFilterMode(FILTER_BILINEAR);

    RenderSurface *surface = renderTexture->GetRenderSurface();
    surface->SetUpdateMode(SURFACE_UPDATEALWAYS);

    SharedPtr<Viewport> rttViewport(new Viewport(context_, rttScene_, camera));
    surface->SetViewport(0, rttViewport);

    Renderer *renderer = rttScene_->GetSubsystem<Renderer>();
    //renderer->SetViewport(0, rttViewport);

    SharedPtr<RenderPath> effectRenderPath = viewport->GetRenderPath()->Clone();
    effectRenderPath->Append(cache->GetResource<XMLFile>("PostProcess/VisMap.xml"));

    viewport->SetRenderPath(effectRenderPath);
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

    PhysicsWorld2D *phWorld = rttScene_->GetComponent<PhysicsWorld2D>();

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
    const float ANGLE_BIAS = 0.00001f;
    const float DISTANCE_EPSILON = 0.5f;

    pass1Material_->SetShaderParameter("CenterPos", center);
    pass1Material_->SetShaderParameter("VisMapShift", 10.0f);

    for (int i = 0; i < points.size(); ++i) {
        Vector2 point = points[i];
        Vector2 dirToPoint = (point - center);
        float distanceToPoint = dirToPoint.Length();

        dirToPoint.Normalize();

        phWorld->RaycastSingle(resultRight, center, center + rotatedVectorRad(dirToPoint, -ANGLE_BIAS) * MAX_DISTANCE);
        phWorld->RaycastSingle(resultLeft, center, center + rotatedVectorRad(dirToPoint, ANGLE_BIAS) * MAX_DISTANCE);

        bool addRightPoint = resultRight.distance_ >= distanceToPoint - DISTANCE_EPSILON;
        bool addLeftPoint = resultLeft.distance_ >= distanceToPoint - DISTANCE_EPSILON;

        if (addRightPoint) {
            out.push_back(resultRight.position_);
        }

        if (addLeftPoint) {
            out.push_back(resultLeft.position_);
        }
    }


    ResourceCache *cache = rttScene_->GetSubsystem<ResourceCache>();
    visMapGeometry_->BeginGeometry(0, TRIANGLE_FAN);
    visMapGeometry_->SetDynamic(true);
    visMapGeometry_->DefineVertex(center);
    //visMapGeometry->DefineColor(Color::CYAN);

    for (int i = 0; i < out.size(); ++i) {
        visMapGeometry_->DefineVertex(out.at((unsigned long) i));
        // visMapGeometry->DefineTexCoord(Vector2::ZERO);
        //visMapGeometry->DefineColor(Color::CYAN);
    }

    if (out.size() > 0) {
        visMapGeometry_->DefineVertex(out.front());
    }

    visMapGeometry_->Commit();
}

void Level::PostRender(DebugRenderer *debugRenderer) {
    b2world_->SetDrawShape(true);
    b2world_->DrawDebugGeometry();

    Graphics *graphics = rttScene_->GetSubsystem<Graphics>();
    //visMapGeometry->DrawDebugGeometry(debugRenderer, false);
}
