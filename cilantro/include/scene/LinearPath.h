#ifndef _LINEARPATH_H_
#define _LINEARPATH_H_

#include "cilantroengine.h"
#include "scene/Path.h"

namespace cilantro {

class __CEAPI LinearPath : public Path
{
public:
    __EAPI LinearPath (std::shared_ptr<GameScene> gameScene);
    __EAPI virtual ~LinearPath ();

    __EAPI Vector3f GetPositionAtDistance (float distance) const;
    __EAPI Vector3f GetTangentAtDistance (float distance) const;

private:
    __EAPI void UpdatePathAtWaypoint (std::size_t wIndex);
};

} // namespace cilantro

#endif