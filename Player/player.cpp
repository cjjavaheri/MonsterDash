
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
    const Site* monster = playfield->getMonsterSite();
   vector<Site*> corridors;
   map<Site*, vector<Site*>> adj;
   map<Site*, vector<Site*>>::iterator it;
   vector<Site*>::iterator vectIt;
   Site* mySite;
   bool **markedMonster = nullptr;
   bool **markedPlayer = nullptr;
   Site* **prevMonster = nullptr;
   Site* **prevPlayer = nullptr;
   int **distMonster = nullptr;
   int **distPlayer = nullptr;
   Site* nextMove;
   const Site* tryMove;
   vector<Site*> allocatedMemory;

   // Perform a breadth-first-search starting at the monster.
   allocateStorage(markedMonster, prevMonster, distMonster);
   bfs(markedMonster, prevMonster, distMonster, monster, allocatedMemory);


   // Perform a breadth-first-search starting at the player.
   allocateStorage(markedPlayer, prevPlayer, distPlayer);

   bfs(markedPlayer, prevPlayer, distPlayer, player, allocatedMemory);

   // Store all corridor sites in a vector.
   findCorridors(corridors);

   // Get the list of adjacent corridor sites to each corridor site.
   adj = findAdjLists(adj, corridors);

   /*
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

   */

    nextMove = getNextMove(markedMonster, prevMonster, distMonster, markedPlayer, prevPlayer, distPlayer, monster, player, allocatedMemory, adj);

    tryMove = new Site(nextMove->i(), nextMove->j());

   // Free any dynamically allocated memory up.
    deallocateStorage(markedMonster, prevMonster, distMonster, markedPlayer, prevPlayer, distPlayer, allocatedMemory);

	return tryMove;

/*

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

*/
}

map<Site*, vector<Site*>> Player::findAdjLists(map<Site*, vector<Site*>> &adj, vector<Site*> &corridors)
{
	vector<Site*>::iterator it;
	vector<Site*>::iterator curr;
	vector<Site*> adjacentVertices;
	Site* currSite;
	Site* compareSite;
	curr = corridors.begin();
	// For each corridor site, check all of the corridor sites to see which ones are adjacent.
	while (curr != corridors.end())
	{
		currSite = (*curr);
		it = corridors.begin();
		adjacentVertices.clear();
		while (it != corridors.end())
		{
			// Check the lower vertical site.
			compareSite = (*it);
			if (currSite->i() + 1 == compareSite->i() && currSite->j() == 				compareSite->j())
					adjacentVertices.push_back(compareSite);

			// Check the upper vertical site.
			if (currSite->i() - 1 == compareSite->i() && currSite->j() == 				compareSite->j())
					adjacentVertices.push_back(compareSite);

			// Check the right horizontal site.
			if (currSite->i() == compareSite->i() && currSite->j() + 1 == 				compareSite->j())
					adjacentVertices.push_back(compareSite);

			// Check the left horizontal site.
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

	// For each vertex on the playfield, store it in a vector if it's a corridor.
	for (i = 0; i < N; i++)
		for (j = 0; j < N; j++)
			if (playfield->isCorridor(i, j))
			{
				addSite = new Site(i, j);
				if (playfield->isCorridor(addSite))
				{
				//cout << "Corridors[" << i << "][" << j << "]" << endl << endl;
				corridors.push_back(addSite);
				}
			}

	return corridors;

}





   Site* Player::getNextMove(bool **&markedMonster, Site* **&prevMonster, int **&distMonster, bool **&markedPlayer, Site* **&prevPlayer, int **&distPlayer, const Site* monster, const Site* player, vector<Site*> &allocatedMemory, map<Site*, vector<Site*>> &adj)
{
        unsigned int i;
        unsigned int j;
	int longestDist;
	int shortestDist;
	map<Site*, vector<Site*>>::iterator it;
	vector<Site*> adjVect;
	vector<Site*>::iterator vectIt;
	Site* nextMove = nullptr;

	longestDist = distMonster[player->i()][player->j()];
	nextMove = new Site(player->i(), player->j());

	i = player->i();
	j = player->j();

	if (playfield->isCorridor(player))
	{
		it = adj.begin();
		while (it != adj.end() && ((it->first)->i() != i || (it->first)->j() != j))
		{
			it++;
		}

		adjVect = it->second;
		vectIt = adjVect.begin();
		while (vectIt != adjVect.end())
		{
			if (distMonster[(*vectIt)->i()][(*vectIt)->j()] > longestDist)
			{
				longestDist = distMonster[(*vectIt)->i()][(*vectIt)->j()];
				nextMove = new Site((*vectIt)->i(),(*vectIt)->j());
			}

			vectIt++;
		}

		return nextMove;
	}

	if (playfield->isRoom(player))
	{

		it = adj.begin();
		shortestDist = distPlayer[(it->first)->i()][(it->first)->j()];
		nextMove = new Site((it->first)->i(), (it->first)->j());

		while (it != adj.end() && ((it->first)->i() != i || (it->first)->j() != j))
		{
			if (distPlayer[(it->first)->i()][(it->first)->j()] < shortestDist)
			{
				shortestDist = distPlayer[(it->first)->i()][(it->first)->j()];
				nextMove = new Site((it->first)->i(), (it->first)->j());
			}
			it++;
		}
		
	}

	i = nextMove->i();
	j = nextMove->j();

	
	// Starting distance is not 1
	if (distPlayer[i][j] != 1)
	{

		while (distPlayer[i][j] != 1)
		{
			nextMove = prevPlayer[i][j];
			i = nextMove->i();
			j = nextMove->j();
		}
		return nextMove;

	}

	// Starting distance is 1 ; therefore, the monster only has to take
	// one more move to reach the player.
	else
	{
		
		/*
		nextMove = new Site(player->i(), player->j());
		allocatedMemory.push_back(nextMove);
		*/
		
		return nextMove;
	}


	return nextMove;


}


void Player::bfs(bool **&marked, Site* **&prev, int **&dist, const Site* player, vector<Site*> &allocatedMemory)
{
	queue< Site*> myqueue;
	bool isRoom;
	Site* playerSite = new Site(player->i(), player->j());
	Site* temp;

	// Mark the initial site as visited.
	marked[player->i()][player->j()] = true;
	dist[player->i()][player->j()] = 0;
	prev[player->i()][player->j()] = playerSite;
	myqueue.push(playerSite);


	// While the queue is not empty, visit all adjacent vertices.
	while (!myqueue.empty())
	{
		temp = myqueue.front();
		myqueue.pop();
		isRoom = playfield->isRoom(temp);
		// Check all 8 adjacent vertices to temp.
		if (isRoom)
			checkAdjacentRoomSquares(marked, prev, dist, myqueue, temp, allocatedMemory);


		// Temp must be a corridor site, so check all 4 adjacent vertices.
		else
			checkAdjacentCorridorSquares(marked, prev, dist, myqueue, temp, allocatedMemory);

	}

	// Store playerSite in order to free the memory later.
	allocatedMemory.push_back(playerSite);

}

void Player::checkAdjacentCorridorSquares(bool **marked, Site* **prev, int **dist, queue<Site*> &myqueue, Site* temp, vector<Site*> &allocatedMemory)
{
			int i;
			int j;
			Site* addSite = nullptr;
			i = temp->i();
			j = temp->j();
	

			// Check all 4 horizontal and vertical sites.
			// First, check upper vertical site.
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
						allocatedMemory.push_back(addSite);
						myqueue.push(addSite);
					}
				}
			}


			//Next, check lower vertical site.
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
						allocatedMemory.push_back(addSite);
						myqueue.push(addSite);
					}
				}
			}


			// Next, check the right horizontal site.
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
						allocatedMemory.push_back(addSite);
						myqueue.push(addSite);
					}
				}
			}


			//Finally, check the left horizontal site.
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
						allocatedMemory.push_back(addSite);
						myqueue.push(addSite);
					}
				}
			}

}

void Player::checkAdjacentRoomSquares(bool **marked, Site* **prev, int **dist, queue<Site*> &myqueue, Site* temp, vector<Site*> &allocatedMemory)
{

			Site* addSite = nullptr;
			int i;
			int j;
			i = temp->i();
			j = temp->j();

			// Check all 4 diagonal sites.
			// First check lower right diagonal
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
						allocatedMemory.push_back(addSite);
						myqueue.push(addSite);
					}
				}
			}


			// Next check lower left diagonal
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
						allocatedMemory.push_back(addSite);
						myqueue.push(addSite);
					}
				}
			}


			// Next check upper right diagonal
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
						allocatedMemory.push_back(addSite);
						myqueue.push(addSite);
					}
				}
			}



			// Finally check upper left diagonal
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
						allocatedMemory.push_back(addSite);
						myqueue.push(addSite);
					}
				}
			}

			// Now check the horizontal and vertical sites.
			checkAdjacentCorridorSquares(marked, prev, dist, myqueue, temp, allocatedMemory);

}

	       void Player::deallocateStorage(bool **&markedMonster, Site* **&prevMonster, int **&distMonster, bool **&markedPlayer, Site* **&prevPlayer, int **&distPlayer, vector<Site*> &allocatedMemory)
{
	int i;
	vector<Site*>::iterator it;

	// Free all of the 2D arrays.
	for (i = 0; i < N; i++)
		delete[] markedMonster[i];

	delete[] markedMonster;

	for (i = 0; i < N; i++)
		delete[] prevMonster[i];

	delete[] prevMonster;

	for (i = 0; i < N; i++)
		delete[] distMonster[i];

	delete[] distMonster;


	for (i = 0; i < N; i++)
		delete[] markedPlayer[i];

	delete[] markedPlayer;

	for (i = 0; i < N; i++)
		delete[] prevPlayer[i];

	delete[] prevPlayer;

	for (i = 0; i < N; i++)
		delete[] distPlayer[i];

	delete[] distPlayer;


	// Free all allocated site pointers.
	it = allocatedMemory.begin();
	while (it != allocatedMemory.end())
	{
		delete (*it);
		it++;
	}

}

void Player::allocateStorage(bool **&marked, Site* **&prev, int **&dist)
{
	int i;
	int j;

	// Create a 2D array of distances between monster and other vertices.
	dist = new (nothrow) int *[N];
	for (i = 0; i < N; i++)
		dist[i] = new (nothrow) int [N];

	// Create a 2D array of previous site pointers.
	prev = new (nothrow) Site* *[N];
	for (i = 0; i < N; i++)
		prev[i] = new (nothrow) Site* [N];

	// Create a 2D array of bools to mark visited vertices.
	marked = new (nothrow) bool *[N];
	for (i = 0; i < N; i++)
		marked[i] = new (nothrow) bool [N];

	// Initialize them all as false.
	for (i = 0; i < N; i++)
		for (j = 0; j < N; j++)
			marked[i][j] = false;


}

