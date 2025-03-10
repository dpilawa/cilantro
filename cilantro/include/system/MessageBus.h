#ifndef _MESSAGEBUS_H_
#define _MESSAGEBUS_H_

#include <unordered_map>
#include <vector>
#include <functional>
#include <memory>
#include <typeindex>
#include "system/Message.h"

namespace cilantro {

class MessageBus 
{
public:
    MessageBus();
    virtual ~MessageBus();

    // Subscribe to a specific message type
    template <typename T>
    void Subscribe(std::function<void(const std::shared_ptr<T>&)> callback) 
    {
        static_assert(std::is_base_of<Message, T>::value, "T must be a subclass of Message");
        subscribers[typeid(T)].emplace_back([callback](const std::shared_ptr<Message>& msg) 
        {
            callback(std::static_pointer_cast<T>(msg));
        });
    }

    // Publish a message to the bus
    template <typename T>
    void Publish(const std::shared_ptr<T>& message) 
    {
        static_assert(std::is_base_of<Message, T>::value, "T must be a subclass of Message");
        auto it = subscribers.find(typeid(T));
        if (it != subscribers.end()) 
        {
            for (auto& callback : it->second) 
            {
                callback(message);
            }
        }
    }

private:
    // Map to store subscribers for each message type
    std::unordered_map <std::type_index, std::vector <std::function <void (const std::shared_ptr<Message>&)>>> subscribers;

};

} // namespace cilantro

#endif