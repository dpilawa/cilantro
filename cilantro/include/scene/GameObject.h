#ifndef _GAMEOBJECT_H_
#define _GAMEOBJECT_H_

#include "cilantroengine.h"
#include "game/GameLoop.h"
#include "scene/Transform.h"
#include "util/CallbackProvider.h"
#include <string>
#include <vector>

class GameScene;
class Renderer;

class GameObject : public CallbackProvider<std::string, unsigned int>
{
public:
    __EAPI GameObject ();
    __EAPI virtual ~GameObject ();

    // attach to game loop
    __EAPI void SetGameLoop (GameLoop* gameLoop);

    // handle  operations
    __EAPI void SetHandle (unsigned int handle);
    __EAPI unsigned int GetHandle () const;

    // set pointer to parent object (i.e. put current object inside hierarchy)
    __EAPI void SetParentObject (GameObject& parent);

    // get related objects
    __EAPI GameObject* GetParentObject ();
    __EAPI std::vector<GameObject*> GetChildObjects ();

    // invoked by game loop during initializaton
    __EAPI virtual void OnStart ();

    // invoked by game loop on each frame or on update (e.g. transform change)
    __EAPI virtual void OnFrame ();
    __EAPI virtual void OnDraw (Renderer& renderer);
    __EAPI virtual void OnUpdate (Renderer &renderer);

    // invoked by game loop during deinitialization
    __EAPI virtual void OnEnd ();

    // get transformation object's reference
    // this function is recursive and concatenates world-space
    // transformation matrices of parent objects
    __EAPI Transform& GetModelTransform ();

    // get transformation matrices
    __EAPI Matrix4f GetModelTransformMatrix ();
    __EAPI void CalculateModelTransformMatrix ();

    // get object origin's position in world space
    __EAPI Vector4f GetPosition ();

    // get object's orthogonal basis vectors
    __EAPI Vector3f GetRight ();
    __EAPI Vector3f GetUp ();
    __EAPI Vector3f GetForward ();	

    // check if object is a light
    __EAPI bool IsLight ();

protected:

    GameLoop* gameLoop;

    // is object a light
    bool isLight;

private:

    // object's handle (index in vector inside GameScene)
    unsigned int objectHandle;

    // pointer to parent object (objects may form a hierarchy)
    GameObject* parentObject;
    
    // vector of child objects
    std::vector<GameObject*> childObjects;

    // object's transformation from local space to world space
    Transform modelTransform;

    // transform matrix starting from object's scene root
    Matrix4f modelTransformMatrix;
};

#endif
