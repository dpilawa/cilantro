#ifndef _CALLBACKPROVIDER_H_
#define _CALLBACKPROVIDER_H_

#include <vector>
#include <functional>

// Class template to enable object callbacks
// This has the following sample uses in the engine:
// - GameScene being notified by GameObject about its changes (to notify Renderer)
// - Renderer being notified by GameScene about GameObject changes (to reload object into GPU memory)
// Template types: P - callback parameter; all callbacks return void
template<typename P>
class CallbackProvider
{
public:
	CallbackProvider ();
	~CallbackProvider ();

	// Registers a new callback 
	// Recipient must set callback to one of its functions
	void RegisterCallback (std::function<void (P)> callback);

private:
	// vector of callback recipients
	std::vector <std::function<void (P)>> callbacks;

protected:
	// Notify callback recipients
	void InvokeCallbacks (P parameter);
};

template<typename P>
inline CallbackProvider<P>::CallbackProvider ()
{
}

template<typename P>
inline CallbackProvider<P>::~CallbackProvider ()
{
}

template<typename P>
inline void CallbackProvider<P>::RegisterCallback (std::function<void (P)> callback)
{
	// push new callback at the end of callbacks vector
	callbacks.push_back (callback);
}

template<typename P>
inline void CallbackProvider<P>::InvokeCallbacks (P parameter)
{
	// invoke all registered callbacks
	for (auto&& callback : callbacks)
	{
		callback (parameter);
	}
}

#endif

