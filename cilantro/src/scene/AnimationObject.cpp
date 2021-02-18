#include "scene/AnimationObject.h"
#include "scene/AnimationProperty.h"
#include "system/LogMessage.h"
#include "system/Timer.h"

// template instantiations
template __EAPI void AnimationObject::AddAnimationProperty<float> (const std::string& propertyName, float startValue, std::function<void (float)> updateFunction, std::function<float (float, float, float)> interpolateFunction);
template __EAPI void AnimationObject::AddAnimationProperty<Vector3f> (const std::string& propertyName, Vector3f startValue, std::function<void (Vector3f)> updateFunction, std::function<Vector3f (Vector3f, Vector3f, float)> interpolateFunction);
template __EAPI void AnimationObject::AddAnimationProperty<Quaternion> (const std::string& propertyName, Quaternion startValue, std::function<void (Quaternion)> updateFunction, std::function<Quaternion (Quaternion, Quaternion, float)> interpolateFunction);

template __EAPI void AnimationObject::AddKeyframe<float> (const std::string& propertyName, float time, float value);
template __EAPI void AnimationObject::AddKeyframe<Vector3f> (const std::string& propertyName, float time, Vector3f value);
template __EAPI void AnimationObject::AddKeyframe<Quaternion> (const std::string& propertyName, float time, Quaternion value);

template __EAPI void AnimationObject::UpdateProperties<float> ();
template __EAPI void AnimationObject::UpdateProperties<Vector3f> ();
template __EAPI void AnimationObject::UpdateProperties<Quaternion> ();

AnimationObject::AnimationObject ()
{
    isPlaying = false;
    isLooping = true;
    playedTime = 0.0f;
    maxAnimationTime = 0.0f;
}

AnimationObject::~AnimationObject()
{
}

void AnimationObject::Play ()
{
    isPlaying = true;
}

void AnimationObject::Stop ()
{
    isPlaying = false;
}

void AnimationObject::Seek (float time)
{
    playedTime = time;
}

void AnimationObject::SetLooping (float looping)
{
    isLooping = looping;
}

void AnimationObject::OnFrame ()
{
    GameObject::OnFrame ();

    if (playedTime > maxAnimationTime)
    {
        if (isLooping)
        {
            playedTime = std::max (0.0f, playedTime - maxAnimationTime);
        }
        else
        {
            playedTime = maxAnimationTime;
            isPlaying = false;

            return;
        }
    }

    if (isPlaying)
    {
        UpdateProperties<float> ();
        UpdateProperties<Vector3f> ();
        UpdateProperties<Quaternion> ();

        playedTime += Timer::GetFrameDeltaTime ();
    }
}

template <typename P>
void AnimationObject::AddAnimationProperty (const std::string& propertyName, P startValue, std::function<void (P)> updateFunction, std::function<P (P, P, float)> interpolateFunction)
{
    auto find = GetProperties<P> ().find (propertyName);
    std::shared_ptr<AnimationProperty<P>> property;

    if (find == GetProperties<P> ().end())
    {
        property = std::make_shared<AnimationProperty<P>> (updateFunction, interpolateFunction);
        GetProperties<P> ()[propertyName] = property;
        property->AddKeyframe (0.0f, startValue);
    }
    else
    {
        LogMessage (MSG_LOCATION, EXIT_FAILURE) << "Animation property" << propertyName << "already exists for this AnimationObject";
    }
}

template <typename P>
void AnimationObject::AddKeyframe (const std::string& propertyName, float time, P value)
{
    auto find = GetProperties<P> ().find (propertyName);

    if (find != GetProperties<P> ().end ())
    {
        if (find->second->AddKeyframe (time, value))
        {
            if (time > maxAnimationTime)
            {
                maxAnimationTime = time;
            }
        }
        else
        {
            LogMessage (MSG_LOCATION, EXIT_FAILURE) << "Unable to insert keyframe in property" << propertyName << "(t =" << time << ")";
        }
    }
    else
    {
        LogMessage (MSG_LOCATION, EXIT_FAILURE) << "Animation property" << propertyName << "does not exist";
    }
}

template <typename P>
void AnimationObject::UpdateProperties ()
{
    for (auto&& property : GetProperties<P> ())
    {
        std::shared_ptr<AnimationProperty<P>> propertyPtr = property.second;
        propertyPtr->GetUpdateFunction () (propertyPtr->GetFrame (playedTime));
    }
}

template <>
std::unordered_map<std::string, std::shared_ptr<AnimationProperty<float>>>& AnimationObject::GetProperties<float> ()
{
    return floatProperties;
};

template <>
std::unordered_map<std::string, std::shared_ptr<AnimationProperty<Vector3f>>>& AnimationObject::GetProperties<Vector3f> ()
{
    return vectorProperties;
};

template <>
std::unordered_map<std::string, std::shared_ptr<AnimationProperty<Quaternion>>>& AnimationObject::GetProperties<Quaternion> ()
{
    return quaternionProperties;
};

