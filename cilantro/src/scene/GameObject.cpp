#include "cilantroengine.h"
#include "math/Matrix4f.h"
#include "math/Vector4f.h"
#include "math/Mathf.h"
#include "graphics/Renderer.h"
#include "scene/GameScene.h"
#include "scene/GameObject.h"
#include "scene/Transform.h"
#include "system/Game.h"
#include "system/HookProvider.h"
#include <string>

namespace cilantro {

GameObject::GameObject (GameScene* gameScene)
{
    parentObject = nullptr;
    this->gameScene = gameScene;

    CalculateModelTransformMatrix ();

    // set callbacks on transform modification
    // this is just a passthrough of callbacks to subscribers (Scene)
    localTransform.SubscribeHook ("OnUpdateTransform", [&]() 
    {
        CalculateModelTransformMatrix (); 
        Game::GetMessageBus ().Publish<TransformUpdateMessage> (std::make_shared<TransformUpdateMessage> (this->GetHandle ()));
    });
}

GameObject::~GameObject ()
{
}

GameObject& GameObject::SetParentObject (const std::string& name)
{
    GameObject& parent = gameScene->GetGameObjectManager ().GetByName<GameObject> (name);

    parentObject = &parent;
    parent.childObjects.push_back (this);
    Game::GetMessageBus ().Publish<SceneGraphUpdateMessage> (std::make_shared<SceneGraphUpdateMessage> (this->GetHandle ()));

    return *this;
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

void GameObject::OnDraw (IRenderer& renderer)
{
}

void GameObject::OnUpdate (IRenderer& renderer)
{
}

void GameObject::OnEnd ()
{
}

Transform& GameObject::GetLocalTransform ()
{
    return localTransform;
}

Matrix4f GameObject::GetModelTransformMatrix () const
{
    return modelTransformMatrix;
}

void GameObject::CalculateModelTransformMatrix ()
{
    if (parentObject != nullptr)
    {
        modelTransformMatrix = parentObject->GetModelTransformMatrix () * localTransform.GetTransformMatrix ();
    }
    else
    {
        modelTransformMatrix = localTransform.GetTransformMatrix ();
    }

    for (auto&& childObject : childObjects)
    {
        childObject->CalculateModelTransformMatrix ();
    }
}

Vector4f GameObject::GetPosition () const
{
    return GetModelTransformMatrix () * Vector4f (0.0f, 0.0f, 0.0f, 1.0f);
}

Vector3f GameObject::GetScaling () const
{
    Matrix4f m = GetModelTransformMatrix ();

    float sx = Mathf::Length (Vector3f (m[0][0], m[1][0], m[2][0]));
    float sy = Mathf::Length (Vector3f (m[0][1], m[1][1], m[2][1]));
    float sz = Mathf::Length (Vector3f (m[0][2], m[1][2], m[2][2]));

    return Vector3f (sx, sy, sz);
}

Quaternion GameObject::GetRotation () const
{
    Matrix4f m = GetModelTransformMatrix ();

    /* convert to pure rotation matrix */
    m[3][0] = 0.0f;
    m[3][1] = 0.0f;
    m[3][2] = 0.0f;

    float sx = Mathf::Length (Vector3f (m[0][0], m[1][0], m[2][0]));
    float sy = Mathf::Length (Vector3f (m[0][1], m[1][1], m[2][1]));
    float sz = Mathf::Length (Vector3f (m[0][2], m[1][2], m[2][2]));

    m[0][0] /= sx;
    m[1][0] /= sx;
    m[2][0] /= sx;

    m[0][1] /= sy;
    m[1][1] /= sy;
    m[2][1] /= sy;

    m[0][1] /= sz;
    m[1][1] /= sz;
    m[2][1] /= sz;

    /* extract quaternion from matrix */
    return Mathf::GenQuaternion (m);
}

Vector3f GameObject::GetRight () const
{
    Matrix4f modelTransforMatrix = GetModelTransformMatrix ();

    return Mathf::Normalize (Vector3f (modelTransforMatrix[0][0], modelTransforMatrix[1][0], modelTransforMatrix[2][0]));
}

Vector3f GameObject::GetUp () const
{
    Matrix4f modelTransforMatrix = GetModelTransformMatrix ();

    return Mathf::Normalize (Vector3f (modelTransforMatrix[0][1], modelTransforMatrix[1][1], modelTransforMatrix[2][1]));
}

Vector3f GameObject::GetForward () const
{
    Matrix4f modelTransforMatrix = GetModelTransformMatrix ();

    return Mathf::Normalize (Vector3f (modelTransforMatrix[0][2], modelTransforMatrix[1][2], modelTransforMatrix[2][2]));
}

} // namespace cilantro

