#include "GameObject.h"

GameObject::GameObject ()
{
}

GameObject::~GameObject ()
{
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