#ifndef _CALLBACKPROVIDER_H_
#define _CALLBACKPROVIDER_H_

#include <functional>
#include <unordered_map>
#include <vector>

// Class template to enable object callbacks
template<typename Key, typename... Params>
class CallbackProvider
{
public:
    CallbackProvider ();
    ~CallbackProvider ();

    // Registers a new callback
    // Recipient must set callback to one of its functions
    void RegisterCallback (Key key, std::function<void (Params... params)> callback);

private:
    // map of callback recipients
    std::unordered_map<Key, std::vector <std::function<void (Params... params)>>> callbacks;

protected:
    // Notify callback recipients
    void InvokeCallbacks (Key key, Params... params);
};

template<typename Key, typename... Params>
inline CallbackProvider<Key, Params...>::CallbackProvider ()
{
}

template<typename Key, typename... Params>
inline CallbackProvider<Key, Params...>::~CallbackProvider ()
{
}

template<typename Key, typename... Params>
inline void CallbackProvider<Key, Params...>::RegisterCallback (Key key, std::function<void (Params... params)> callback)
{
    // push new callback at the end of callbacks vector
    callbacks[key].push_back (callback);
}

template<typename Key, typename... Params>
inline void CallbackProvider<Key, Params...>::InvokeCallbacks (Key key, Params... params)
{
    // invoke all registered callbacks of given key
    for (auto&& callback : callbacks[key])
    {
        callback (params...);
    }
}

#endif

