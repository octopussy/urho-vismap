//
// Created by octopussy on 10/12/2016.
//

#include <Urho3D/Scene/Scene.h>
#include <Urho3D/Urho2D/RigidBody2D.h>
#include <Urho3D/Urho2D/CollisionChain2D.h>
#include "Level.h"

Level::Level() {
}

void Level::Init(class Scene *root) {
    InitRawData();

    Node *geometry = root->CreateChild("Geometry");

    for (int i = 0; i < OBJ_COUNT; ++i) {
        geometry->CreateComponent<RigidBody2D>();
        CollisionChain2D *chain = geometry->CreateComponent<CollisionChain2D>();
        chain->SetVertices(rawData[i]);
        chain->SetLoop(true);
    }
}
