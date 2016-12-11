//
// Created by octopussy on 10/12/2016.
//

#include <Urho3D/Core/CoreEvents.h>
#include <Urho3D/Engine/Engine.h>
#include <Urho3D/IO/FileSystem.h>
#include <Urho3D/Input/Input.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/Graphics/Camera.h>
#include <Urho3D/Graphics/Graphics.h>
#include <Urho3D/Graphics/Octree.h>
#include <Urho3D/Graphics/Renderer.h>
#include <Urho3D/Graphics/Model.h>
#include <Urho3D/Graphics/Octree.h>
#include <Urho3D/Graphics/Renderer.h>
#include <Urho3D/Graphics/StaticModel.h>
#include <Urho3D/UI/Font.h>
#include <Urho3D/UI/Text.h>
#include <Urho3D/UI/UI.h>

#include <Urho3D/Scene/Scene.h>
#include <Urho3D/Scene/SceneEvents.h>
#include <Urho3D/Graphics/DebugRenderer.h>

#include <Urho3D/Urho2D/Sprite2D.h>

#include <Urho3D/Urho2D/RigidBody2D.h>
#include <Urho3D/Urho2D/CollisionChain2D.h>
#include <Urho3D/Urho2D/CollisionBox2D.h>
#include <Urho3D/Urho2D/PhysicsWorld2D.h>

#include "Demo.h"

URHO3D_DEFINE_APPLICATION_MAIN(Demo)

Demo::Demo(Context *context) : Application(context) {

}

void Demo::Setup() {
    engineParameters_["WindowTitle"] = "Olololo!!!";
    engineParameters_["LogName"] =
            GetSubsystem<FileSystem>()->GetAppPreferencesDir("urho3d", "logs") + GetTypeName() + ".log";
    engineParameters_["FullScreen"] = false;
    engineParameters_["Headless"] = false;
    engineParameters_["Sound"] = false;
}

void Demo::Start() {
    ResourceCache *cache = GetSubsystem<ResourceCache>();
    Input *input = GetSubsystem<Input>();

    input->SetMouseMode(MM_ABSOLUTE);
    input->SetMouseVisible(true);

    CreateUI();

    mainScene_ = new Scene(context_);
    mainScene_->CreateComponent<Octree>();

    debugRenderer_ = mainScene_->CreateComponent<DebugRenderer>();

    cameraNode_ = mainScene_->CreateChild("Camera");
    cameraNode_->SetPosition(Vector3(0, 0, -10));
    Camera *camera = cameraNode_->CreateComponent<Camera>();
    camera->SetFarClip(100.0f);
    camera->SetOrthographic(true);
    camera->SetOrthoSize((float) 500);

    Node* lightNode = mainScene_->CreateChild("DirectionalLight");
    lightNode->SetDirection(Vector3(0.6f, -1.0f, 0.8f)); // The direction vector does not need to be normalized
    Light* light = lightNode->CreateComponent<Light>();
    light->SetLightType(LIGHT_DIRECTIONAL);

    Renderer *renderer = GetSubsystem<Renderer>();

    SharedPtr<Viewport> viewport(new Viewport(context_, mainScene_, cameraNode_->GetComponent<Camera>()));
    renderer->SetViewport(0, viewport);

    level_ = new Level();
    level_->Init(context_, viewport, mainScene_, cameraNode_->GetComponent<Camera>());

    SubscribeToEvent(E_UPDATE, URHO3D_HANDLER(Demo, HandleUpdate));
    SubscribeToEvent(E_RENDERUPDATE, URHO3D_HANDLER(Demo, Render));
    SubscribeToEvent(E_POSTRENDERUPDATE, URHO3D_HANDLER(Demo, HandlePostRenderUpdate));
 //   UnsubscribeFromEvent(E_SCENEUPDATE);
}

void Demo::CreateB2Geometry() {

}

void Demo::CreateUI() {
    ResourceCache *cache = GetSubsystem<ResourceCache>();
    // Construct new Text object

    Text *text = new Text(context_);
    debugText_ = text;

    // Set font and text color
    debugText_->SetFont(cache->GetResource<Font>("Data/Fonts/BlueHighway.ttf"), 14);
    debugText_->SetColor(Color(1.0f, 1.0f, 1.0f));

    // Align Text center-screen
    debugText_->SetHorizontalAlignment(HA_LEFT);
    debugText_->SetVerticalAlignment(VA_TOP);

    // Add Text instance to the UI root element
    GetSubsystem<UI>()->GetRoot()->AddChild(debugText_);
}

void Demo::HandleUpdate(StringHash eventType, VariantMap &eventData) {
    using namespace Update;

    Graphics *graphics = GetSubsystem<Graphics>();

    // Take the frame time step, which is stored as a float
    float timeStep = eventData[P_TIMESTEP].GetFloat();

    // Move the camera, scale movement with time step
    MoveCamera(timeStep);

    String str;
    const Vector2 &pos = cameraNode_->GetPosition2D();
    str.AppendWithFormat("Pos: %f %f", roundf(pos.x_), roundf(pos.y_));
    debugText_->SetText(str);
}

void Demo::Render(StringHash eventType, VariantMap &eventData) {
}

void Demo::HandlePostRenderUpdate(StringHash eventType, VariantMap &eventData) {
    Graphics *graphics = GetSubsystem<Graphics>();

    const Vector2 &camPosition = cameraNode_->GetPosition2D();

    std::vector<Vector2> points;
    level_->GetVisPoints(camPosition, points);

    for (int i = 0; i < points.size(); ++i) {
        debugRenderer_->AddLine(camPosition, points[i], Color::BLUE);
    }

    level_->PostRender(debugRenderer_);

    debugRenderer_->AddCircle(camPosition, Vector3::FORWARD, 5.0f, Color::MAGENTA);
}

void Demo::MoveCamera(float timeStep) {
    // Do not move if the UI has a focused element (the console)
    if (GetSubsystem<UI>()->GetFocusElement())
        return;

    Input *input = GetSubsystem<Input>();

    // Movement speed as world units per second
    const float MOVE_SPEED = 150.0f;

    // Read WASD keys and move the camera scene node to the corresponding direction if they are pressed
    if (input->GetKeyDown(KEY_W))
        cameraNode_->Translate(Vector3::UP * MOVE_SPEED * timeStep);
    if (input->GetKeyDown(KEY_S))
        cameraNode_->Translate(Vector3::DOWN * MOVE_SPEED * timeStep);
    if (input->GetKeyDown(KEY_A))
        cameraNode_->Translate(Vector3::LEFT * MOVE_SPEED * timeStep);
    if (input->GetKeyDown(KEY_D))
        cameraNode_->Translate(Vector3::RIGHT * MOVE_SPEED * timeStep);

    if (input->GetKeyDown(KEY_PAGEUP)) {
        Camera *camera = cameraNode_->GetComponent<Camera>();
        camera->SetZoom(camera->GetZoom() * 1.01f);
    }

    if (input->GetKeyDown(KEY_PAGEDOWN)) {
        Camera *camera = cameraNode_->GetComponent<Camera>();
        camera->SetZoom(camera->GetZoom() * 0.99f);
    }
}
