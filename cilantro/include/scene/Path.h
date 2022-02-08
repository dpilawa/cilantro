#ifndef _PATH_H_
#define _PATH_H_

#include "cilantroengine.h"
#include "scene/GameObject.h"
#include "scene/Transform.h"
#include "math/Vector3f.h"
#include "math/Quaternion.h"
#include <vector>

class Waypoint
{
public:
    Transform t;
    Vector3f inTangent;
    Vector3f outTangent;
    float segmentLength;
    float cumulativeLength;
};

class Path : public GameObject
{
public:
    Path (GameScene* gameScene);
    virtual ~Path ();

    __EAPI Path& AddWaypoint (std::size_t wIndex, const Vector3f& position, const Quaternion& rotation);
    __EAPI Path& AddWaypoint (const Vector3f& position, const Quaternion& rotation);
    
    __EAPI Path& SetWaypoint (std::size_t wIndex, const Vector3f& position, const Quaternion& rotation);

    virtual Vector3f GetPositionAtDistance (float distance) const = 0;
    virtual Vector3f GetTangentAtDistance (float distance) const = 0;
    virtual Quaternion GetRotationAtDistance (float distance) const;

    __EAPI float GetPathLength () const;

protected:
    std::vector<Waypoint> waypoints;

    /* return index of first waypoint whose cumulative distance is greater or equal (not less) than parameter */
    std::size_t FindPathSegment (float distance) const;

    /* return interpolation parameter with respect to given segment, based on cumulative distance */
    float CalculateInterpolationParameter (std::size_t wIndex, float distance) const;

private:
    /* rebuild path distances and tangents */
    virtual void UpdatePathAtWaypoint (std::size_t wIndex) = 0;
};

#endif