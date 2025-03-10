#include "cilantroengine.h"
#include "resource/Resource.h"

namespace cilantro {

handle_t CResource::GetHandle () const
{
    return m_handle;
}

std::string CResource::GetName () const
{
    return m_name;
}

} // namespace cilantro

