#include "scene/AnimationProperty.h"
#include "math/Vector3f.h"
#include "math/Quaternion.h"

template<typename P>
AnimationProperty<P>::AnimationProperty (std::function<void (P)> updateFunction, std::function<P (P, P, float)> interpolateFunction)
: updateFunction (updateFunction), interpolateFunction(interpolateFunction)
{
    maxKeyframeTime = 0.0f;
}

template<typename P>
bool AnimationProperty<P>::AddKeyframe (float time, P propertyValue)
{
    auto result = keyframes.insert (std::make_pair (time, propertyValue));
    if (result.second == true)
    {
        if (time > maxKeyframeTime)
        {
            maxKeyframeTime = time;
        }
        return true;
    }
    else
    {
        return false;
    }
}

template<typename P>
P AnimationProperty<P>::GetFrame (float time)
{
    if (keyframes.empty ())
    {
        LogMessage (__func__, EXIT_FAILURE) << "Keyframe collection is empty";
        return P ();
    }

    if ((time > maxKeyframeTime) || (keyframes.size() == 1))
    {
        return keyframes.rbegin ()->second;
    }
    else 
    {
        typename std::map<float, P>::iterator i0 = --keyframes.upper_bound (time);
        typename std::map<float, P>::iterator i1 = keyframes.upper_bound (time);

        float t0 = i0->first;
        float t1 = i1->first;

        P p0 = i0->second;
        P p1 = i1->second;

        return interpolateFunction (p0, p1, (time - t0) / (t1 - t0));
    }
}

template<typename P>
std::function<void(P)> AnimationProperty<P>::GetUpdateFunction ()
{
    return updateFunction;
}

template<typename P>
std::function<P(P, P, float)> AnimationProperty<P>::GetInterpolateFunction ()
{
    return interpolateFunction;
}