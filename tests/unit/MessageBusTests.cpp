#include <gtest/gtest.h>
#include "system/Message.h"
#include "system/MessageBus.h"
#include <memory>
#include <string>
#include <vector>

using namespace cilantro;

namespace {

class PingMessage : public Message
{
public:
    explicit PingMessage (int v = 0) : value (v) {}
    int value;
};

class PongMessage : public Message
{
};

class SpecialPingMessage : public PingMessage
{
public:
    explicit SpecialPingMessage (int v = 0) : PingMessage (v) {}
};

} // namespace

TEST (MessageBus, PublishWithoutSubscribersIsHarmless)
{
    MessageBus bus;

    EXPECT_NO_THROW (bus.Publish<PingMessage> (std::make_shared<PingMessage> (1)));
}

TEST (MessageBus, SubscriberReceivesPublishedMessage)
{
    MessageBus bus;
    int received = -1;

    bus.Subscribe<PingMessage> ([&](const std::shared_ptr<PingMessage>& m) { received = m->value; });
    bus.Publish<PingMessage> (std::make_shared<PingMessage> (42));

    EXPECT_EQ (received, 42);
}

TEST (MessageBus, SubscriberReceivesTheSameMessageObject)
{
    MessageBus bus;
    std::shared_ptr<PingMessage> sent = std::make_shared<PingMessage> (1);
    std::shared_ptr<PingMessage> received;

    bus.Subscribe<PingMessage> ([&](const std::shared_ptr<PingMessage>& m) { received = m; });
    bus.Publish<PingMessage> (sent);

    EXPECT_EQ (received, sent);
}

TEST (MessageBus, EverySubscriberIsNotifiedInSubscriptionOrder)
{
    MessageBus bus;
    std::vector<std::string> calls;

    bus.Subscribe<PingMessage> ([&](const std::shared_ptr<PingMessage>&) { calls.push_back ("first"); });
    bus.Subscribe<PingMessage> ([&](const std::shared_ptr<PingMessage>&) { calls.push_back ("second"); });
    bus.Subscribe<PingMessage> ([&](const std::shared_ptr<PingMessage>&) { calls.push_back ("third"); });
    bus.Publish<PingMessage> (std::make_shared<PingMessage> ());

    EXPECT_EQ (calls, (std::vector<std::string> { "first", "second", "third" }));
}

TEST (MessageBus, MessagesAreRoutedByType)
{
    MessageBus bus;
    int pings = 0;
    int pongs = 0;

    bus.Subscribe<PingMessage> ([&](const std::shared_ptr<PingMessage>&) { pings++; });
    bus.Subscribe<PongMessage> ([&](const std::shared_ptr<PongMessage>&) { pongs++; });

    bus.Publish<PingMessage> (std::make_shared<PingMessage> ());
    bus.Publish<PingMessage> (std::make_shared<PingMessage> ());
    bus.Publish<PongMessage> (std::make_shared<PongMessage> ());

    EXPECT_EQ (pings, 2);
    EXPECT_EQ (pongs, 1);
}

TEST (MessageBus, RoutingUsesExactMessageTypeNotBaseClass)
{
    MessageBus bus;
    int basePings = 0;
    int specialPings = 0;

    bus.Subscribe<PingMessage> ([&](const std::shared_ptr<PingMessage>&) { basePings++; });
    bus.Subscribe<SpecialPingMessage> ([&](const std::shared_ptr<SpecialPingMessage>&) { specialPings++; });

    // a derived message is delivered to subscribers of its own type only
    bus.Publish<SpecialPingMessage> (std::make_shared<SpecialPingMessage> ());

    EXPECT_EQ (basePings, 0);
    EXPECT_EQ (specialPings, 1);

    // published through its base type it goes to the base subscribers only
    bus.Publish<PingMessage> (std::make_shared<SpecialPingMessage> ());

    EXPECT_EQ (basePings, 1);
    EXPECT_EQ (specialPings, 1);
}

TEST (MessageBus, EngineMessagesCarryResourceHandle)
{
    MessageBus bus;
    handle_t received = 0;

    bus.Subscribe<MeshObjectUpdateMessage> ([&](const std::shared_ptr<MeshObjectUpdateMessage>& m) { received = m->GetHandle (); });
    bus.Publish<MeshObjectUpdateMessage> (std::make_shared<MeshObjectUpdateMessage> (17));

    EXPECT_EQ (received, 17u);
}

TEST (MessageBus, MaterialTextureUpdateIsNotDeliveredToMaterialUpdateSubscribers)
{
    // MaterialTextureUpdateMessage derives from MaterialUpdateMessage, but routing is by exact type,
    // so a subscriber that wants both has to subscribe twice (the renderer does exactly that)
    MessageBus bus;
    int materialUpdates = 0;
    int textureUpdates = 0;
    int textureUnit = -1;

    bus.Subscribe<MaterialUpdateMessage> ([&](const std::shared_ptr<MaterialUpdateMessage>&) { materialUpdates++; });
    bus.Subscribe<MaterialTextureUpdateMessage> ([&](const std::shared_ptr<MaterialTextureUpdateMessage>& m) { textureUpdates++; textureUnit = m->GetTextureUnit (); });

    bus.Publish<MaterialTextureUpdateMessage> (std::make_shared<MaterialTextureUpdateMessage> (3, 2));

    EXPECT_EQ (materialUpdates, 0);
    EXPECT_EQ (textureUpdates, 1);
    EXPECT_EQ (textureUnit, 2);
}

TEST (MessageBus, InputMessagesCarryEventNameAndValue)
{
    MessageBus bus;
    std::string name;
    float value = 0.0f;

    bus.Subscribe<InputEventMessage> ([&](const std::shared_ptr<InputEventMessage>& m) { name = m->GetEvent (); value = m->GetValue (); });
    bus.Publish<InputEventMessage> (std::make_shared<InputEventMessage> ("jump", 1.0f));

    EXPECT_EQ (name, "jump");
    EXPECT_FLOAT_EQ (value, 1.0f);
}

TEST (MessageBus, SubscriberMaySubscribeToAnotherMessageWhileHandling)
{
    MessageBus bus;
    int pongs = 0;

    bus.Subscribe<PingMessage> ([&](const std::shared_ptr<PingMessage>&)
    {
        bus.Subscribe<PongMessage> ([&](const std::shared_ptr<PongMessage>&) { pongs++; });
    });

    bus.Publish<PingMessage> (std::make_shared<PingMessage> ());
    bus.Publish<PongMessage> (std::make_shared<PongMessage> ());

    EXPECT_EQ (pongs, 1);
}
