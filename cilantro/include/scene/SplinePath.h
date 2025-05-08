#ifndef _SPLINEPATH_H_
#define _SPLINEPATH_H_

#include "cilantroengine.h"
#include "scene/Path.h"
#include "math/CubicHermite.h"
#include <vector>

namespace cilantro {

// Catmull-Rom Spline
class __CEAPI SplinePath : public Path 
{
public:
    __EAPI SplinePath (std::shared_ptr<GameScene> gameScene);
    __EAPI virtual ~SplinePath ();

    __EAPI Vector3f GetPositionAtDistance (float distance) const;
    __EAPI Vector3f GetTangentAtDistance (float distance) const;

    __EAPI std::shared_ptr<SplinePath> SetStartTangent (const Vector3f& tangent);
    __EAPI std::shared_ptr<SplinePath> SetEndTangent (const Vector3f& tangent);

private:
    std::vector<CubicHermite<Vector3f>> m_curves;
    bool m_hasStartTangent;
    bool m_hasEndTangent;
    Vector3f m_startTangent;
    Vector3f m_endTangent;

    __EAPI void UpdatePathAtWaypoint (const std::size_t wIndex);
};

} // namespace cilantro

#endif