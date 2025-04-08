#include "scene/AnimationProperty.h"
#include "scene/AnimationObject.h"
#include "math/Vector3f.h"
#include "math/Quaternion.h"

namespace cilantro {

template<typename P>
AnimationProperty<P>::AnimationProperty (std::shared_ptr<AnimationObject> animationObject, std::function<void (P)> updateFunction, std::function<P (P, P, float)> interpolateFunction)
: m_animationObject (animationObject), m_updateFunction (updateFunction), m_interpolateFunction (interpolateFunction)
{
    m_maxKeyframeTime = 0.0f;
}

template<typename P>
std::shared_ptr<AnimationProperty<P>> AnimationProperty<P>::AddKeyframe (float time, P propertyValue)
{
    auto result = m_keyframes.insert (std::make_pair (time, propertyValue));
    if (result.second == true)
    {
        if (time > m_maxKeyframeTime)
        {
            m_maxKeyframeTime = time;
            m_animationObject.lock ()->SetTotalTime (m_maxKeyframeTime);
        }
        return std::dynamic_pointer_cast<AnimationProperty<P>> (this->shared_from_this ());
    }
    else
    {
        LogMessage (MSG_LOCATION, EXIT_FAILURE) << "Unable to add keyframe at time" << time << "for property" << this->GetName ();
    }

    return nullptr;
}

template<typename P>
P AnimationProperty<P>::GetFrame (float time)
{
    if (m_keyframes.empty ())
    {
        LogMessage (MSG_LOCATION, EXIT_FAILURE) << "Keyframe collection is empty";
        return P ();
    }

    if ((time > m_maxKeyframeTime) || (m_keyframes.size() == 1))
    {
        return m_keyframes.rbegin ()->second;
    }
    else 
    {
        typename std::map<float, P>::iterator i0 = --m_keyframes.upper_bound (time);
        typename std::map<float, P>::iterator i1 = m_keyframes.upper_bound (time);

        float t0 = i0->first;
        float t1 = i1->first;

        P p0 = i0->second;
        P p1 = i1->second;

        return m_interpolateFunction (p0, p1, (time - t0) / (t1 - t0));
    }
}

template<typename P>
std::function<void(P)> AnimationProperty<P>::GetUpdateFunction ()
{
    return m_updateFunction;
}

template<typename P>
std::function<P(P, P, float)> AnimationProperty<P>::GetInterpolateFunction ()
{
    return m_interpolateFunction;
}

// template instantiations
template class AnimationProperty<float>;
template class AnimationProperty<Vector3f>;
template class AnimationProperty<Quaternion>;

} // namespace cilantro
