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

void GameObject::SetParentObject (GameObject & parent)
{
	parentObject = &parent;
}

void GameObject::SetGameScene (GameScene & scene)
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
		return modelTransform.GetModelMatrix () * parentObject->GetModelTransformMatrix ();
	}
}


