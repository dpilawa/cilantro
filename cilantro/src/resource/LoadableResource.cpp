#include "cilantroengine.h"
#include "resource/LoadableResource.h"

namespace cilantro {

CLoadableResource::CLoadableResource () : CResource ()
{
    this->path = std::string ();
}

CLoadableResource::CLoadableResource (const std::string& path) : CResource ()
{
    this->path = path;
}

} // namespace cilantro



