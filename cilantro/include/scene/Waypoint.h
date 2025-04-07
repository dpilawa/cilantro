#ifndef _WAYPOINT_H_
#define _WAYPOINT_H_

#include "cilantroengine.h"
#include "scene/Transform.h"
#include "math/Vector3f.h"
#include <vector>

namespace cilantro {

class __CEAPI Waypoint
{
public:
    Waypoint ();
    virtual ~Waypoint ();

    std::shared_ptr<Transform> m_Transform;
    Vector3f m_inTangent;
    Vector3f m_outTangent;
    float m_segmentLength;
    float m_cumulativeLength;
};

} // namespace cilantro

#endif