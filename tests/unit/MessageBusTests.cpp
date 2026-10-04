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

// ---------------------------------------------------------------------------
// Subscription handles
// ---------------------------------------------------------------------------

TEST (MessageBusSubscription, DefaultHandleIsInactiveAndHarmless)
{
    MessageBus::Subscription subscription;

    EXPECT_FALSE (subscription.IsActive ());
    EXPECT_NO_THROW (subscription.Unsubscribe ());
}

TEST (MessageBusSubscription, HandleIsActiveWhileSubscribed)
{
    MessageBus bus;

    auto subscription = bus.Subscribe<PingMessage> ([](const std::shared_ptr<PingMessage>&) {});

    EXPECT_TRUE (subscription.IsActive ());
}

TEST (MessageBusSubscription, ExplicitUnsubscribeStopsDelivery)
{
    MessageBus bus;
    int calls = 0;

    auto subscription = bus.Subscribe<PingMessage> ([&](const std::shared_ptr<PingMessage>&) { calls++; });
    subscription.Unsubscribe ();

    bus.Publish<PingMessage> (std::make_shared<PingMessage> ());

    EXPECT_EQ (calls, 0);
    EXPECT_FALSE (subscription.IsActive ());
}

TEST (MessageBusSubscription, UnsubscribingTwiceIsHarmless)
{
    MessageBus bus;
    auto subscription = bus.Subscribe<PingMessage> ([](const std::shared_ptr<PingMessage>&) {});

    subscription.Unsubscribe ();

    EXPECT_NO_THROW (subscription.Unsubscribe ());
}

TEST (MessageBusSubscription, UnsubscribingOneKeepsOtherSubscribers)
{
    MessageBus bus;
    int first = 0;
    int second = 0;

    auto a = bus.Subscribe<PingMessage> ([&](const std::shared_ptr<PingMessage>&) { first++; });
    auto b = bus.Subscribe<PingMessage> ([&](const std::shared_ptr<PingMessage>&) { second++; });
    a.Unsubscribe ();

    bus.Publish<PingMessage> (std::make_shared<PingMessage> ());

    EXPECT_EQ (first, 0);
    EXPECT_EQ (second, 1);
}

TEST (MessageBusSubscription, HandleMayOutliveTheBus)
{
    MessageBus::Subscription subscription;

    {
        MessageBus bus;
        subscription = bus.Subscribe<PingMessage> ([](const std::shared_ptr<PingMessage>&) {});
    }

    EXPECT_NO_THROW (subscription.Unsubscribe ());
    EXPECT_FALSE (subscription.IsActive ());
}

TEST (MessageBusSubscription, SubscribingToSameTypeWhilePublishingIsSafe)
{
    MessageBus bus;
    int outer = 0;
    int inner = 0;
    std::vector<MessageBus::Subscription> keep;

    keep.push_back (bus.Subscribe<PingMessage> ([&](const std::shared_ptr<PingMessage>&)
    {
        outer++;
        if (outer == 1)
        {
            keep.push_back (bus.Subscribe<PingMessage> ([&](const std::shared_ptr<PingMessage>&) { inner++; }));
        }
    }));

    bus.Publish<PingMessage> (std::make_shared<PingMessage> ());

    // the new callback does not see the message that was being published
    EXPECT_EQ (outer, 1);
    EXPECT_EQ (inner, 0);

    bus.Publish<PingMessage> (std::make_shared<PingMessage> ());

    EXPECT_EQ (outer, 2);
    EXPECT_EQ (inner, 1);
}

TEST (MessageBusSubscription, UnsubscribingAnotherSubscriberWhilePublishingSkipsIt)
{
    MessageBus bus;
    int second = 0;
    MessageBus::Subscription secondSubscription;

    auto first = bus.Subscribe<PingMessage> ([&](const std::shared_ptr<PingMessage>&) { secondSubscription.Unsubscribe (); });
    secondSubscription = bus.Subscribe<PingMessage> ([&](const std::shared_ptr<PingMessage>&) { second++; });

    bus.Publish<PingMessage> (std::make_shared<PingMessage> ());

    EXPECT_EQ (second, 0);
}

TEST (MessageBusSubscription, SubscriberMayUnsubscribeItselfWhileHandling)
{
    MessageBus bus;
    int calls = 0;
    MessageBus::Subscription subscription;

    subscription = bus.Subscribe<PingMessage> ([&](const std::shared_ptr<PingMessage>&)
    {
        calls++;
        subscription.Unsubscribe ();
    });

    bus.Publish<PingMessage> (std::make_shared<PingMessage> ());
    bus.Publish<PingMessage> (std::make_shared<PingMessage> ());

    EXPECT_EQ (calls, 1);
}

TEST (MessageBusSubscription, IgnoredHandleKeepsCallbackSubscribed)
{
    // the plain handle does not own the subscription, ignoring it (as engine code does) keeps the callback alive
    MessageBus bus;
    int calls = 0;

    bus.Subscribe<PingMessage> ([&](const std::shared_ptr<PingMessage>&) { calls++; });
    bus.Publish<PingMessage> (std::make_shared<PingMessage> ());
    bus.Publish<PingMessage> (std::make_shared<PingMessage> ());

    EXPECT_EQ (calls, 2);
}

TEST (MessageBusSubscription, CopiedHandlesControlTheSameSubscription)
{
    MessageBus bus;
    int calls = 0;

    auto handle = bus.Subscribe<PingMessage> ([&](const std::shared_ptr<PingMessage>&) { calls++; });
    auto copy = handle;
    copy.Unsubscribe ();

    bus.Publish<PingMessage> (std::make_shared<PingMessage> ());

    EXPECT_EQ (calls, 0);
    EXPECT_FALSE (handle.IsActive ());
}

// ---------------------------------------------------------------------------
// Scoped subscriptions
// ---------------------------------------------------------------------------

TEST (MessageBusScopedSubscription, DefaultIsInactiveAndHarmless)
{
    MessageBus::ScopedSubscription scoped;

    EXPECT_FALSE (scoped.IsActive ());
    EXPECT_NO_THROW (scoped.Unsubscribe ());
}

TEST (MessageBusScopedSubscription, DestructionUnsubscribes)
{
    MessageBus bus;
    int calls = 0;

    {
        MessageBus::ScopedSubscription scoped = bus.Subscribe<PingMessage> ([&](const std::shared_ptr<PingMessage>&) { calls++; });
        EXPECT_TRUE (scoped.IsActive ());

        bus.Publish<PingMessage> (std::make_shared<PingMessage> ());
        EXPECT_EQ (calls, 1);
    }

    bus.Publish<PingMessage> (std::make_shared<PingMessage> ());
    EXPECT_EQ (calls, 1);
}

TEST (MessageBusScopedSubscription, MovingTransfersOwnership)
{
    MessageBus bus;
    int calls = 0;

    MessageBus::ScopedSubscription original = bus.Subscribe<PingMessage> ([&](const std::shared_ptr<PingMessage>&) { calls++; });
    MessageBus::ScopedSubscription moved = std::move (original);

    EXPECT_FALSE (original.IsActive ());
    EXPECT_TRUE (moved.IsActive ());

    // destroying the moved-from object must not cancel the subscription
    original.Unsubscribe ();
    bus.Publish<PingMessage> (std::make_shared<PingMessage> ());
    EXPECT_EQ (calls, 1);
}

TEST (MessageBusScopedSubscription, MovedFromDestructionKeepsSubscription)
{
    MessageBus bus;
    int calls = 0;
    MessageBus::ScopedSubscription owner;

    {
        MessageBus::ScopedSubscription temporary = bus.Subscribe<PingMessage> ([&](const std::shared_ptr<PingMessage>&) { calls++; });
        owner = std::move (temporary);
    }

    bus.Publish<PingMessage> (std::make_shared<PingMessage> ());
    EXPECT_EQ (calls, 1);
}

TEST (MessageBusScopedSubscription, AssigningUnsubscribesPreviousCallback)
{
    // this is what a stage does when it is initialized twice: only the latest callback may stay
    MessageBus bus;
    int oldCalls = 0;
    int newCalls = 0;

    MessageBus::ScopedSubscription scoped;
    scoped = bus.Subscribe<PingMessage> ([&](const std::shared_ptr<PingMessage>&) { oldCalls++; });
    scoped = bus.Subscribe<PingMessage> ([&](const std::shared_ptr<PingMessage>&) { newCalls++; });

    bus.Publish<PingMessage> (std::make_shared<PingMessage> ());

    EXPECT_EQ (oldCalls, 0);
    EXPECT_EQ (newCalls, 1);
}

TEST (MessageBusScopedSubscription, MoveAssigningUnsubscribesPreviousCallback)
{
    MessageBus bus;
    int oldCalls = 0;
    int newCalls = 0;

    MessageBus::ScopedSubscription target = bus.Subscribe<PingMessage> ([&](const std::shared_ptr<PingMessage>&) { oldCalls++; });
    MessageBus::ScopedSubscription source = bus.Subscribe<PingMessage> ([&](const std::shared_ptr<PingMessage>&) { newCalls++; });
    target = std::move (source);

    bus.Publish<PingMessage> (std::make_shared<PingMessage> ());

    EXPECT_EQ (oldCalls, 0);
    EXPECT_EQ (newCalls, 1);
}

TEST (MessageBusScopedSubscription, MayOutliveTheBus)
{
    MessageBus::ScopedSubscription scoped;

    {
        MessageBus bus;
        scoped = bus.Subscribe<PingMessage> ([](const std::shared_ptr<PingMessage>&) {});
    }

    EXPECT_FALSE (scoped.IsActive ());
    EXPECT_NO_THROW (scoped.Unsubscribe ());
}

TEST (MessageBusScopedSubscription, OwnerDestroyedWhilePublisherKeepsPublishingIsSafe)
{
    // a callback capturing its owner must not be invoked after the owner is gone
    MessageBus bus;
    struct Owner
    {
        int calls = 0;
        MessageBus::ScopedSubscription subscription;
    };

    auto owner = std::make_unique<Owner> ();
    Owner* raw = owner.get ();
    owner->subscription = bus.Subscribe<PingMessage> ([raw](const std::shared_ptr<PingMessage>&) { raw->calls++; });

    bus.Publish<PingMessage> (std::make_shared<PingMessage> ());
    EXPECT_EQ (owner->calls, 1);

    owner.reset ();

    // would write through a dangling pointer without the scoped subscription
    EXPECT_NO_FATAL_FAILURE (bus.Publish<PingMessage> (std::make_shared<PingMessage> ()));
}
