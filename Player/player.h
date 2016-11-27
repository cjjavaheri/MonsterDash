#ifndef __PLAYER_H
#define __PLAYER_H
#include <stdlib.h>
#include <vector>
#include <map>
#include "site.h"
#include "playfield.h"
#include <queue>
#include <vector>

class Player 
{
   Playfield* playfield;
   int N;

   public:
      Player(Playfield* p);
      const Site* move();

	vector<Site*> findCorridors(vector<Site*> &corridors);
	map<Site*, vector<Site*>> findAdjLists(map<Site*, vector<Site*>> &adj, vector<Site*> &corridors);


	       void bfs(bool **&marked, Site* **&prev, int **&dist, const Site* player, vector<Site*> &allocatedMemory);
       void allocateStorage(bool **&marked, Site* **&prev, int **&dist);
       void checkAdjacentRoomSquares(bool **marked, Site* **prev, int **dist, queue<Site*> &myqueue, Site* temp, vector<Site*> &allocatedMemory);


	void checkAdjacentCorridorSquares(bool **marked, Site* **prev, int **dist, queue<Site*> &myqueue, Site* temp, vector<Site*> &allocatedMemory);



	       void deallocateStorage(bool **&markedMonster, Site* **&prevMonster, int **&distMonster, bool **&markedPlayer, Site* **&prevPlayer, int **&distPlayer, vector<Site*> &allocatedMemory);

       Site* getNextMove(bool **&markedMonster, Site* **&prevMonster, int **&distMonster, bool **&markedPlayer, Site* **&prevPlayer, int **&distPlayer, const Site* monster, const Site* player, vector<Site*> &allocatedMemory, map<Site*, vector<Site*>> &adj);


};
#endif
