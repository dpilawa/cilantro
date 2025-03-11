#include "cilantroengine.h"
#include "resource/Resource.h"

namespace cilantro {

Resource::Resource ()
{
    m_handle = -1;
}

Resource::~Resource ()
{
}

handle_t Resource::GetHandle () const
{
    return m_handle;
}

std::string Resource::GetName () const
{
    return m_name;
}

} // namespace cilantro

