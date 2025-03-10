#include "scene/LinearPath.h"
#include "math/Mathf.h"

namespace cilantro {

LinearPath::LinearPath (CGameScene* gameScene) : Path (gameScene)
{

}

LinearPath::~LinearPath ()
{

}

Vector3f LinearPath::GetPositionAtDistance (float distance) const
{
    std::size_t wIndex = FindPathSegment (distance);
    float u = CalculateInterpolationParameter (wIndex, distance);

    if (wIndex != 0)
    {
        return GetModelTransformMatrix () * Vector4f (Mathf::Lerp (waypoints[wIndex - 1].t.GetTranslation (), waypoints[wIndex].t.GetTranslation (), u), 1.0f);
    }

    return GetModelTransformMatrix () * Vector4f (waypoints[wIndex].t.GetTranslation (), 1.0f);
}

Vector3f LinearPath::GetTangentAtDistance (float distance) const
{
    std::size_t wIndex = FindPathSegment (distance);

    return GetModelTransformMatrix () * Vector4f (waypoints[wIndex + 1].outTangent, 0.0f);
}

void LinearPath::UpdatePathAtWaypoint (std::size_t wIndex) 
{
    std::vector<Waypoint>::iterator it = waypoints.begin () + wIndex;

    for (auto i = it; i != waypoints.end(); ++i)
    {
        Waypoint& w = *i;

        if (i == waypoints.begin ())
        {
            // first waypoint
            w.segmentLength = 0.0f;
            w.cumulativeLength = 0.0f;

            w.inTangent = Vector3f (0.0f, 0.0f, 0.0f);
            w.outTangent = Vector3f (0.0f, 0.0f, 0.0f);
        }
        else
        {
            Waypoint& w0 = *(i - 1);

            w.segmentLength = Mathf::Length (w.t.GetTranslation () - w0.t.GetTranslation ());
            w.cumulativeLength = w0.cumulativeLength + w.segmentLength;

            w0.outTangent = Mathf::Normalize (w.t.GetTranslation () - w0.t.GetTranslation ());
            w.inTangent = w0.outTangent;
        }
    }
}

} // namespace cilantro