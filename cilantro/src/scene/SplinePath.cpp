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
        return GetWorldTransformMatrix () * Vector4f (m_curves[wIndex - 1].GetCurvePoint (u), 1.0f);
    }

    return GetWorldTransformMatrix () * Vector4f (m_waypoints[wIndex].m_Transform->GetTranslation (), 1.0f);
}

Vector3f SplinePath::GetTangentAtDistance (float distance) const
{
    std::size_t wIndex = FindPathSegment (distance);
    float u = CalculateInterpolationParameter (wIndex, distance);

    if (wIndex != 0)
    {
        return GetWorldTransformMatrix () * Vector4f (m_curves[wIndex - 1].GetCurveTangent (u), 1.0f);
    }

    return GetWorldTransformMatrix () * Vector4f (m_waypoints[wIndex].m_outTangent, 1.0f);
}

std::shared_ptr<SplinePath> SplinePath::SetStartTangent (const Vector3f& tangent)
{
    if (m_waypoints.size () > 1)
    {
        m_waypoints[0].m_outTangent = tangent;
        m_curves[0].SetPointsAndTangents (m_waypoints[0].m_Transform->GetTranslation (), m_waypoints[1].m_Transform->GetTranslation (), m_waypoints[0].m_outTangent, m_waypoints[1].m_inTangent);
    }

    return std::dynamic_pointer_cast<SplinePath> (shared_from_this ());
}

std::shared_ptr<SplinePath> SplinePath::SetEndTangent (const Vector3f& tangent)
{
    if (m_waypoints.size () > 1)
    {
        size_t wIndex = m_waypoints.size () - 1;

        m_waypoints[wIndex].m_inTangent = tangent;
        m_curves[wIndex - 1].SetPointsAndTangents (m_waypoints[wIndex - 1].m_Transform->GetTranslation (), m_waypoints[wIndex].m_Transform->GetTranslation (), m_waypoints[wIndex - 1].m_outTangent, m_waypoints[wIndex].m_inTangent);
    }

    return std::dynamic_pointer_cast<SplinePath> (shared_from_this ());
}

void SplinePath::UpdatePathAtWaypoint (const std::size_t wIndex)
{
    // create new curve if needed
    if (m_curves.size () + 1 < m_waypoints.size ())
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

    for (std::size_t i = std::max<std::size_t>(0, wIndex - 1); i < m_waypoints.size (); ++i)
    {
        Vector3f tangent;

        if (i == 0)
        {
            // first waypoint
            tangent = 0.5f * (m_waypoints[i + 1].m_Transform->GetTranslation () - m_waypoints[i].m_Transform->GetTranslation ());
            m_waypoints[i].m_outTangent = tangent;
            m_waypoints[i].m_segmentLength = 0.0f;
            m_waypoints[i].m_cumulativeLength = 0.0f;
        }
        else if (i == m_waypoints.size() - 1)
        {
            // last waypoint
            tangent = 0.5f * (m_waypoints[i].m_Transform->GetTranslation () - m_waypoints[i - 1].m_Transform->GetTranslation ());
            m_waypoints[i].m_inTangent = tangent;
            m_curves[i - 1].SetPointsAndTangents (m_waypoints[i - 1].m_Transform->GetTranslation (), m_waypoints[i].m_Transform->GetTranslation (), m_waypoints[i - 1].m_outTangent, m_waypoints[i].m_inTangent);
            m_waypoints[i].m_segmentLength = m_curves[i - 1].GetCurveLength ();
            m_waypoints[i].m_cumulativeLength = m_waypoints[i - 1].m_cumulativeLength + m_waypoints[i].m_segmentLength;
        }
        else
        {
            // internal waypoint
            tangent = 0.5f * (m_waypoints[i + 1].m_Transform->GetTranslation () - m_waypoints[i - 1].m_Transform->GetTranslation ());
            m_waypoints[i].m_inTangent = tangent;
            m_waypoints[i].m_outTangent = tangent;
            m_curves[i - 1].SetPointsAndTangents (m_waypoints[i - 1].m_Transform->GetTranslation (), m_waypoints[i].m_Transform->GetTranslation (), m_waypoints[i - 1].m_outTangent, m_waypoints[i].m_inTangent);
            m_waypoints[i].m_segmentLength = m_curves[i - 1].GetCurveLength ();
            m_waypoints[i].m_cumulativeLength = m_waypoints[i - 1].m_cumulativeLength + m_waypoints[i].m_segmentLength;      
        }
    }
}

} // namespace cilantro