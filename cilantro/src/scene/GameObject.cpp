#include "cilantroengine.h"
#include "math/Matrix4f.h"
#include "math/Vector4f.h"
#include "math/Mathf.h"
#include "graphics/Renderer.h"
#include "scene/GameScene.h"
#include "scene/GameObject.h"
#include "scene/Transform.h"
#include "util/CallbackProvider.h"
#include <string>

GameObject::GameObject ()
{
    parentObject = nullptr;
    isLight = false;

    CalculateModelTransformMatrix ();

    // set callbacks on transform modification
    // this is just a passthrough of callbacks to subscribers (Scene)
    modelTransform.RegisterCallback ("OnUpdateTransform", [&](unsigned int objectHandle) { InvokeCallbacks ("OnUpdateTransform", this->GetHandle ()); });
}

GameObject::~GameObject ()
{
}

void GameObject::SetGameLoop (GameLoop* gameLoop)
{
    this->gameLoop = gameLoop;
}

void GameObject::SetHandle (unsigned int handle)
{
    objectHandle = handle;
}

unsigned int GameObject::GetHandle () const
{
    return objectHandle;
}

void GameObject::SetParentObject (GameObject& parent)
{
    parentObject = &parent;
    parent.childObjects.push_back (this);
    InvokeCallbacks ("OnUpdateSceneGraph", this->GetHandle ());
}

GameObject* GameObject::GetParentObject ()
{
    return parentObject;
}

std::vector<GameObject*> GameObject::GetChildObjects ()
{
    return childObjects;
}

void GameObject::OnStart ()
{
}

void GameObject::OnFrame ()
{
}

void GameObject::OnDraw (Renderer& renreder)
{
}

void GameObject::OnUpdate (Renderer& renreder)
{
}

void GameObject::OnEnd ()
{
}

Transform& GameObject::GetModelTransform ()
{
    return modelTransform;
}

Matrix4f GameObject::GetModelTransformMatrix ()
{
    return modelTransformMatrix;
}

void GameObject::CalculateModelTransformMatrix ()
{
    if (parentObject != nullptr)
    {
        modelTransformMatrix = parentObject->GetModelTransformMatrix () * modelTransform.GetModelMatrix ();
    }
    else
    {
        modelTransformMatrix = modelTransform.GetModelMatrix ();
    }

    for (auto&& childObject : childObjects)
    {
        childObject->CalculateModelTransformMatrix ();
    }
}

Vector4f GameObject::GetPosition ()
{
    return GetModelTransformMatrix () * Vector4f (0.0f, 0.0f, 0.0f, 1.0f);
}

Vector3f GameObject::GetRight ()
{
    Matrix4f modelTransforMatrix = GetModelTransformMatrix ();

    return Mathf::Normalize (Vector3f (modelTransforMatrix[0][0], modelTransforMatrix[1][0], modelTransforMatrix[2][0]));
}

Vector3f GameObject::GetUp ()
{
    Matrix4f modelTransforMatrix = GetModelTransformMatrix ();

    return Mathf::Normalize (Vector3f (modelTransforMatrix[0][1], modelTransforMatrix[1][1], modelTransforMatrix[2][1]));
}

Vector3f GameObject::GetForward ()
{
    Matrix4f modelTransforMatrix = GetModelTransformMatrix ();

    return Mathf::Normalize (Vector3f (modelTransforMatrix[0][2], modelTransforMatrix[1][2], modelTransforMatrix[2][2]));
}

bool GameObject::IsLight ()
{
    return isLight;
}