#include "cilantroengine.h"
#include "resource/Resource.h"

namespace cilantro {

handle_t Resource::GetHandle () const
{
    return m_handle;
}

std::string Resource::GetName () const
{
    return m_name;
}

} // namespace cilantro

