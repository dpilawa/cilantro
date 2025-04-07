#include "cilantroengine.h"
#include "scene/Waypoint.h"

namespace cilantro {

Waypoint::Waypoint ()
{ 
    m_Transform=std::make_shared<Transform>(); 
}

Waypoint::~Waypoint ()
{

}

}