#ifndef __MONSTER_H
#define __MONSTER_H
#include <stdlib.h>
#include <vector>
#include "site.h"
#include "playfield.h"
#include <queue>

class Monster 
{
    Playfield* playfield;
    int N;
    const Site* tryMove;
 
    public:
       Monster(Playfield* play);
	~Monster();
       const Site* move();
       void bfs(bool **&marked, Site* **&prev, int **&dist, const Site* monster);
       void allocateStorage(bool **&marked, Site* **&prev, int **&dist);
       void deallocateStorage(bool **&marked, Site* **&prev, int **&dist);
       void checkAdjacentRoomSquares(bool **marked, Site* **prev, int **dist, queue<Site*> &myqueue, Site* temp);


	void checkAdjacentCorridorSquares(bool **marked, Site* **prev, int **dist, queue<Site*> &myqueue, Site* temp);


	 Site* getNextMove(bool **&marked, Site* **&prev, int **&dist, const Site* monster, const Site* player);
};

#endif
