#ifndef _GAMECOMPOSITE_H_
#define _GAMECOMPOSITE_H_

#include "cilantroengine.h"
#include "game/Game.h"

class GameComposite
{
public:

    GameComposite ();
    virtual ~GameComposite ();

    // run a game loop
    __EAPI void AttachToGame (Game* game);

protected:

    // game pointer
    Game* game;

};

#endif

