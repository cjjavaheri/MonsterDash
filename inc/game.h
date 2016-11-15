#ifndef __GAME_H
#define __GAME_H
#include <chrono>
#include <fstream>
#include <future>
#include <iostream>
#include <fstream>
#include <stdexcept>
#include "playfield.h"
#include "monster.h"
#include "player.h"

using namespace std;

enum GameResults
{
   NO_DECISION = 0,
   MONSTER_WINS,
   PLAYER_WINS,
   MONSTER_TIMEOUT,
   PLAYER_TIMEOUT,
   MONSTER_CHEATING,
   PLAYER_CHEATING,
};

class Game 
{
       Playfield* playfield;     // the playfield
       char MONSTER;        // name of the monster (A - Z)
       char PLAYER = '@';    // name of the player
       int N;               // board dimension
       Monster* monster;     // the monster
       Player* player;         // the player
       int moveCount;           // the number of moves so far
       GameResults gameOver;           // Game is over
       bool skipPlayer;       // Player loses a turn
       bool skipMonster;      // Monster loses a turn
       int winMoves;

    public:

       Game();
       void readFile(const char *fileName);
       const Site* getMonsterSite() const{ return playfield->getMonsterSite(); }
       const Site* getPlayerSite()const  { return playfield->getPlayerSite();  }
       const Playfield* getPlayfield()  { return playfield;     }
       void executeMove(ofstream&);
       bool isGameOver() { return gameOver ; }
       int getMoveCount() { return moveCount ; }
       int getResults() { return gameOver ; }
};

#endif
