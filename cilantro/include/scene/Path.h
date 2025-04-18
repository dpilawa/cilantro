#ifndef _PATH_H_
#define _PATH_H_

#include "cilantroengine.h"
#include "scene/GameObject.h"
#include "scene/Transform.h"
#include "scene/Waypoint.h"
#include "math/Vector3f.h"
#include "math/Quaternion.h"
#include <vector>

namespace cilantro {

class __CEAPI Path : public GameObject
{
public:
    __EAPI Path (std::shared_ptr<GameScene> gameScene);
    __EAPI virtual ~Path ();

    __EAPI std::shared_ptr<Path> AddWaypoint (std::size_t wIndex, const Vector3f& position, const Quaternion& rotation);
    __EAPI std::shared_ptr<Path> AddWaypoint (const Vector3f& position, const Quaternion& rotation);
    
    __EAPI std::shared_ptr<Path> SetWaypoint (std::size_t wIndex, const Vector3f& position, const Quaternion& rotation);

    __EAPI virtual Vector3f GetPositionAtDistance (float distance) const = 0;
    __EAPI virtual Vector3f GetTangentAtDistance (float distance) const = 0;
    __EAPI virtual Quaternion GetRotationAtDistance (float distance) const;

    __EAPI float GetPathLength () const;

protected:
    std::vector<Waypoint> m_waypoints;

    /* return index of first waypoint whose cumulative distance is greater or equal (not less) than parameter */
    std::size_t FindPathSegment (float distance) const;

    /* return interpolation parameter with respect to given segment, based on cumulative distance */
    __EAPI float CalculateInterpolationParameter (std::size_t wIndex, float distance) const;

private:
    /* rebuild path distances and tangents */
    virtual void UpdatePathAtWaypoint (std::size_t wIndex) = 0;
};

} // namespace cilantro

#endif