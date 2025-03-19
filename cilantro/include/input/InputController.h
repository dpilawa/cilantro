#ifndef _INPUTCONTROLLER_H_
#define _INPUTCONTROLLER_H_

#include "cilantroengine.h"
#include "scene/GameScene.h"
#include "input/Input.h"
#include <string>
#include <functional>
#include <unordered_map>
#include <set>
#include <vector>

namespace cilantro {

class __CEAPI InputController
{
public:
    __EAPI InputController ();
    __EAPI virtual ~InputController ();

    __EAPI virtual void Initialize () = 0;
    __EAPI virtual void Deinitialize () = 0;

    __EAPI virtual void OnFrame ();

    __EAPI virtual std::shared_ptr<Input<bool>> CreateInputEvent (const std::string& name);
    __EAPI virtual std::shared_ptr<Input<bool>> CreateInputEvent (const std::string& name, EInputKey key, EInputTrigger trigger, std::set<EInputModifier> modifiers) = 0;

    __EAPI virtual std::shared_ptr<Input<float>>  CreateInputAxis (const std::string& name, float scale);
    __EAPI virtual std::shared_ptr<Input<float>> CreateInputAxis (const std::string& name, EInputKey key, std::set<EInputModifier> modifiers, float scale) = 0;
    __EAPI virtual std::shared_ptr<Input<float>>  CreateInputAxis (const std::string& name, EInputAxis value, float scale) = 0;

    __EAPI void BindInputEvent (const std::string& name, std::function<void ()>);
    __EAPI void BindInputAxis (const std::string& name, std::function<void (float)>);

    __EAPI virtual void SetMouseGameMode (bool value) = 0;
    __EAPI bool IsGameMode ();

private: 

    std::vector<std::shared_ptr<Input<bool>>> events;
    std::unordered_map<std::string, std::vector<std::shared_ptr<Input<float>>>> axes;

protected:

    bool isGameMode;

};

} // namespace cilantro

#endif // _INPUTCONTROLLER_H_
