#include "AnimationObject.h"
#include "AnimationProperty.h"
#include "LogMessage.h"
#include "Timer.h"

AnimationObject::AnimationObject()
{
    isPlaying = false;
    isLooping = true;
    playedTime = 0.0f;
    maxAnimationTime = 0.0f;
}

AnimationObject::~AnimationObject()
{
}

void AnimationObject::AddAnimationProperty (std::string propertyName, float startValue, std::function<void (float)> updateFunction, std::function<float (float, float, float)> interpolateFunction)
{
    auto find = floatProperties.find (propertyName);
    std::shared_ptr<AnimationProperty<float>> property;

    if (find == floatProperties.end())
    {
        property = std::make_shared<AnimationProperty<float>> (updateFunction, interpolateFunction);
        floatProperties[propertyName] = property;
        property->AddKeyframe (0.0f, startValue);
    }
    else
    {
        LogMessage (__func__, EXIT_FAILURE) << "Animation float property" << propertyName << "already exists for this AnimationObject";
    }
}

void AnimationObject::AddAnimationProperty (std::string propertyName, Vector3f startValue, std::function<void (Vector3f)> updateFunction, std::function<Vector3f (Vector3f, Vector3f, float)> interpolateFunction)
{
    auto find = vectorProperties.find (propertyName);
    std::shared_ptr<AnimationProperty<Vector3f>> property;

    if (find == vectorProperties.end())
    {
        property = std::make_shared<AnimationProperty<Vector3f>> (updateFunction, interpolateFunction);
        vectorProperties[propertyName] = property;
        property->AddKeyframe (0.0f, startValue);
    }
    else
    {
        LogMessage (__func__, EXIT_FAILURE) << "Animation vector property" << propertyName << "already exists for this AnimationObject";
    }
}

void AnimationObject::AddAnimationProperty (std::string propertyName, Quaternion startValue, std::function<void (Quaternion)> updateFunction, std::function<Quaternion (Quaternion, Quaternion, float)> interpolateFunction)
{
    auto find = quaternionProperties.find (propertyName);
    std::shared_ptr<AnimationProperty<Quaternion>> property;

    if (find == quaternionProperties.end())
    {
        property = std::make_shared<AnimationProperty<Quaternion>> (updateFunction, interpolateFunction);
        quaternionProperties[propertyName] = property;
        property->AddKeyframe (0.0f, startValue);
    }
    else
    {
        LogMessage (__func__, EXIT_FAILURE) << "Animation quaternion property" << propertyName << "already exists for this AnimationObject";
    }
}

void AnimationObject::AddKeyframe (std::string propertyName, float time, float value)
{
    auto find = floatProperties.find (propertyName);

    if (find != floatProperties.end ())
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
            LogMessage (__func__, EXIT_FAILURE) << "Unable to insert keyframe in property" << propertyName << "(t=" << time << ")";
        }
    }
    else
    {
        LogMessage (__func__, EXIT_FAILURE) << "Animation property" << propertyName << "does not exist";
    }
}

void AnimationObject::AddKeyframe (std::string propertyName, float time, Vector3f value)
{
    auto find = vectorProperties.find (propertyName);

    if (find != vectorProperties.end ())
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
            LogMessage (__func__, EXIT_FAILURE) << "Unable to insert keyframe in property" << propertyName << "(t=" << time << ")";
        }
    }
    else
    {
        LogMessage (__func__, EXIT_FAILURE) << "Animation property" << propertyName << "does not exist";
    }
}

void AnimationObject::AddKeyframe (std::string propertyName, float time, Quaternion value)
{
    auto find = quaternionProperties.find (propertyName);

    if (find != quaternionProperties.end ())
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
            LogMessage (__func__, EXIT_FAILURE) << "Unable to insert keyframe in property" << propertyName << "(t=" << time << ")";
        }
    }
    else
    {
        LogMessage (__func__, EXIT_FAILURE) << "Animation property" << propertyName << "does not exist";
    }
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
            playedTime = playedTime - maxAnimationTime;
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
        for (auto&& property : floatProperties)
        {
            std::string propertyName = property.first;
            std::shared_ptr<AnimationProperty<float>> propertyPtr = property.second;

            propertyPtr->GetUpdateFunction () (propertyPtr->GetFrame (playedTime));
        }

        for (auto&& property : vectorProperties)
        {
            std::string propertyName = property.first;
            std::shared_ptr<AnimationProperty<Vector3f>> propertyPtr = property.second;

            propertyPtr->GetUpdateFunction () (propertyPtr->GetFrame (playedTime));
        }

        for (auto&& property : quaternionProperties)
        {
            std::string propertyName = property.first;
            std::shared_ptr<AnimationProperty<Quaternion>> propertyPtr = property.second;

            propertyPtr->GetUpdateFunction () (propertyPtr->GetFrame (playedTime));
        }

        playedTime += Timer::GetFrameDeltaTime ();
    }
}