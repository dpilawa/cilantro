#include "GameObject.h"

GameObject::GameObject ()
{
}

GameObject::~GameObject ()
{
}

Transform& GameObject::getTransform ()
{
	return objectTransform;
}