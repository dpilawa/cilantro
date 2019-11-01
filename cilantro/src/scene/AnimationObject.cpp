#include "scene/AnimationObject.h"
#include "scene/AnimationProperty.h"
#include "util/LogMessage.h"
#include "util/Timer.h"

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
        UpdateProperties<float> ();
        UpdateProperties<Vector3f> ();
        UpdateProperties<Quaternion> ();

        playedTime += Timer::GetFrameDeltaTime ();
    }
}