#ifndef _ANIMATIONOBJECT_H_
#define _ANIMATIONOBJECT_H_

#include "cilantroengine.h"
#include "scene/AnimationProperty.h"
#include "scene/GameObject.h"
#include "math/Vector3f.h"
#include "math/Quaternion.h"
#include <functional>
#include <string>
#include <unordered_map>
#include <memory>

class AnimationObject : public GameObject
{
public:
    __EAPI AnimationObject (GameScene* gameScene);
    __EAPI virtual ~AnimationObject ();

    template <typename P>
    __EAPI void AddAnimationProperty (const std::string& propertyName, P startValue, std::function<void (P)> updateFunction, std::function<P (P, P, float)> interpolateFunc);

    template <typename P>
    __EAPI void AddKeyframe (const std::string& propertyName, float time, P value);
    
    __EAPI void Play ();
    __EAPI void Stop ();
    __EAPI void Seek (float time);

    __EAPI void SetLooping (float looping);

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

#endif