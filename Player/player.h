#ifndef __PLAYER_H
#define __PLAYER_H
#include <stdlib.h>
#include <vector>
#include <map>
#include "site.h"
#include "playfield.h"
#include <queue>
#include <vector>
#include <stack>

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



	Site* findCorridorCycle(map<Site*, vector<Site*>> &adj, int **&distMonster, vector<Site*> &allocatedMemory, vector<Site*> &cycle, Site* &start, const Site* player);



	Site* returnToStart(map<Site*, vector<Site*>> &adj, int **&distPlayer, Site* **&prevPlayer, vector<Site*> &allocatedMemory, Site* &start, const Site* player);

	bool doCorridorsExist();

	bool doWallsExist();

	Site* keepDistance(int **&distMonster, const Site* player, vector<Site*> &allocatedMemory);

	int countDisconnectedComponents(map<Site*, vector<Site*>> &adj);


	map<Site*, vector<Site*>> getAdjListDisc(map<Site*, vector<Site*>> &adj, vector<Site*> &allocatedMemory);

	 void removeDeadEnds(map<Site*, vector<Site*>> &adj);

	void search(vector<Site*> &vectDisc, Site* site, int &i, int &j);

	void checkDuplicates(vector<Site*> &vectDisc, Site* site);


	Site* calculateNextRoom(map<Site*, vector<Site*>> &adjDisc, int **&distMonster, int**&distPlayer, Site* **&prevPlayer, const Site* player);


	

};
#endif
