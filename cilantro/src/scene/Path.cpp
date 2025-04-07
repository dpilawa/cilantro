#include "scene/Path.h"
#include "math/Mathf.h"

namespace cilantro
{

Path::Path (std::shared_ptr<GameScene> gameScene) : GameObject (gameScene)
{
}

Path::~Path ()
{
}

std::shared_ptr<Path> Path::AddWaypoint (std::size_t wIndex, const Vector3f& position, const Quaternion& rotation)
{
    Waypoint w;
    std::vector<Waypoint>::iterator it;

    w.m_Transform->Translate (position)->Rotate (rotation);

    if (wIndex >= m_waypoints.size ())
    {
        // insert at back
        it = m_waypoints.insert (m_waypoints.end (), std::move (w));
    }
    else 
    {
        // insert in between
        it = m_waypoints.insert (m_waypoints.begin () + wIndex, std::move (w));
    }

    UpdatePathAtWaypoint (std::distance(m_waypoints.begin(), it));

    return std::dynamic_pointer_cast<Path> (shared_from_this ());
}

std::shared_ptr<Path> Path::AddWaypoint (const Vector3f& position, const Quaternion& rotation)
{
    return AddWaypoint (m_waypoints.size (), position, rotation);
}

std::shared_ptr<Path> Path::SetWaypoint (std::size_t wIndex, const Vector3f& position, const Quaternion& rotation)
{
    if (wIndex < m_waypoints.size ())
    {
        m_waypoints[wIndex].m_Transform->Translate (position)->Rotate (rotation);

        UpdatePathAtWaypoint (wIndex);
    }

    return std::dynamic_pointer_cast<Path> (shared_from_this ());
}

Quaternion Path::GetRotationAtDistance (float distance) const
{
    std::size_t wIndex = FindPathSegment (distance);
    float u = CalculateInterpolationParameter (wIndex, distance);

    Quaternion q = this->GetRotation ();

    if (wIndex != 0)
    {
        return Mathf::Slerp (Mathf::Product (q, m_waypoints[wIndex - 1].m_Transform->GetRotationQuaternion ()), Mathf::Product (q, m_waypoints[wIndex].m_Transform->GetRotationQuaternion ()), u);
    }

    return Mathf::Product (q, m_waypoints[wIndex].m_Transform->GetRotationQuaternion ());
}

float Path::GetPathLength () const
{
    return m_waypoints.back ().m_cumulativeLength;
}

std::size_t Path::FindPathSegment (float distance) const
{
    Waypoint w;
    
    // binary search (this assumes vector is sorted by cumulativeLength)
    w.m_cumulativeLength = distance;
    auto it = std::lower_bound (m_waypoints.begin (), m_waypoints.end (), w, [] (Waypoint w0, Waypoint w1) { return w0.m_cumulativeLength < w1.m_cumulativeLength; });
    
    return std::distance(m_waypoints.begin(), it);
}

float Path::CalculateInterpolationParameter (std::size_t wIndex, float distance) const
{
    if (wIndex != 0)
    {
        return (distance - m_waypoints[wIndex - 1].m_cumulativeLength) / (m_waypoints[wIndex].m_cumulativeLength - m_waypoints[wIndex - 1].m_cumulativeLength);
    }

    return 0.0f;
}

} // namespace cilantro