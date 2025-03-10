#include "cilantroengine.h"
#include "resource/LoadableResource.h"

namespace cilantro {

LoadableResource::LoadableResource () : Resource ()
{
    this->path = std::string ();
}

LoadableResource::LoadableResource (const std::string& path) : Resource ()
{
    this->path = path;
}

} // namespace cilantro



