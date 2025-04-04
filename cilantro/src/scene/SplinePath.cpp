#include "scene/SplinePath.h"
#include "math/Mathf.h"
#include "math/GaussLegendreIntegrator.h"
#include "math/Vector3f.h"

namespace cilantro {

SplinePath::SplinePath (std::shared_ptr<GameScene> gameScene) : Path (gameScene)
{
}

SplinePath::~SplinePath ()
{
}

Vector3f SplinePath::GetPositionAtDistance (float distance) const
{
    std::size_t wIndex = FindPathSegment (distance);
    float u = CalculateInterpolationParameter (wIndex, distance);

    if (wIndex != 0)
    {
        return GetModelTransformMatrix () * Vector4f (m_curves[wIndex - 1].GetCurvePoint (u), 1.0f);
    }

    return GetModelTransformMatrix () * Vector4f (waypoints[wIndex].t.GetTranslation (), 1.0f);
}

Vector3f SplinePath::GetTangentAtDistance (float distance) const
{
    std::size_t wIndex = FindPathSegment (distance);
    float u = CalculateInterpolationParameter (wIndex, distance);

    if (wIndex != 0)
    {
        return GetModelTransformMatrix () * Vector4f (m_curves[wIndex - 1].GetCurveTangent (u), 1.0f);
    }

    return GetModelTransformMatrix () * Vector4f (waypoints[wIndex].outTangent, 1.0f);
}

SplinePath& SplinePath::SetStartTangent (const Vector3f& tangent)
{
    if (waypoints.size () > 1)
    {
        waypoints[0].outTangent = tangent;
        m_curves[0].SetPointsAndTangents (waypoints[0].t.GetTranslation (), waypoints[1].t.GetTranslation (), waypoints[0].outTangent, waypoints[1].inTangent);
    }

    return *this;
}

SplinePath& SplinePath::SetEndTangent (const Vector3f& tangent)
{
    if (waypoints.size () > 1)
    {
        size_t wIndex = waypoints.size () - 1;

        waypoints[wIndex].inTangent = tangent;
        m_curves[wIndex - 1].SetPointsAndTangents (waypoints[wIndex - 1].t.GetTranslation (), waypoints[wIndex].t.GetTranslation (), waypoints[wIndex - 1].outTangent, waypoints[wIndex].inTangent);
    }

    return *this;
}

void SplinePath::UpdatePathAtWaypoint (const std::size_t wIndex)
{
    // create new curve if needed
    if (m_curves.size () + 1 < waypoints.size ())
    {
		if (m_curves.size() <= wIndex - 1)
		{
			m_curves.emplace_back ();
		}
		else
		{
			m_curves.emplace (m_curves.begin () + wIndex - 1);
		}
    }

    for (std::size_t i = std::max<std::size_t>(0, wIndex - 1); i < waypoints.size (); ++i)
    {
        Vector3f tangent;

        if (i == 0)
        {
            // first waypoint
            tangent = 0.5f * (waypoints[i + 1].t.GetTranslation () - waypoints[i].t.GetTranslation ());
            waypoints[i].outTangent = tangent;
            waypoints[i].segmentLength = 0.0f;
            waypoints[i].cumulativeLength = 0.0f;
        }
        else if (i == waypoints.size() - 1)
        {
            // last waypoint
            tangent = 0.5f * (waypoints[i].t.GetTranslation () - waypoints[i - 1].t.GetTranslation ());
            waypoints[i].inTangent = tangent;
            m_curves[i - 1].SetPointsAndTangents (waypoints[i - 1].t.GetTranslation (), waypoints[i].t.GetTranslation (), waypoints[i - 1].outTangent, waypoints[i].inTangent);
            waypoints[i].segmentLength = m_curves[i - 1].GetCurveLength ();
            waypoints[i].cumulativeLength = waypoints[i - 1].cumulativeLength + waypoints[i].segmentLength;
        }
        else
        {
            // internal waypoint
            tangent = 0.5f * (waypoints[i + 1].t.GetTranslation () - waypoints[i - 1].t.GetTranslation ());
            waypoints[i].inTangent = tangent;
            waypoints[i].outTangent = tangent;
            m_curves[i - 1].SetPointsAndTangents (waypoints[i - 1].t.GetTranslation (), waypoints[i].t.GetTranslation (), waypoints[i - 1].outTangent, waypoints[i].inTangent);
            waypoints[i].segmentLength = m_curves[i - 1].GetCurveLength ();
            waypoints[i].cumulativeLength = waypoints[i - 1].cumulativeLength + waypoints[i].segmentLength;      
        }
    }
}

} // namespace cilantro