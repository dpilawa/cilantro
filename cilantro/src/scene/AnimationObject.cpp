#include "scene/AnimationObject.h"
#include "scene/AnimationProperty.h"
#include "scene/GameScene.h"
#include "system/Timer.h"
#include <functional>
#include <string>
#include <unordered_map>
#include <memory>

namespace cilantro {

// template instantiations
template __EAPI std::shared_ptr<AnimationProperty<float>> AnimationObject::AddAnimationProperty<float> (const std::string& propertyName, float startValue, std::function<void (float)> updateFunction, std::function<float (float, float, float)> interpolateFunction);
template __EAPI std::shared_ptr<AnimationProperty<Vector3f>> AnimationObject::AddAnimationProperty<Vector3f> (const std::string& propertyName, Vector3f startValue, std::function<void (Vector3f)> updateFunction, std::function<Vector3f (Vector3f, Vector3f, float)> interpolateFunction);
template __EAPI std::shared_ptr<AnimationProperty<Quaternion>> AnimationObject::AddAnimationProperty<Quaternion> (const std::string& propertyName, Quaternion startValue, std::function<void (Quaternion)> updateFunction, std::function<Quaternion (Quaternion, Quaternion, float)> interpolateFunction);

template __EAPI void AnimationObject::UpdateProperties<float> ();
template __EAPI void AnimationObject::UpdateProperties<Vector3f> ();
template __EAPI void AnimationObject::UpdateProperties<Quaternion> ();

AnimationObject::AnimationObject (std::shared_ptr<GameScene> gameScene) : GameObject (gameScene)
{
    m_isPlaying = false;
    m_isLooping = true;
    m_playedTime = 0.0f;
    m_totalTime = 0.0f;
}

AnimationObject::~AnimationObject()
{
}

void AnimationObject::Play ()
{
    m_isPlaying = true;
}

void AnimationObject::Stop ()
{
    m_isPlaying = false;
}

void AnimationObject::Seek (float time)
{
    m_playedTime = time;
}

void AnimationObject::SetLooping (float looping)
{
    m_isLooping = looping;
}

void AnimationObject::OnFrame ()
{
    GameObject::OnFrame ();

    if (m_playedTime > m_totalTime)
    {
        if (m_isLooping)
        {
            m_playedTime = std::max (0.0f, m_playedTime - m_totalTime);
        }
        else
        {
            m_playedTime = m_totalTime;
            m_isPlaying = false;

            return;
        }
    }

    if (m_isPlaying)
    {
        UpdateProperties<float> ();
        UpdateProperties<Vector3f> ();
        UpdateProperties<Quaternion> ();

        m_playedTime += m_gameScene.lock ()->GetTimer ()->GetFrameDeltaTime ();
    }
}

template <typename P>
std::shared_ptr<AnimationProperty<P>> AnimationObject::AddAnimationProperty (const std::string& propertyName, P startValue, std::function<void (P)> updateFunction, std::function<P (P, P, float)> interpolateFunction)
{
    auto property = GetProperties<P> ().template Create<AnimationProperty<P>> (propertyName, std::static_pointer_cast<AnimationObject> (shared_from_this ()), updateFunction, interpolateFunction);
    property->AddKeyframe (0.0f, startValue);

    return property;
}

template <>
ResourceManager<AnimationProperty<float>>& AnimationObject::GetProperties<float> ()
{
    return m_floatProperties;
}

template <>
ResourceManager<AnimationProperty<Vector3f>>& AnimationObject::GetProperties<Vector3f> ()
{
    return m_vectorProperties;
}

template <>
ResourceManager<AnimationProperty<Quaternion>>& AnimationObject::GetProperties<Quaternion> ()
{
    return m_quaternionProperties;
}

void AnimationObject::SetTotalTime (float totalTime)
{
    m_totalTime = std::max (m_totalTime, totalTime);
}

template <typename P>
void AnimationObject::UpdateProperties ()
{
    for (auto&& property : GetProperties<P> ())
    {
        property->GetUpdateFunction () (property->GetFrame (m_playedTime));
    }
}

} // namespace cilantro

