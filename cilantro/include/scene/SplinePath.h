#ifndef _SPLINEPATH_H_
#define _SPLINEPATH_H_

#include "cilantroengine.h"
#include "scene/Path.h"
#include "math/CubicHermite.h"
#include <vector>

// Catmull-Rom Spline
class SplinePath : public Path 
{
public:
    __EAPI SplinePath ();
    __EAPI virtual ~SplinePath ();

    __EAPI Vector3f GetPositionAtDistance (float distance) const;

    __EAPI SplinePath& SetStartTangent (const Vector3f& tangent);
    __EAPI SplinePath& SetEndTangent (const Vector3f& tangent);

private:
    std::vector<CubicHermite<Vector3f>> curves;

    void UpdatePathAtWaypoint (const std::size_t wIndex);
};

#endif