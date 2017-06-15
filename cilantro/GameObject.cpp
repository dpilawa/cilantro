#include "GameObject.h"

GameObject::GameObject ()
{
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

Transform& GameObject::getWorldTransform ()
{
	return worldTransform;
}


