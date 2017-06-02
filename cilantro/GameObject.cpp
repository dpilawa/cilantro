#include "GameObject.h"

GameObject::GameObject ()
{
}

GameObject::~GameObject ()
{
}

void GameObject::SetHandle (size_t handle)
{
	objectHandle = handle;
}

size_t GameObject::GetHandle () const
{
	return objectHandle;
}

void GameObject::OnStart ()
{
}

void GameObject::OnFrame ()
{
}

void GameObject::OnEnd ()
{
}

Transform& GameObject::getWorldTransform ()
{
	return worldTransform;
}


