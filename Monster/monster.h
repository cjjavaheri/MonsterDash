#ifndef __MONSTER_H
#define __MONSTER_H
#include <stdlib.h>
#include <vector>
#include "site.h"
#include "playfield.h"

class Monster 
{
    Playfield* playfield;
    int N;
 
    public:
       Monster(Playfield* play);
       const Site* move();
       const Site* moveRoom(const Site* monster, const Site* player);
       const Site* moveCorridor(const Site* monster, const Site* player);
};

#endif
