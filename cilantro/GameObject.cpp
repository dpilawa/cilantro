#include "GameObject.h"

GameObject::GameObject ()
{
	parentObject = nullptr;
	myGameScene = nullptr;

	// set callbacks on transform modification
	// this is just a passthrough of callbacks to subscribers (Scene)
	modelTransform.RegisterCallback ("OnUpdateTransform", [ & ](unsigned int objectHandle) { InvokeCallbacks ("OnUpdateTransform", objectHandle); });

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
	InvokeCallbacks ("OnUpdateSceneGraph", this->GetHandle ());
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
		return parentObject->GetModelTransformMatrix () * modelTransform.GetModelMatrix ();
	}
}

Vector4f GameObject::GetPosition ()
{
	return Vector4f (0.0f, 0.0f, 0.0f, 1.0f) * GetModelTransformMatrix ();
}



