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

void GameObject::Draw ()
{
}

Transform& GameObject::getWorldTransform ()
{
	return worldTransform;
}