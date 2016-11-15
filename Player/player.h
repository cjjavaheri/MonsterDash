#ifndef __PLAYER_H
#define __PLAYER_H
#include <stdlib.h>
#include <vector>
#include "site.h"
#include "playfield.h"

class Player 
{
   Playfield* playfield;
   int N;

   public:
      Player(Playfield* p);
      const Site* move();
};
#endif
