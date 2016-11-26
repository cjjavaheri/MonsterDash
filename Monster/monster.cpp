#include "monster.h"

Monster::Monster(Playfield* p) 
{
   playfield = p;
   N       = playfield->size();
}

Monster::~Monster()
{

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
   Site* nextMove;
   const Site* tryMove;
   vector<Site*> allocatedMemory;
   cout << "Monster[i][j] " << monster->i() << " " << monster->j() << endl;
   cout << "PLayer[i][j] " << player->i() << " " << player->j() << endl;
   cout << endl << endl;

	// Create the 2D arrays needed for bfs.
	allocateStorage(marked, prev, dist);

	// Perform a breadth-first-search on the playfield.
	bfs(marked, prev, dist, monster, allocatedMemory);

	// Check to make sure a path exists to the player.
	if (!marked[player->i()][player->j()])
	{
		deallocateStorage(marked, prev, dist, allocatedMemory);
		return monster;
	}

	// Calculate the next move on the shortest path the monster needs to take.
	nextMove = getNextMove(marked, prev, dist, monster, player, allocatedMemory);

	// Create the pointer to the next move.
	tryMove = new Site(nextMove->i(), nextMove->j());

	// Free up any allocated memory used.
	deallocateStorage(marked, prev, dist, allocatedMemory);

	//Make sure the move is legal.
	if (playfield->isLegalMove(monster, tryMove))
		return tryMove;

	return nullptr;


}



 Site* Monster::getNextMove(bool **&marked, Site* **&prev, int **&dist, const Site* monster, const Site* player, vector<Site*> &allocatedMemory)
{
	unsigned int i;
	unsigned int j;
	Site* nextMove = nullptr;

	i = player->i();
	j = player->j();

	// Starting distance is not 1
	if (dist[i][j] != 1)
	{

		while (dist[i][j] != 1)
		{
			nextMove = prev[i][j];
			i = nextMove->i();
			j = nextMove->j();
		}
		return nextMove;

	}

	// Starting distance is 1 ; therefore, the monster only has to take
	// one more move to reach the player.
	else
	{
		nextMove = new Site(player->i(), player->j());
		allocatedMemory.push_back(nextMove);
		return nextMove;
	}


}


void Monster::bfs(bool **&marked, Site* **&prev, int **&dist, const Site* monster, vector<Site*> &allocatedMemory)
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
			checkAdjacentRoomSquares(marked, prev, dist, myqueue, temp, allocatedMemory);


		// Temp must be a corridor site, so check all 4 adjacent vertices.
		else
			checkAdjacentCorridorSquares(marked, prev, dist, myqueue, temp, allocatedMemory);

	}

	// Store monsterSite in order to free the memory later.
	allocatedMemory.push_back(monsterSite);

}

void Monster::checkAdjacentCorridorSquares(bool **marked, Site* **prev, int **dist, queue<Site*> &myqueue, Site* temp, vector<Site*> &allocatedMemory)
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

void Monster::checkAdjacentRoomSquares(bool **marked, Site* **prev, int **dist, queue<Site*> &myqueue, Site* temp, vector<Site*> &allocatedMemory)
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

void Monster::deallocateStorage(bool **&marked, Site* **&prev, int **&dist, vector<Site*> &allocatedMemory)
{
	int i;
	vector<Site*>::iterator it;

	// Free all of the 2D arrays.
	for (i = 0; i < N; i++)
		delete[] marked[i];

	delete[] marked;

	for (i = 0; i < N; i++)
		delete[] prev[i];

	delete[] prev;

	for (i = 0; i < N; i++)
		delete[] dist[i];

	delete[] dist;

	// Free all allocated site pointers.
	it = allocatedMemory.begin();
	while (it != allocatedMemory.end())
	{
		delete (*it);
		it++;
	}

}

void Monster::allocateStorage(bool **&marked, Site* **&prev, int **&dist)
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
