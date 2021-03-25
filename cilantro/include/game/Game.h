#ifndef _GAME_H_
#define _GAME_H_

#include "cilantroengine.h"

class Game
{
public:

    __EAPI Game ();
    __EAPI ~Game ();

    // run a game loop
    __EAPI void Run ();
    __EAPI void Stop ();	
    __EAPI void Step ();

private:

    // game state
    bool shouldStop;
    bool isRunning;

};

#endif

