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

void GameObject::OnDraw (Renderer& renderer)
{
}

Transform& GameObject::getWorldTransform ()
{
	return worldTransform;
}