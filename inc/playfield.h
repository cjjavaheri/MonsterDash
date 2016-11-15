#ifndef __PLAYFIELD_H
#define __PLAYFIELD_H
#include <iostream>
#include <string>
#include "site.h"

using namespace std;

class Playfield 
{
    bool **Room;        // is v-w a room site?
    bool **Corridor;    // is v-w a corridor site?
    unsigned int N;     // dimension of dungeon
    const Site* playerSite;
    const Site* monsterSite;

   public:
      Playfield(char **board, int N);
      int size() const { return N ; }
      bool isCorridor(const Site *v) const ;
      bool isCorridor(const unsigned int x, const unsigned int y) const;
      bool isRoom(const Site *v) const ;
      bool isRoom(const unsigned int x, const unsigned int y) const;
      bool isWall(const Site *v) const ;
      bool isWall(const unsigned int x, const unsigned int y) const;
      bool isLegalMove(const Site* v, const Site* w);
      void setPlayerSite(const Site *v);
      void setMonsterSite(const Site *v);
      const Site* getPlayerSite() const { return playerSite ; }
      const Site* getMonsterSite() const { return monsterSite ; }

      friend ostream& operator<<(ostream &os, Playfield *);
};

#endif
