#include "cilantroengine.h"
#include "Matrix4f.h"
#include "Vector4f.h"
#include "Renderer.h"
#include "GameScene.h"
#include "GameObject.h"
#include "Transform.h"
#include "CallbackProvider.h"
#include "Mathf.h"
#include <string>

GameObject::GameObject ()
{
	parentObject = nullptr;
	//myGameScene = nullptr;

	isLight = false;

	// set callbacks on transform modification
	// this is just a passthrough of callbacks to subscribers (Scene)
	modelTransform.RegisterCallback ("OnUpdateTransform", [ & ](unsigned int objectHandle) { InvokeCallbacks ("OnUpdateTransform", this->GetHandle ()); });

}

GameObject::~GameObject ()
{
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

Transform & GameObject::GetModelTransform ()
{
	return modelTransform;
}

Matrix4f GameObject::GetModelTransformMatrix ()
{
	if (parentObject == nullptr)
	{
		return modelTransform.GetModelMatrix ();
	}
	else
	{
		return parentObject->GetModelTransformMatrix () * modelTransform.GetModelMatrix ();
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