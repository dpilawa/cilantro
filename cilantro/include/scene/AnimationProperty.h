#ifndef _ANIMATIONPROPERTY_H_
#define _ANIMATIONPROPERTY_H_

#include "cilantroengine.h"
#include "system/LogMessage.h"
#include "resource/Resource.h"
#include "resource/ResourceManager.h"
#include "math/Vector3f.h"
#include "math/Quaternion.h"
#include <functional>
#include <string>
#include <map>
#include <memory>

namespace cilantro {

class AnimationObject;

template<typename P>
class __CEAPI AnimationProperty : public Resource, public std::enable_shared_from_this<AnimationProperty<P>>
{
public:
    __EAPI AnimationProperty (std::shared_ptr<AnimationObject> animationObject, std::function<void (P)> updateFunction, std::function<P (P, P, float)> interpolateFunction);
    virtual ~AnimationProperty () {};

    __EAPI std::shared_ptr<AnimationProperty<P>> AddKeyframe (float time, P propertyValue);
    __EAPI P GetFrame (float time);

    __EAPI std::function<void(P)> GetUpdateFunction ();
    __EAPI std::function<P(P, P, float)> GetInterpolateFunction ();

private:
    float m_maxKeyframeTime;

    std::weak_ptr<AnimationObject> m_animationObject;

    std::function<void (P)> m_updateFunction;
    std::function<P (P, P, float)> m_interpolateFunction;

    std::map<float, P> m_keyframes;

};

} // namespace cilantro

#endif