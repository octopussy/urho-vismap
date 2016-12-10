//
// Created by octopussy on 10/12/2016.
//

#include <Urho3D/Engine/Engine.h>
#include <Urho3D/IO/FileSystem.h>
#include <Urho3D/Input/Input.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/UI/Font.h>
#include <Urho3D/UI/Text.h>
#include <Urho3D/UI/UI.h>

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
    Input *input = GetSubsystem<Input>();

    input->SetMouseMode(MouseMode::MM_ABSOLUTE);
    input->SetMouseVisible(true);

    CreateUI();

}

void Demo::CreateUI() const {
    ResourceCache *cache = GetSubsystem();
    // Construct new Text object
    SharedPtr<Text> helloText(new Text(context_));

    // Set String to display
    helloText->SetText("Hello World from Urho3D!");

    // Set font and text color
    helloText->SetFont(cache->GetResource<Font>("Data/Fonts/BlueHighway.ttf"), 30);
    helloText->SetColor(Color(0.0f, 1.0f, 0.0f));

    // Align Text center-screen
    helloText->SetHorizontalAlignment(HA_CENTER);
    helloText->SetVerticalAlignment(VA_CENTER);

    // Add Text instance to the UI root element
    GetSubsystem()->GetRoot()->AddChild(helloText);
}


