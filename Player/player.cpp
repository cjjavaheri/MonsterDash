#include <vector>
#include "player.h"

using namespace std;

Player::Player(Playfield* p)
{
   playfield = p;
   N = playfield->size();
}

const Site* Player::move()
{
   // TAKE A RANDOM LEGAL MOVE
   // YOUR MAIN TASK IS TO RE-IMPLEMENT THIS METHOD TO DO SOMETHING INTELLIGENT
   const Site* player   = playfield->getPlayerSite();
   int myI = player->i();
   int myJ = player->j();
   vector<Site*> corridors;
   map<Site*, vector<Site*>> adj;
   map<Site*, vector<Site*>>::iterator it;
   vector<Site*>::iterator vectIt;
   Site* mySite;

   findCorridors(corridors);
   adj = findAdjLists(adj, corridors);

   it = adj.begin();
   while (it != adj.end())
   {
	mySite = it->first;
	cout << "[" <<  mySite->i() << "]" << "[" << mySite->j() << "]: ";
	vectIt = (it->second).begin();
	while (vectIt != (it->second).end())
	{
		cout << "[" << (*vectIt)->i() << "][" << (*vectIt)->j() << "] ";
		vectIt++;
	}

	it++;
	cout << endl;
   }

   vector<const Site *> valid;
   for (int i = -1; i < 2; i++) 
   {
      for (int j = -1; j < 2; j++) 
      {
         const Site* site = new Site(myI+i, myJ+j);
         if (playfield->isLegalMove(player, site)) 
            valid.push_back(site);
      }
   }

   return (valid.size() == 0) ? nullptr : valid[rand() % valid.size()];
}

map<Site*, vector<Site*>> Player::findAdjLists(map<Site*, vector<Site*>> &adj, vector<Site*> &corridors)
{
	vector<Site*>::iterator it;
	vector<Site*>::iterator curr;
	vector<Site*> adjacentVertices;
	Site* currSite;
	Site* compareSite;
	curr = corridors.begin();
	while (curr != corridors.end())
	{
		currSite = (*curr);
		it = corridors.begin();
		adjacentVertices.clear();
		while (it != corridors.end())
		{
			compareSite = (*it);
			if (currSite->i() + 1 == compareSite->i() && currSite->j() == 				compareSite->j())
					adjacentVertices.push_back(compareSite);

			if (currSite->i() - 1 == compareSite->i() && currSite->j() == 				compareSite->j())
					adjacentVertices.push_back(compareSite);

			if (currSite->i() == compareSite->i() && currSite->j() + 1 == 				compareSite->j())
					adjacentVertices.push_back(compareSite);

			if (currSite->i() == compareSite->i() && currSite->j() - 1 == 				compareSite->j())
					adjacentVertices.push_back(compareSite);

			it++;
		}

		adj.insert({currSite, adjacentVertices});
		curr++;
	}

	return adj;

}

vector<Site*> Player::findCorridors(vector<Site*> &corridors)
{
	int i;
	int j;
	Site* addSite;

	for (i = 0; i < N; i++)
		for (j = 0; j < N; j++)
			if (playfield->isCorridor(i, j))
			{
				addSite = new Site(i, j);
				if (playfield->isCorridor(addSite))
				{
				cout << "Corridors[" << i << "][" << j << "]" << endl << endl;
				corridors.push_back(addSite);
				}
			}

	return corridors;

}
