#include "game/GameComposite.h"

GameComposite::GameComposite ()
{

}

GameComposite::~GameComposite ()
{
    
}

void GameComposite::AttachToGame (Game* game)
{
    this->game = game;
}