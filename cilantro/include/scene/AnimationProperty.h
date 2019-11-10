#ifndef _ANIMATIONPROPERTY_H_
#define _ANIMATIONPROPERTY_H_

#include "cilantroengine.h"
#include "util/LogMessage.h"
#include "math/Vector3f.h"
#include "math/Quaternion.h"
#include <functional>
#include <string>
#include <map>

template<typename P> class __EAPI AnimationProperty;

template<typename P>
class AnimationProperty 
{
public:
    AnimationProperty (std::function<void (P)> updateFunction, std::function<P (P, P, float)> interpolateFunction);
    ~AnimationProperty (){};

    bool AddKeyframe (float time, P propertyValue);
    P GetFrame (float time);

    std::function<void(P)> GetUpdateFunction ();
    std::function<P(P, P, float)> GetInterpolateFunction ();

private:
    float maxKeyframeTime;

    std::function<void (P)> updateFunction;
    std::function<P (P, P, float)> interpolateFunction;

    std::map<float, P> keyframes;

};

// template instantiations
template class AnimationProperty<float>;
template class AnimationProperty<Vector3f>;
template class AnimationProperty<Quaternion>;

#endif