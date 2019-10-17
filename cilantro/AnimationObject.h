#ifndef _ANIMATIONOBJECT_H_
#define _ANIMATIONOBJECT_H_

#include "cilantroengine.h"
#include "AnimationProperty.h"
#include "GameObject.h"
#include "Vector3f.h"
#include "Quaternion.h"
#include <functional>
#include <string>
#include <unordered_map>
#include <memory>

class AnimationObject : public GameObject
{
public:
    __EAPI AnimationObject ();
    __EAPI virtual ~AnimationObject ();

    template <typename P>
    void AddAnimationProperty (std::string propertyName, P startValue, std::function<void (P)> updateFunction, std::function<P (P, P, float)> interpolateFunc);

    template <typename P>
    void AddKeyframe (std::string propertyName, float time, P value);
    
    __EAPI void Play ();
    __EAPI void Stop ();
    __EAPI void Seek (float time);

    __EAPI void SetLooping (float looping);

    __EAPI float getPlayedTime ();

    void OnFrame ();

private:

    template <typename P>
    void UpdateProperties ();

    template <typename P>
    std::unordered_map<std::string, std::shared_ptr<AnimationProperty<P>>>& GetProperties () = delete;

    bool isLooping;
    bool isPlaying;

    float playedTime;
    float maxAnimationTime;

    std::unordered_map<std::string, std::shared_ptr<AnimationProperty<float>>> floatProperties;
    std::unordered_map<std::string, std::shared_ptr<AnimationProperty<Vector3f>>> vectorProperties;
    std::unordered_map<std::string, std::shared_ptr<AnimationProperty<Quaternion>>> quaternionProperties;
};

template <typename P>
void AnimationObject::AddAnimationProperty (std::string propertyName, P startValue, std::function<void (P)> updateFunction, std::function<P (P, P, float)> interpolateFunction)
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
        LogMessage (__func__, EXIT_FAILURE) << "Animation property" << propertyName << "already exists for this AnimationObject";
    }
}

template <typename P>
void AnimationObject::AddKeyframe (std::string propertyName, float time, P value)
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
            LogMessage (__func__, EXIT_FAILURE) << "Unable to insert keyframe in property" << propertyName << "(t =" << time << ")";
        }
    }
    else
    {
        LogMessage (__func__, EXIT_FAILURE) << "Animation property" << propertyName << "does not exist";
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

#endif