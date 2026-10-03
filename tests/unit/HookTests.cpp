#include <gtest/gtest.h>
#include "system/Hook.h"
#include <string>
#include <vector>

using namespace cilantro;

TEST (Hook, InvokingHookWithoutSubscribersIsHarmless)
{
    Hook<std::string> hooks;

    EXPECT_NO_THROW (hooks.InvokeHook ("nothing"));
}

TEST (Hook, SubscribedCallbackIsInvoked)
{
    Hook<std::string> hooks;
    int calls = 0;

    hooks.SubscribeHook ("event", [&]() { calls++; });
    hooks.InvokeHook ("event");
    hooks.InvokeHook ("event");

    EXPECT_EQ (calls, 2);
}

TEST (Hook, CallbacksAreInvokedInSubscriptionOrder)
{
    Hook<std::string> hooks;
    std::vector<int> order;

    hooks.SubscribeHook ("event", [&]() { order.push_back (1); });
    hooks.SubscribeHook ("event", [&]() { order.push_back (2); });
    hooks.SubscribeHook ("event", [&]() { order.push_back (3); });
    hooks.InvokeHook ("event");

    EXPECT_EQ (order, (std::vector<int> { 1, 2, 3 }));
}

TEST (Hook, HooksAreIndependent)
{
    Hook<std::string> hooks;
    int a = 0;
    int b = 0;

    hooks.SubscribeHook ("a", [&]() { a++; });
    hooks.SubscribeHook ("b", [&]() { b++; });
    hooks.InvokeHook ("a");

    EXPECT_EQ (a, 1);
    EXPECT_EQ (b, 0);
}

TEST (Hook, ParametersArePassedToCallbacks)
{
    Hook<std::string, int, std::string> hooks;
    int number = 0;
    std::string text;

    hooks.SubscribeHook ("event", [&](int n, std::string s) { number = n; text = s; });
    hooks.InvokeHook ("event", 7, "seven");

    EXPECT_EQ (number, 7);
    EXPECT_EQ (text, "seven");
}

TEST (Hook, WorksWithIntegerKeys)
{
    Hook<int> hooks;
    int calls = 0;

    hooks.SubscribeHook (5, [&]() { calls++; });
    hooks.InvokeHook (5);
    hooks.InvokeHook (6);

    EXPECT_EQ (calls, 1);
}
