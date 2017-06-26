#include "GameObject.h"

GameObject::GameObject ()
{
	parentObject = nullptr;
	myGameScene = nullptr;
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

void GameObject::SetParentObject (GameObject & const parent)
{
	parentObject = &parent;
}

void GameObject::SetGameScene (GameScene & const scene)
{
	myGameScene = &scene;
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

void GameObject::OnEnd ()
{
}

Transform & GameObject::GetWorldTransform ()
{
	return worldTransform;
}

Matrix4f GameObject::GetWorldTransformMatrix ()
{
	if (parentObject == nullptr) 
	{
		return worldTransform.GetModelMatrix ();
	}
	else 
	{
		return worldTransform.GetModelMatrix () * parentObject->GetWorldTransformMatrix ();
	}
}


