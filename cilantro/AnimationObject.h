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

    __EAPI void AddAnimationProperty (std::string propertyName, float startValue, std::function<void (float)> updateFunc, std::function<float (float, float, float)> interpolateFunc);
    __EAPI void AddAnimationProperty (std::string propertyName, Vector3f startValue, std::function<void (Vector3f)> updateFunc, std::function<Vector3f (Vector3f, Vector3f, float)> interpolateFunc);
    __EAPI void AddAnimationProperty (std::string propertyName, Quaternion startValue, std::function<void (Quaternion)> updateFunc, std::function<Quaternion (Quaternion, Quaternion, float)> interpolateFunc);

    __EAPI void AddKeyframe (std::string propertyName, float time, float value);
    __EAPI void AddKeyframe (std::string propertyName, float time, Vector3f value);
    __EAPI void AddKeyframe (std::string propertyName, float time, Quaternion value);

    __EAPI void Play ();
    __EAPI void Stop ();
    __EAPI void Seek (float time);

    __EAPI void SetLooping (float looping);

    __EAPI float getPlayedTime ();

    void OnFrame ();

private:
    
    bool isLooping;
    bool isPlaying;

    float playedTime;
    float maxAnimationTime;

    std::unordered_map<std::string, std::shared_ptr<AnimationProperty<float>>> floatProperties;
    std::unordered_map<std::string, std::shared_ptr<AnimationProperty<Vector3f>>> vectorProperties;
    std::unordered_map<std::string, std::shared_ptr<AnimationProperty<Quaternion>>> quaternionProperties;
};

#endif