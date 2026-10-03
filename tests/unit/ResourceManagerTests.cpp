#include <gtest/gtest.h>
#include "resource/Resource.h"
#include "resource/ResourceManager.h"
#include <cstdlib>
#include <memory>
#include <string>
#include <vector>

using namespace cilantro;

namespace {

class TestResource : public Resource
{
public:
    TestResource () : value (0) {}
    explicit TestResource (int v) : value (v) {}

    int value;
};

class OtherResource : public Resource
{
};

} // namespace

TEST (ResourceManager, StartsEmpty)
{
    ResourceManager<Resource> manager;

    EXPECT_EQ (manager.GetCount (), 0u);
    EXPECT_TRUE (manager.begin () == manager.end ());
}

TEST (ResourceManager, CreateRegistersResourceWithNameAndSequentialHandles)
{
    ResourceManager<Resource> manager;

    auto a = manager.Create<TestResource> ("first");
    auto b = manager.Create<TestResource> ("second");
    auto c = manager.Create<TestResource> ("third");

    EXPECT_EQ (a->GetName (), "first");
    EXPECT_EQ (b->GetName (), "second");
    EXPECT_EQ (c->GetName (), "third");
    EXPECT_EQ (a->GetHandle (), 0u);
    EXPECT_EQ (b->GetHandle (), 1u);
    EXPECT_EQ (c->GetHandle (), 2u);
    EXPECT_EQ (manager.GetCount (), 3u);
}

TEST (ResourceManager, CreateForwardsConstructorArguments)
{
    ResourceManager<Resource> manager;

    auto r = manager.Create<TestResource> ("answer", 42);

    EXPECT_EQ (r->value, 42);
}

TEST (ResourceManager, LookupByNameAndHandleReturnsSameObject)
{
    ResourceManager<Resource> manager;
    auto a = manager.Create<TestResource> ("a", 1);
    auto b = manager.Create<TestResource> ("b", 2);

    EXPECT_EQ (manager.GetByName<TestResource> ("a"), a);
    EXPECT_EQ (manager.GetByName<TestResource> ("b"), b);
    EXPECT_EQ (manager.GetByHandle<TestResource> (a->GetHandle ()), a);
    EXPECT_EQ (manager.GetByHandle<TestResource> (b->GetHandle ()), b);
}

TEST (ResourceManager, LookupCanReturnBaseType)
{
    ResourceManager<Resource> manager;
    auto a = manager.Create<TestResource> ("a");

    std::shared_ptr<Resource> base = manager.GetByName<Resource> ("a");

    EXPECT_EQ (base, a);
}

TEST (ResourceManager, HasNameChecksExistenceAndType)
{
    ResourceManager<Resource> manager;
    manager.Create<TestResource> ("existing");

    EXPECT_TRUE (manager.HasName<TestResource> ("existing"));
    EXPECT_TRUE (manager.HasName<Resource> ("existing"));
    EXPECT_FALSE (manager.HasName<OtherResource> ("existing"));
    EXPECT_FALSE (manager.HasName<TestResource> ("missing"));
}

TEST (ResourceManager, AddRegistersExistingObject)
{
    ResourceManager<Resource> manager;
    auto r = std::make_shared<TestResource> (7);

    auto added = manager.Add ("added", r);

    EXPECT_EQ (added, r);
    EXPECT_EQ (r->GetName (), "added");
    EXPECT_EQ (manager.GetByName<TestResource> ("added")->value, 7);
}

TEST (ResourceManager, IterationVisitsResourcesInCreationOrder)
{
    ResourceManager<Resource> manager;
    manager.Create<TestResource> ("c");
    manager.Create<TestResource> ("a");
    manager.Create<TestResource> ("b");

    std::vector<std::string> names;
    for (auto&& resource : manager)
    {
        names.push_back (resource->GetName ());
    }

    EXPECT_EQ (names, (std::vector<std::string> { "c", "a", "b" }));
}

TEST (ResourceManager, ConstIterationIsSupported)
{
    ResourceManager<Resource> manager;
    manager.Create<TestResource> ("a");
    manager.Create<TestResource> ("b");
    const ResourceManager<Resource>& constManager = manager;

    size_t count = 0;
    for (auto it = constManager.cbegin (); it != constManager.cend (); ++it)
    {
        count++;
    }

    EXPECT_EQ (count, 2u);
}

TEST (ResourceManager, IterationWorksThroughSharedPointer)
{
    auto manager = std::make_shared<ResourceManager<Resource>> ();
    manager->Create<TestResource> ("a");
    manager->Create<TestResource> ("b");

    size_t count = 0;
    for (auto&& resource : manager)
    {
        EXPECT_NE (resource, nullptr);
        count++;
    }

    EXPECT_EQ (count, 2u);
}

TEST (ResourceManager, ManagersKeepIndependentHandleSpaces)
{
    ResourceManager<Resource> first;
    ResourceManager<Resource> second;

    auto a = first.Create<TestResource> ("same");
    auto b = second.Create<TestResource> ("same");

    EXPECT_EQ (a->GetHandle (), 0u);
    EXPECT_EQ (b->GetHandle (), 0u);
    EXPECT_NE (a, b);
}

TEST (ResourceManager, ManagerKeepsResourcesAlive)
{
    ResourceManager<Resource> manager;

    {
        manager.Create<TestResource> ("kept", 5);
    }

    EXPECT_EQ (manager.GetByName<TestResource> ("kept")->value, 5);
}

// Resource errors are fatal: the error is logged and the process exits with EXIT_FAILURE

TEST (ResourceManagerDeathTest, DuplicateNameIsFatal)
{
    EXPECT_EXIT (
        {
            ResourceManager<Resource> manager;
            manager.Create<TestResource> ("dup");
            manager.Create<TestResource> ("dup");
        },
        ::testing::ExitedWithCode (EXIT_FAILURE), "");
}

TEST (ResourceManagerDeathTest, UnknownNameIsFatal)
{
    EXPECT_EXIT (
        {
            ResourceManager<Resource> manager;
            manager.GetByName<TestResource> ("missing");
        },
        ::testing::ExitedWithCode (EXIT_FAILURE), "");
}

TEST (ResourceManagerDeathTest, HandleOutOfBoundsIsFatal)
{
    EXPECT_EXIT (
        {
            ResourceManager<Resource> manager;
            manager.Create<TestResource> ("only");
            manager.GetByHandle<TestResource> (5);
        },
        ::testing::ExitedWithCode (EXIT_FAILURE), "");
}

TEST (ResourceManagerDeathTest, WrongTypeIsFatal)
{
    EXPECT_EXIT (
        {
            ResourceManager<Resource> manager;
            manager.Create<TestResource> ("typed");
            manager.GetByName<OtherResource> ("typed");
        },
        ::testing::ExitedWithCode (EXIT_FAILURE), "");
}
