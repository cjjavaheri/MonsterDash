#include "monster.h"

Monster::Monster(Playfield* p) 
{
   playfield = p;
   N       = playfield->size();
}

// TAKE A RANDOM LEGAL MOVE
// YOUR MAIN TASK IS TO RE-IMPLEMENT THIS METHOD TO DO SOMETHING INTELLIGENT
const Site* Monster::move() 
{
   bool **marked = nullptr;
   Site* **prev = nullptr;
   int ** dist = nullptr;
   const Site* monster   = playfield->getMonsterSite();
   const Site* player = playfield->getPlayerSite();
   const Site* test;
   Site* nextMove;
   int i;
   int j;
   cout << "Monster[i][j] " << monster->i() << " " << monster->j() << endl;
   cout << "PLayer[i][j] " << player->i() << " " << player->j() << endl;
   cout << endl << endl;

	allocateStorage(marked, prev, dist);

	bfs(marked, prev, dist, monster);

	nextMove = getNextMove(marked, prev, dist, monster, player);

	test = new Site(nextMove->i(), nextMove->j());
	deallocateStorage(marked, prev, dist);
	return test;

	/*


	if (playfield->isRoom(monster) == true)
	{
		test = moveRoom(monster, player);
		return test;
	}
	if (playfield->isCorridor(monster) == true)
	{
		test = moveCorridor(monster, player);
		return test;
	}

	*/


   return nullptr;

/*
   int myI = monster->i();
   int myJ = monster->j();

   vector<const Site *> valid;
   for (int i = -1; i < 2; i++)
   {      
      for (int j = -1; j < 2; j++)
      {
         const Site* site = new Site(myI+i, myJ+j);
         if (playfield->isLegalMove(monster, site))
            valid.push_back(site);
      }
   }   

   const Site* ret = (valid.size() == 0) ? nullptr : valid[rand() % valid.size()];
   return ret;

*/
}



const Site* Monster::moveRoom(const Site* monster, const Site* player)
{
	const Site* test = monster;

			// Monster's row is smaller than player's row
		if (monster->i() < player->i())
		{
			//Monster's column is smaller than player's column
			if (monster->j() < player->j())
			{
				test = new Site(monster->i() + 1, monster->j() + 1);
				if (playfield->isLegalMove(monster, test))
				{
					return test;
				}

				test = new Site(monster->i() + 1, monster->j());
				if (playfield->isLegalMove(monster, test))
				{
					return test;
				}
				
				test = new Site(monster->i() , monster->j() + 1);
				if (playfield->isLegalMove(monster, test))
				{
					return test;
				}

				test = new Site(monster->i() , monster->j() + 1);
				if (playfield->isLegalMove(monster, test))
				{
					return test;
				}
				
			}
			//Monster's column is greater than player's column
		        if (monster->j() > player->j())
			{
				test = new Site(monster->i() + 1, monster->j() - 1);
				if (playfield->isLegalMove(monster, test))
				{
					return test;
				}

				test = new Site(monster->i() + 1, monster->j());
				if (playfield->isLegalMove(monster, test))
				{
					return test;
				}

				test = new Site(monster->i(), monster->j() - 1);
				if (playfield->isLegalMove(monster, test))
				{
					return test;
				}
			}
			//Monster's column is equal to player's column
			else
			{
				test = new Site(monster->i() + 1, monster->j());
				if (playfield->isLegalMove(monster, test))
				{
					return test;
				}
			}
		}
		//Monster's row is greater than player's row
	        if (monster->i() > player->i())
		{
				//Monster's column is smaller than player's column
			if (monster->j() < player->j())
			{
				test = new Site(monster->i() - 1, monster->j() + 1);
				if (playfield->isLegalMove(monster, test))
				{
					return test;
				}

				test = new Site(monster->i() - 1, monster->j());
				if (playfield->isLegalMove(monster, test))
				{
					return test;
				}

				test = new Site(monster->i(), monster->j() + 1);
				if (playfield->isLegalMove(monster, test))
				{
					return test;
				}
			}
			//Monster's column is greater than player's column
		        if (monster->j() > player->j())
			{
				test = new Site(monster->i() - 1, monster->j() - 1);
				if (playfield->isLegalMove(monster, test))
				{
					return test;
				}

				test = new Site(monster->i() - 1, monster->j());
				if (playfield->isLegalMove(monster, test))
				{
					return test;
				}

				test = new Site(monster->i(), monster->j() - 1);
				if (playfield->isLegalMove(monster, test))
				{
					return test;
				}
			}
			//Monster's column is equal to player's column
			else
			{
				test = new Site(monster->i() - 1, monster->j());
				if (playfield->isLegalMove(monster, test))
				{
					return test;
				}
			}
		}
		// Monster's row is equal to player's row
		else
		{
			//Monster's column is smaller than player's column
			if (monster->j() < player->j())
			{
				test = new Site(monster->i(), monster->j() + 1);
				if (playfield->isLegalMove(monster, test))
				{
					return test;
				}
			}
			//Monster's column is greater than player's column
		        if (monster->j() > player->j())
			{
				test = new Site(monster->i(), monster->j() - 1);
				if (playfield->isLegalMove(monster, test))
				{
					return test;
				}
			}
			//Monster's column is equal to player's column
			else
			{
				test = new Site(monster->i() , monster->j());
				if (playfield->isLegalMove(monster, test))
				{
					return test;
				}
			}
		}
		test = monster;
		return test;

}

const Site* Monster::moveCorridor(const Site* monster, const Site* player)
{
	const Site* test = monster;
			// Monster's row is smaller than player's row
		if (monster->i() < player->i())
		{
			test = new Site(monster->i() + 1, monster->j());
			if (playfield->isLegalMove(monster, test))
				return test;
			
			// Monster's column is smaller than player's column
			if (monster->j() < player->j())
			{
				test = new Site(monster->i(), monster->j() + 1 );
				if (playfield->isLegalMove(monster, test))
					return test;
				
			}
			//Monster's column is greater than player's column
			if (monster->j() > player->j())
			{
				test = new Site(monster->i(), monster->j() - 1);
				if (playfield->isLegalMove(monster, test))
					return test;
				
			}
		}
		//Monster's row is greater than player's row
	       if (monster->i() > player->i())
		{
			test = new Site(monster->i() - 1, monster->j());
			if (playfield->isLegalMove(monster, test))
				return test;

			// Monster's column is smaller than player's column
			if (monster->j() < player->j())
			{
				test = new Site(monster->i(), monster->j() + 1 );
				if (playfield->isLegalMove(monster, test))
					return test;
				
			}
			//Monster's column is greater than player's column
			if (monster->j() > player->j())
			{
				test = new Site(monster->i(), monster->j() - 1);
				if (playfield->isLegalMove(monster, test))
					return test;
				
			}
		}
		//Monster's row is equal to player's row
		if (monster->i() == player->i())
		{
			//Monster's column is smaller than player's column
			if (monster->j() < player->j())
			{
				test = new Site(monster->i(), monster->j() + 1);
				if (playfield->isLegalMove(monster, test))
					return test;
			}
			//Monster's column is greater than player's column
			if (monster->j() > player->j())
			{
				test = new Site(monster->i(), monster->j() - 1);
				if (playfield->isLegalMove(monster, test))
					return test;
			}

			//Monster's column is equal to player's column
			if (monster->j() == player->j())
			{
				test = new Site(monster->i(), monster->j());
				if (playfield->isLegalMove(monster, test))
					return test;
			}
		}
		test = monster;
		return test;
}

 Site* Monster::getNextMove(bool **&marked, Site* **&prev, int **&dist, const Site* monster, const Site* player)
{
	unsigned int i;
	unsigned int j;
	Site* nextMove = nullptr;
	Site* temp = nullptr;

	i = player->i();
	j = player->j();

	while (dist[i][j] != 1)
	{
		nextMove = prev[i][j];
		i = nextMove->i();
		j = nextMove->j();
	}

	return nextMove;


}


void Monster::bfs(bool **&marked, Site* **&prev, int **&dist, const Site* monster)
{
	queue< Site*> myqueue;
	bool isRoom;
	Site* monsterSite = new Site(monster->i(), monster->j());
	Site* temp;

	// Mark the initial site(monster's site) as visited.
	marked[monster->i()][monster->j()] = true;
	dist[monster->i()][monster->j()] = 0;
	prev[monster->i()][monster->j()] = monsterSite;
	myqueue.push(monsterSite);

	// While the queue is not empty, visit all adjacent vertices.
	while (!myqueue.empty())
	{
		temp = myqueue.front();
		myqueue.pop();
		isRoom = playfield->isRoom(temp);
		// Check all 8 adjacent vertices to temp.
		if (isRoom)
			checkAdjacentRoomSquares(marked, prev, dist, myqueue, temp);


		// Temp must be a corridor site, so check all 4 adjacent vertices.
		else
			checkAdjacentCorridorSquares(marked, prev, dist, myqueue, temp);

	}

	delete monsterSite;

}

void Monster::checkAdjacentCorridorSquares(bool **marked, Site* **prev, int **dist, queue<Site*> &myqueue, Site* temp)
{
			int i;
			int j;
			Site* addSite = nullptr;
			i = temp->i();
			j = temp->j();
			if (!playfield->isWall(i - 1, j))
			{
				if (playfield->isRoom(i - 1, j) || playfield->isCorridor(i - 1, j))
				{
					if (!marked[i - 1][j])
					{
						marked[i - 1][j] = true;
						dist[i - 1][j] = dist[i][j] + 1;
						prev[i - 1][j] = temp;
						addSite = new Site(i - 1, j);
						myqueue.push(addSite);
					}
				}
			}



			if (!playfield->isWall(i + 1, j))
			{
				if (playfield->isRoom(i + 1, j) || playfield->isCorridor(i + 1, j))
				{
					if (!marked[i + 1][j])
					{
						marked[i + 1][j] = true;
						dist[i + 1][j] = dist[i][j] + 1;
						prev[i + 1][j] = temp;
						addSite = new Site(i + 1, j);
						myqueue.push(addSite);
					}
				}
			}



			if (!playfield->isWall(i, j + 1))
			{
				if (playfield->isRoom(i, j + 1) || playfield->isCorridor(i, j + 1))
				{
					if (!marked[i][j + 1])
					{
						marked[i][j + 1] = true;
						dist[i][j + 1] = dist[i][j] + 1;
						prev[i][j + 1] = temp;
						addSite = new Site(i, j + 1);
						myqueue.push(addSite);
					}
				}
			}



			if (!playfield->isWall(i, j - 1))
			{
				if (playfield->isRoom(i, j - 1) || playfield->isCorridor(i, j - 1))
				{
					if (!marked[i][j - 1])
					{
						marked[i][j - 1] = true;
						dist[i][j - 1] = dist[i][j] + 1;
						prev[i][j - 1] = temp;
						addSite = new Site(i, j - 1);
						myqueue.push(addSite);
					}
				}
			}

}

void Monster::checkAdjacentRoomSquares(bool **marked, Site* **prev, int **dist, queue<Site*> &myqueue, Site* temp)
{

			Site* addSite = nullptr;
			int i;
			int j;
			i = temp->i();
			j = temp->j();
			if (!playfield->isWall(i + 1, j + 1))
			{
				if (playfield->isRoom(i + 1, j + 1))
				{
					if (!marked[i + 1][j + 1])
					{
						marked[i + 1][j + 1] = true;
						dist[i + 1][j + 1] = dist[i][j] + 1;
						prev[i + 1][j + 1] = temp;
						addSite = new Site(i + 1, j + 1);
						myqueue.push(addSite);
					}
				}
			}



			if (!playfield->isWall(i + 1, j - 1))
			{
				if (playfield->isRoom(i + 1, j - 1))
				{
					if (!marked[i + 1][j - 1])
					{
						marked[i + 1][j - 1] = true;
						dist[i + 1][j - 1] = dist[i][j] + 1;
						prev[i + 1][j - 1] = temp;
						addSite = new Site (i + 1, j - 1);
						myqueue.push(addSite);
					}
				}
			}



			if (!playfield->isWall(i - 1, j + 1))
			{
				if (playfield->isRoom(i - 1, j + 1))
				{
					if (!marked[i - 1][j + 1])
					{
						marked[i - 1][j + 1] = true;
						dist[i - 1][j + 1] = dist[i][j] + 1;
						prev[i - 1][j + 1] = temp;
						addSite = new Site(i - 1, j + 1);
						myqueue.push(addSite);
					}
				}
			}




			if (!playfield->isWall(i - 1, j - 1))
			{
				if (playfield->isRoom(i - 1, j - 1))
				{
					if (!marked[i - 1][j - 1])
					{
						marked[i - 1][j - 1] = true;
						dist[i - 1][j - 1] = dist[i][j] + 1;
						prev[i - 1][j - 1] = temp;
						addSite = new Site(i - 1, j - 1);
						myqueue.push(addSite);
					}
				}
			}

			checkAdjacentCorridorSquares(marked, prev, dist, myqueue, temp);

}

void Monster::deallocateStorage(bool **&marked, Site* **&prev, int **&dist)
{
	int i;
	int j;

	for (i = 0; i < N; i++)
		delete[] marked[i];

	delete[] marked;

	for (i = 0; i < N; i++)
		delete[] prev[i];

	delete[] prev;

	for (i = 0; i < N; i++)
		delete[] dist[i];

	delete[] dist;

}

void Monster::allocateStorage(bool **&marked, Site* **&prev, int **&dist)
{
	int i;
	int j;

	dist = new (nothrow) int *[N];
	for (i = 0; i < N; i++)
		dist[i] = new (nothrow) int [N];

	prev = new (nothrow) Site* *[N];
	for (i = 0; i < N; i++)
		prev[i] = new (nothrow) Site* [N];


	marked = new (nothrow) bool *[N];
	for (i = 0; i < N; i++)
		marked[i] = new (nothrow) bool [N];

	for (i = 0; i < N; i++)
		for (j = 0; j < N; j++)
			marked[i][j] = false;


}
