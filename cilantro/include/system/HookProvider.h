#ifndef _HOOKPROVIDER_H_
#define _HOOKPROVIDER_H_

#include <functional>
#include <unordered_map>
#include <vector>

// Class template to enable object hooks
template<typename Key, typename... Params>
class HookProvider
{
public:
    HookProvider () = default;
    ~HookProvider () = default;

    // Registers a new hook and subscribes a callback function
    void SubscribeHook (Key hook, std::function<void (Params... params)> function);

    // Notify hooks subscribers
    void InvokeHook (Key hook, Params... params);

private:
    // map of hook subscribers
    std::unordered_map<Key, std::vector <std::function<void (Params... params)>>> hooks;
};

template<typename Key, typename... Params>
inline void HookProvider<Key, Params...>::SubscribeHook (Key hook, std::function<void (Params... params)> function)
{
    // push new hook at the end of callbacks vector
    hooks[hook].push_back (function);
}

template<typename Key, typename... Params>
inline void HookProvider<Key, Params...>::InvokeHook (Key hook, Params... params)
{
    // invoke all registered callbacks of given hook
    for (auto&& callback : hooks[hook])
    {
        callback (params...);
    }
}

#endif

