#include "scene/Path.h"
#include "math/Mathf.h"

namespace cilantro
{

Path::Path (GameScene* gameScene) : GameObject (gameScene)
{
}

Path::~Path ()
{
}

Path& Path::AddWaypoint (std::size_t wIndex, const Vector3f& position, const Quaternion& rotation)
{
    Waypoint w;
    std::vector<Waypoint>::iterator it;

    w.t.Translate (position);
    w.t.Rotate (rotation);

    if (wIndex >= waypoints.size ())
    {
        // insert at back
        it = waypoints.insert (waypoints.end (), std::move (w));
    }
    else 
    {
        // insert in between
        it = waypoints.insert (waypoints.begin () + wIndex, std::move (w));
    }

    UpdatePathAtWaypoint (std::distance(waypoints.begin(), it));

    return *this;
}

Path& Path::AddWaypoint (const Vector3f& position, const Quaternion& rotation)
{
    return AddWaypoint (waypoints.size (), position, rotation);
}

Path& Path::SetWaypoint (std::size_t wIndex, const Vector3f& position, const Quaternion& rotation)
{
    if (wIndex < waypoints.size ())
    {
        waypoints[wIndex].t.Translate (position);
        waypoints[wIndex].t.Rotate (rotation);

        UpdatePathAtWaypoint (wIndex);
    }

    return *this;
}

Quaternion Path::GetRotationAtDistance (float distance) const
{
    std::size_t wIndex = FindPathSegment (distance);
    float u = CalculateInterpolationParameter (wIndex, distance);

    Quaternion q = this->GetRotation ();

    if (wIndex != 0)
    {
        return Mathf::Slerp (Mathf::Product (q, waypoints[wIndex - 1].t.GetRotationQuaternion ()), Mathf::Product (q, waypoints[wIndex].t.GetRotationQuaternion ()), u);
    }

    return Mathf::Product (q, waypoints[wIndex].t.GetRotationQuaternion ());
}

float Path::GetPathLength () const
{
    return waypoints.back ().cumulativeLength;
}

std::size_t Path::FindPathSegment (float distance) const
{
    Waypoint w;
    
    // binary search (this assumes vector is sorted by cumulativeLength)
    w.cumulativeLength = distance;
    auto it = std::lower_bound (waypoints.begin (), waypoints.end (), w, [] (Waypoint w0, Waypoint w1) { return w0.cumulativeLength < w1.cumulativeLength; });
    
    return std::distance(waypoints.begin(), it);
}

float Path::CalculateInterpolationParameter (std::size_t wIndex, float distance) const
{
    if (wIndex != 0)
    {
        return (distance - waypoints[wIndex - 1].cumulativeLength) / (waypoints[wIndex].cumulativeLength - waypoints[wIndex - 1].cumulativeLength);
    }

    return 0.0f;
}

} // namespace cilantro