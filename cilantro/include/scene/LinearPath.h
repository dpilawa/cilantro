#ifndef _LINEARPATH_H_
#define _LINEARPATH_H_

#include "cilantroengine.h"
#include "scene/Path.h"

class LinearPath : public Path
{
public:
    __EAPI LinearPath ();
    __EAPI virtual ~LinearPath ();

    __EAPI Vector3f GetPositionAtDistance (float distance) const;

private:
    void UpdatePathAtWaypoint (std::size_t wIndex);
};

#endif