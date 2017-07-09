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
	return GetTranslationTransformMatrix () * GetRotationTransformMatrix () * GetScalingTransformMatrix ();
}

Matrix4f GameObject::GetRotationTransformMatrix ()
{
	if (parentObject == nullptr)
	{
		return modelTransform.GetRotationMatrix ();
	}
	else
	{
		return modelTransform.GetRotationMatrix () * parentObject->GetRotationTransformMatrix ();
	}
}

Matrix4f GameObject::GetTranslationTransformMatrix ()
{
	if (parentObject == nullptr)
	{
		return modelTransform.GetTranslationMatrix ();
	}
	else
	{
		return modelTransform.GetTranslationMatrix () * parentObject->GetTranslationTransformMatrix ();
	}
}

Matrix4f GameObject::GetScalingTransformMatrix ()
{
	if (parentObject == nullptr)
	{
		return modelTransform.GetScalingMatrix ();
	}
	else
	{
		return modelTransform.GetScalingMatrix () * parentObject->GetScalingTransformMatrix ();
	}
}


