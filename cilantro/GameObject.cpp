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

void GameObject::SetGameObjectModifiedCallback (std::function<void (size_t)> callback)
{
	objectCallbacks.push_back (callback);
}

Transform& GameObject::getWorldTransform ()
{
	return worldTransform;
}

void GameObject::GameObjectModifiedNotify ()
{
	// notify all callback subscribers about modified object
	for each (auto callback in objectCallbacks)
	{
		callback (this->GetHandle ());
	}
}
