#ifndef _MESSAGEBUS_H_
#define _MESSAGEBUS_H_

#include <algorithm>
#include <unordered_map>
#include <vector>
#include <functional>
#include <memory>
#include <typeindex>
#include "system/Message.h"

namespace cilantro {

class __CEAPI MessageBus
{
    struct Entry
    {
        std::function <void (const std::shared_ptr<Message>&)> callback;
        bool active = true;
    };

    struct Registry
    {
        std::unordered_map <std::type_index, std::vector <std::shared_ptr<Entry>>> subscribers;
    };

public:
    // Handle of a subscription. It does not unsubscribe when destroyed, so callers that want the callback to stay
    // subscribed for the lifetime of the bus can ignore it. It may outlive the bus.
    class Subscription
    {
    public:
        Subscription () : m_type (typeid (Message)) {}

        // true while the callback is subscribed
        bool IsActive () const
        {
            auto entry = m_entry.lock ();
            return entry != nullptr && entry->active;
        }

        // stop receiving messages (does nothing if already unsubscribed or if the bus no longer exists)
        void Unsubscribe ()
        {
            auto entry = m_entry.lock ();
            auto registry = m_registry.lock ();

            if (entry != nullptr)
            {
                // skipped even when a publish that has already started is about to call it
                entry->active = false;
            }

            if (registry != nullptr && entry != nullptr)
            {
                auto it = registry->subscribers.find (m_type);
                if (it != registry->subscribers.end ())
                {
                    auto& entries = it->second;
                    entries.erase (std::remove (entries.begin (), entries.end (), entry), entries.end ());
                }
            }

            m_registry.reset ();
            m_entry.reset ();
        }

    private:
        friend class MessageBus;

        Subscription (std::weak_ptr<Registry> registry, std::type_index type, std::weak_ptr<Entry> entry)
            : m_registry (registry), m_type (type), m_entry (entry) {}

        std::weak_ptr<Registry> m_registry;
        std::type_index m_type;
        std::weak_ptr<Entry> m_entry;
    };

    // Owner of a subscription: unsubscribes when destroyed or reassigned. An object whose callback captures
    // the object itself should keep one as a member, so that the callback never outlives the object.
    class ScopedSubscription
    {
    public:
        ScopedSubscription () = default;
        ScopedSubscription (const Subscription& subscription) : m_subscription (subscription) {}
        ~ScopedSubscription () { m_subscription.Unsubscribe (); }

        ScopedSubscription (const ScopedSubscription&) = delete;
        ScopedSubscription& operator= (const ScopedSubscription&) = delete;

        ScopedSubscription (ScopedSubscription&& other) noexcept : m_subscription (other.m_subscription)
        {
            other.m_subscription = Subscription ();
        }

        ScopedSubscription& operator= (ScopedSubscription&& other) noexcept
        {
            if (this != &other)
            {
                m_subscription.Unsubscribe ();
                m_subscription = other.m_subscription;
                other.m_subscription = Subscription ();
            }
            return *this;
        }

        ScopedSubscription& operator= (const Subscription& subscription)
        {
            m_subscription.Unsubscribe ();
            m_subscription = subscription;
            return *this;
        }

        bool IsActive () const { return m_subscription.IsActive (); }
        void Unsubscribe () { m_subscription.Unsubscribe (); }

    private:
        Subscription m_subscription;
    };

    __EAPI MessageBus();
    __EAPI virtual ~MessageBus();

    // Subscribe to a specific message type
    // Messages are routed by exact type. The returned handle may be ignored (the callback stays subscribed
    // for the lifetime of the bus). A callback subscribed while a message is being published
    // receives only the following messages.
    template <typename T>
    Subscription Subscribe(std::function<void(const std::shared_ptr<T>&)> callback)
    {
        static_assert(std::is_base_of<Message, T>::value, "T must be a subclass of Message");

        auto entry = std::make_shared<Entry>();
        entry->callback = [callback](const std::shared_ptr<Message>& msg)
        {
            callback(std::static_pointer_cast<T>(msg));
        };

        m_registry->subscribers[typeid(T)].push_back(entry);

        return Subscription (m_registry, typeid(T), entry);
    }

    // Publish a message to the bus
    template <typename T>
    void Publish(const std::shared_ptr<T>& message)
    {
        static_assert(std::is_base_of<Message, T>::value, "T must be a subclass of Message");

        auto it = m_registry->subscribers.find(typeid(T));
        if (it != m_registry->subscribers.end())
        {
            // callbacks may subscribe or unsubscribe while handling the message, so iterate over a snapshot
            auto entries = it->second;

            for (auto& entry : entries)
            {
                if (entry->active)
                {
                    entry->callback(message);
                }
            }
        }
    }

private:
    std::shared_ptr<Registry> m_registry;

};

} // namespace cilantro

#endif
