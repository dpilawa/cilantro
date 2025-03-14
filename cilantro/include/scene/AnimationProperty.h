#ifndef _ANIMATIONPROPERTY_H_
#define _ANIMATIONPROPERTY_H_

#include "cilantroengine.h"
#include "system/LogMessage.h"
#include "math/Vector3f.h"
#include "math/Quaternion.h"
#include <functional>
#include <string>
#include <map>

namespace cilantro {

template<typename P>
class __CEAPI AnimationProperty 
{
public:
    __EAPI AnimationProperty (std::function<void (P)> updateFunction, std::function<P (P, P, float)> interpolateFunction);
    virtual ~AnimationProperty () {};

    __EAPI bool AddKeyframe (float time, P propertyValue);
    __EAPI P GetFrame (float time);

    __EAPI std::function<void(P)> GetUpdateFunction ();
    __EAPI std::function<P(P, P, float)> GetInterpolateFunction ();

private:
    float maxKeyframeTime;

    std::function<void (P)> updateFunction;
    std::function<P (P, P, float)> interpolateFunction;

    std::map<float, P> keyframes;

};

} // namespace cilantro

#endif