#include "cilantroengine.h"
#include "resource/Resource.h"

Resource::Resource ()
{
}

Resource::~Resource ()
{

}

handle_t Resource::GetHandle () const
{
    return handle;
}

std::string Resource::GetName () const
{
    return name;
}

