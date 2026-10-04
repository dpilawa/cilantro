#include <gtest/gtest.h>
#include "system/Game.h"
#include "scene/GameScene.h"
#include <memory>

using namespace cilantro;

// Game::Step has to tolerate a game that is not fully set up (no scene, no renderer, no input controller)

TEST (Game, StepWithoutSceneAndInputControllerDoesNothing)
{
    auto game = std::make_shared<Game> ();
    game->Initialize ();

    EXPECT_NO_FATAL_FAILURE (game->Step ());
}

TEST (Game, StepWithSceneWithoutRendererAndInputController)
{
    auto game = std::make_shared<Game> ();
    game->Initialize ();
    auto scene = game->Create<GameScene> ("scene");

    EXPECT_NO_FATAL_FAILURE (game->Step ());
    EXPECT_NO_FATAL_FAILURE (game->Step ());
}

TEST (Game, FirstCreatedSceneBecomesCurrent)
{
    auto game = std::make_shared<Game> ();
    game->Initialize ();

    auto first = game->Create<GameScene> ("first");
    auto second = game->Create<GameScene> ("second");

    EXPECT_EQ (game->GetCurrentGameScene (), first);

    game->SetCurrentGameScene ("second");
    EXPECT_EQ (game->GetCurrentGameScene (), second);
}

TEST (Game, StopRequestEndsRunLoop)
{
    auto game = std::make_shared<Game> ();
    game->Initialize ();
    auto scene = game->Create<GameScene> ("scene");

    EXPECT_FALSE (game->IsRunning ());
    game->Stop ();
    game->Run ();
    EXPECT_FALSE (game->IsRunning ());
}
