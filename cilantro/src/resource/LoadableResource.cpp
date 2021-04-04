#include "cilantroengine.h"
#include "resource/LoadableResource.h"

LoadableResource::LoadableResource () : Resource ()
{
    this->path = std::string ();
}

LoadableResource::LoadableResource (const std::string& path) : Resource ()
{
    this->path = path;
}

LoadableResource::~LoadableResource ()
{

}

