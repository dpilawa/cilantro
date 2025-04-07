#include "scene/LinearPath.h"
#include "math/Mathf.h"

namespace cilantro {

LinearPath::LinearPath (std::shared_ptr<GameScene> gameScene) : Path (gameScene)
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
        return GetWorldTransformMatrix () * Vector4f (Mathf::Lerp (m_waypoints[wIndex - 1].m_Transform->GetTranslation (), m_waypoints[wIndex].m_Transform->GetTranslation (), u), 1.0f);
    }

    return GetWorldTransformMatrix () * Vector4f (m_waypoints[wIndex].m_Transform->GetTranslation (), 1.0f);
}

Vector3f LinearPath::GetTangentAtDistance (float distance) const
{
    std::size_t wIndex = FindPathSegment (distance);

    return GetWorldTransformMatrix () * Vector4f (m_waypoints[wIndex + 1].m_outTangent, 0.0f);
}

void LinearPath::UpdatePathAtWaypoint (std::size_t wIndex) 
{
    std::vector<Waypoint>::iterator it = m_waypoints.begin () + wIndex;

    for (auto i = it; i != m_waypoints.end(); ++i)
    {
        Waypoint& w = *i;

        if (i == m_waypoints.begin ())
        {
            // first waypoint
            w.m_segmentLength = 0.0f;
            w.m_cumulativeLength = 0.0f;

            w.m_inTangent = Vector3f (0.0f, 0.0f, 0.0f);
            w.m_outTangent = Vector3f (0.0f, 0.0f, 0.0f);
        }
        else
        {
            Waypoint& w0 = *(i - 1);

            w.m_segmentLength = Mathf::Length (w.m_Transform->GetTranslation () - w0.m_Transform->GetTranslation ());
            w.m_cumulativeLength = w0.m_cumulativeLength + w.m_segmentLength;

            w0.m_outTangent = Mathf::Normalize (w.m_Transform->GetTranslation () - w0.m_Transform->GetTranslation ());
            w.m_inTangent = w0.m_outTangent;
        }
    }
}

} // namespace cilantro