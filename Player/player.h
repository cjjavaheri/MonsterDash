#ifndef __PLAYER_H
#define __PLAYER_H
#include <stdlib.h>
#include <vector>
#include <map>
#include "site.h"
#include "playfield.h"

class Player 
{
   Playfield* playfield;
   int N;

   public:
      Player(Playfield* p);
      const Site* move();

	vector<Site*> findCorridors(vector<Site*> &corridors);
	map<Site*, vector<Site*>> findAdjLists(map<Site*, vector<Site*>> &adj, vector<Site*> &corridors);
};
#endif
