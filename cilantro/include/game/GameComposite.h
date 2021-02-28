#ifndef _GAMECOMPOSITE_H_
#define _GAMECOMPOSITE_H_

#include "cilantroengine.h"

class Game;

class GameComposite
{
public:

    GameComposite ();
    virtual ~GameComposite ();

    // attach object to game
    __EAPI void AttachToGame (Game* game);

protected:

    // game pointer
    Game* game;

};

#endif

