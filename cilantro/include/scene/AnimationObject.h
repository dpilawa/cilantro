#ifndef _ANIMATIONOBJECT_H_
#define _ANIMATIONOBJECT_H_

#include "cilantroengine.h"
#include "resource/ResourceManager.h"
#include "scene/AnimationProperty.h"
#include "scene/GameObject.h"
#include "math/Vector3f.h"
#include "math/Quaternion.h"

namespace cilantro {

class __CEAPI AnimationObject : public GameObject
{
public:
    __EAPI AnimationObject (std::shared_ptr<GameScene> gameScene);
    __EAPI virtual ~AnimationObject ();

    template <typename P>
    __EAPI std::shared_ptr<AnimationProperty<P>> AddAnimationProperty (const std::string& propertyName, P startValue, std::function<void (P)> updateFunction, std::function<P (P, P, float)> interpolateFunc);

    template <typename P>
    ResourceManager<AnimationProperty<P>>& GetProperties ();

    __EAPI void SetTotalTime (float totalTime);

    __EAPI void Play ();
    __EAPI void Stop ();
    __EAPI void Seek (float time);

    __EAPI void SetLooping (float looping);

    __EAPI void OnFrame ();

private:

    template <typename P>
    void UpdateProperties ();

    bool m_isLooping;
    bool m_isPlaying;

    float m_playedTime;
    float m_totalTime;

    ResourceManager<AnimationProperty<float>> m_floatProperties;
    ResourceManager<AnimationProperty<Vector3f>> m_vectorProperties;
    ResourceManager<AnimationProperty<Quaternion>> m_quaternionProperties;
};

} // namespace cilantro

#endif