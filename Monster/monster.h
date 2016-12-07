#ifndef __MONSTER_H
#define __MONSTER_H
#include <stdlib.h>
#include <vector>
#include "site.h"
#include "playfield.h"
#include <queue>


/***************************************************************************//**
 * @class Player
 *
 * @brief A class used to create a monster.
 *
 ******************************************************************************/

class Monster 
{
    Playfield* playfield;
    int N;
 
    public:
       Monster(Playfield* play);
	~Monster();
       const Site* move();
       void bfs(bool **&marked, Site* **&prev, int **&dist, const Site* monster, vector<Site*> &allocatedMemory);
       void allocateStorage(bool **&marked, Site* **&prev, int **&dist);
       void deallocateStorage(bool **&marked, Site* **&prev, int **&dist, vector<Site*> &allocatedMemory);
       void checkAdjacentRoomSquares(bool **marked, Site* **prev, int **dist, queue<Site*> &myqueue, Site* temp, vector<Site*> &allocatedMemory);


	void checkAdjacentCorridorSquares(bool **marked, Site* **prev, int **dist, queue<Site*> &myqueue, Site* temp, vector<Site*> &allocatedMemory);


	 Site* getNextMove(bool **&marked, Site* **&prev, int **&dist, const Site* monster, const Site* player, vector<Site*> &allocatedMemory);
};

#endif
