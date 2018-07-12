#ifndef _CALLBACKPROVIDER_H_
#define _CALLBACKPROVIDER_H_

#include <functional>
#include <unordered_map>
#include <vector>

// Class template to enable object callbacks
template<typename K, typename P = unsigned int>
class CallbackProvider
{
public:
	CallbackProvider ();
	~CallbackProvider ();

	// Registers a new callback 
	// Recipient must set callback to one of its functions
	void RegisterCallback (K key, std::function<void (P)> callback);

private:
	// map of callback recipients
	std::unordered_map<K, std::vector <std::function<void (P)>>> callbacks;

protected:
	// Notify callback recipients
	void InvokeCallbacks (K key, P parameter);
};

template<typename K, typename P>
inline CallbackProvider<K, P>::CallbackProvider ()
{
}

template<typename K, typename P>
inline CallbackProvider<K, P>::~CallbackProvider ()
{
}

template<typename K, typename P>
inline void CallbackProvider<K, P>::RegisterCallback (K key, std::function<void (P)> callback)
{
	// push new callback at the end of callbacks vector
	callbacks[key].push_back (callback);
}

template<typename K, typename P>
inline void CallbackProvider<K, P>::InvokeCallbacks (K key, P parameter)
{
	// invoke all registered callbacks
	for (auto&& callback : callbacks[key])
	{
		callback (parameter);
	}
}

#endif

