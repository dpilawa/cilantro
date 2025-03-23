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

GameObject::GameObject (std::shared_ptr<GameScene> gameScene)
{
    m_parentObject = std::weak_ptr<GameObject> ();
    m_gameScene = gameScene;

    CalculateModelTransformMatrix ();

    // set callbacks on transform modification
    // this is just a passthrough of callbacks to subscribers (Scene)
    m_localTransform.SubscribeHook ("OnUpdateTransform", [&]() 
    {
        CalculateModelTransformMatrix (); 
        GetGameScene ()->GetGame ()->GetMessageBus ()->Publish<TransformUpdateMessage> (std::make_shared<TransformUpdateMessage> (this->GetHandle ()));
    });
}

GameObject::~GameObject ()
{
}

GameObject& GameObject::SetParentObject (const std::string& name)
{
    if (auto s = m_gameScene.lock ())
    {
        auto parent = s->GetGameObjectManager ().GetByName<GameObject> (name);

        m_parentObject = parent;
        parent->m_childObjects.push_back (std::dynamic_pointer_cast<GameObject>(this->GetPointer ()));
        GetGameScene ()->GetGame ()->GetMessageBus ()->Publish<SceneGraphUpdateMessage> (std::make_shared<SceneGraphUpdateMessage> (this->GetHandle ()));
    }
     
    return *this;
}

std::shared_ptr<GameScene> GameObject::GetGameScene ()
{
    return m_gameScene.lock ();
}

std::shared_ptr<GameObject> GameObject::GetParentObject ()
{
    return m_parentObject.lock ();
}

std::vector<std::shared_ptr<GameObject>> GameObject::GetChildObjects ()
{
    return m_childObjects;
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
    return m_localTransform;
}

Matrix4f GameObject::GetModelTransformMatrix () const
{
    return m_modelTransformMatrix;
}

void GameObject::CalculateModelTransformMatrix ()
{
    if (auto p = m_parentObject.lock ())
    {
        m_modelTransformMatrix = p->GetModelTransformMatrix () * m_localTransform.GetTransformMatrix ();
    }
    else
    {
        m_modelTransformMatrix = m_localTransform.GetTransformMatrix ();
    }

    for (auto&& childObject : m_childObjects)
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

