#ifndef _GAMEOBJECT_H_
#define _GAMEOBJECT_H_

#include "cilantroengine.h"
#include "resource/Resource.h"
#include "scene/Transform.h"
#include <string>
#include <vector>
#include <memory>

namespace cilantro {

class GameScene;
struct IRenderer;

class __CEAPI GameObject : public Resource, public std::enable_shared_from_this<GameObject>
{
public:
    __EAPI GameObject (std::shared_ptr<GameScene> gameScene);
    __EAPI virtual ~GameObject ();

    // set pointer to parent object (i.e. put current object inside hierarchy)
    __EAPI std::shared_ptr<GameObject> SetParentObject (const std::string& name);

    // get related objects
    __EAPI std::shared_ptr<GameScene> GetGameScene ();
    __EAPI std::shared_ptr<GameObject> GetParentObject ();
    __EAPI std::vector<std::weak_ptr<GameObject>> GetChildren ();

    // invoked by game loop during initializaton
    __EAPI virtual void OnStart ();

    // invoked by game loop on each frame or on update (e.g. transform change)
    __EAPI virtual void OnFrame ();
    __EAPI virtual void OnDraw (IRenderer& renderer);
    __EAPI virtual void OnUpdate (IRenderer& renderer);

    // invoked by game loop during deinitialization
    __EAPI virtual void OnEnd ();

    // get transformation object's reference
    __EAPI std::shared_ptr<Transform> GetModelTransform ();

    // get transformation matrices
    __EAPI Matrix4f GetWorldTransformMatrix () const;
    __EAPI void CalculateWorldTransformMatrix ();

    // get object origin's orientation in world space
    __EAPI Vector4f GetPosition () const;
    __EAPI Vector3f GetScaling () const;
    __EAPI Quaternion GetRotation () const;

    // get object's orthogonal basis vectors
    __EAPI Vector3f GetRight () const;
    __EAPI Vector3f GetUp () const;
    __EAPI Vector3f GetForward () const;	

protected:

    // parent scene
    std::weak_ptr<GameScene> m_gameScene;

    // pointer to parent object (objects may form a hierarchy)
    std::weak_ptr<GameObject> m_parentObject;

private:
    // vector of child objects
    std::vector<std::weak_ptr<GameObject>> m_childObjects;

    // object's transformation in relation its origin
    std::shared_ptr<Transform> m_modelTransform;

    // transform matrix in world space
    Matrix4f m_worldTransformMatrix;
};

} // namespace cilantro

#endif
