#include "cilantroengine.h"
#include "resource/Resource.h"

Resource::Resource (const std::string& name)
{
    this->name = name;
}

Resource::Resource (const std::string& name, const std::string& path)
{
    this->name = name;
    this->path = path;
}

Resource::~Resource ()
{

}

unsigned int Resource::GetHandle () const
{
    return handle;
}

std::string Resource::GetName () const
{
    return name;
}

