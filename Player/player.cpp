
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
    adj = findAdjLists(corridors);

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

map<Site*, vector<Site*>> Player::findAdjLists(vector<Site*> corridors)
{
    vector<Site*>::iterator it;
    vector<Site*>::iterator curr;
    vector<Site*> adjacentVertices;
    Site* currSite;
    Site* compareSite;
    map<Site*, vector<Site*>> adj;

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
    map<Site*, vector<Site*>>::iterator it;
    map<Site*, vector<Site*>>::iterator adjDiscIt;
    map<Site*, vector<Site*>> adjDisc;
    map<Site*, vector<Site*>> adjConn;
    map<Site*, vector<Site*>> connectedCycle;
    vector<Site*>::iterator adjDiscVectIt;
    vector<Site*> adjVect;
    vector<Site*>::iterator vectIt;
    Site* nextMove = nullptr;
    static vector<Site*> cycle;
    bool hasCorridors;
    bool hasWalls;
    int disc;

    nextMove = new Site(player->i(), player->j());
    allocatedMemory.push_back(nextMove);

    hasCorridors = doCorridorsExist();

    if (!hasCorridors)
    {
        if (!markedPlayer[monster->i()][monster->j()])
            return nextMove;

        else
        {
            hasWalls = doWallsExist();
            if (!hasWalls)
            {
                nextMove = keepDistance(distMonster, player, allocatedMemory);
                return nextMove;
            }
        }
    }

    disc = countDisconnectedComponents(adj);
    cout << "Dist: " << disc << endl;

    if (disc != 0)
    {
        removeDeadEnds(adj);

        disc = countDisconnectedComponents(adj);
        cout << "Dist: " << disc << endl;

        if (disc != 0)
        {
            adjDisc = getAdjListDisc(adj, allocatedMemory);
            return calculateNextRoom(adjDisc, distMonster, distPlayer, prevPlayer, player, monster);

        }
    }

	adjConn = findConnectedComponents(adj);
	connectedCycle = getCyclesWithinCorridors(adjConn);

	if (!connectedCycle.empty())
	{
		return findCorridorCycle(connectedCycle, distMonster, distPlayer, prevPlayer, player, monster);
	}


}

map<Site*, vector<Site*>> Player::findConnectedComponents(map<Site*, vector<Site*>> adj)
{
	map<Site*, vector<Site*>> adjConn;
	map<Site*, vector<Site*>>::iterator it;
	map<Site*, vector<Site*>>::iterator trav;
	map<Site*, vector<Site*>>::iterator eraseMap;
	vector<Site*> connectedComp;
	vector<Site*>::iterator vectIt;
	stack<Site*> myStack;
	Site* site;
	int i;
	int j;
	bool **marked = nullptr;

	getMarkedArray(marked);

	it = adj.begin();
	while (it != adj.end())
	{
		myStack.push(it->first);
		while (!myStack.empty())
		{
			site = myStack.top();
			i = site->i();
			j = site->j();
			myStack.pop();

			if (!marked[i][j])
				marked[i][j] = true;

			// Visit adjacent vertices.
			trav = adj.find(site);
			vectIt = trav->second.begin();
			while (vectIt != trav->second.end())
			{
				i = (*vectIt)->i();
				j = (*vectIt)->j();
				if (!marked[i][j])
				{
					myStack.push(*vectIt);
					connectedComp.push_back(*vectIt);
				}

				vectIt++;
			}
			
		}

		adjConn.insert({it->first, connectedComp});
		connectedComp.clear();

		it++;
	}


	   it = adjConn.begin();
    	while (it != adjConn.end())
    	{
       		if (it->second.size() == 0)
        	{
            		eraseMap = it;
            		++it;
            		adjConn.erase(eraseMap);
        	}
        	else
            		it++;

    	}

	/*
	it = adjConn.begin();
	while (it != adjConn.end())
	{
		cout << "Start----------------------------" << endl;
		cout << it->first->i() << " " << it->first->j() << endl;
		vectIt = it->second.begin();
		while (vectIt != it->second.end())
		{
			cout << (*vectIt)->i() << " " << (*vectIt)->j() << endl;
			vectIt++;
		}
		cout << "--------------" << it->second.size() << endl;
		it++;
	}
	*/
	return adjConn;

}

map<Site*, vector<Site*>> Player::getCyclesWithinCorridors(map<Site*, vector<Site*>> adjConn)
{
	map<Site*, vector<Site*>> cycle;
	map<Site*, vector<Site*>>::iterator it;
	vector<Site*>::iterator vectIt;
	vector<Site*>::iterator trav;
	vector<Site*> store;
	Site* site;


	it = adjConn.begin();
	while (it != adjConn.end())
	{
		vectIt = it->second.begin();
		while (vectIt != it->second.end())
		{
			site = *vectIt;
			trav = vectIt;
			store.push_back(*trav);
			trav++;
			while (trav != it->second.end())
			{
				store.push_back(*trav);
				if ((*trav)->i() == site->i() && (*trav)->j() == site->j())
				{
					store.push_back(it->first);
					cycle.insert({it->first, store});
				}

				trav++;
			}
			
			store.clear();
			vectIt++;
		}

		it++;
	}


		it = cycle.begin();
	while (it != cycle.end())
	{
		cout << "Start----------------------------" << endl;
		cout << it->first->i() << " " << it->first->j() << endl;
		vectIt = it->second.begin();
		while (vectIt != it->second.end())
		{
			cout << (*vectIt)->i() << " " << (*vectIt)->j() << endl;
			vectIt++;
		}
		cout << "--------------" << it->second.size() << endl;
		it++;
	}

	return cycle;

}


void Player::getMarkedArray(bool **&marked)
{
	int i;
	int j;

    // Create a 2D array of bools to mark visited vertices.
    marked = new (nothrow) bool *[N];
    for (i = 0; i < N; i++)
        marked[i] = new (nothrow) bool [N];

    // Initialize them all as false.
    for (i = 0; i < N; i++)
        for (j = 0; j < N; j++)
            marked[i][j] = false;
}

Site* Player::calculateNextRoom(map<Site*, vector<Site*>> &adjDisc, int **&distMonster, int**&distPlayer, Site* **&prevPlayer, const Site* player, const Site* monster)
{
    unsigned int i;
    unsigned int j;
    int shortestDist;
    int longestDist;
    map<Site*, vector<Site*>>::iterator it;
    map<Site*, vector<Site*>>::iterator eraseMap;
    map<Site*, vector<Site*>>::iterator mapIt;
    vector<Site*>::iterator vectIt;
    vector<Site*>::iterator eraseVect;
    Site* nearestCorr;
    static vector<Site*> cycle;

    // Save the previous corridor site that the player walks through.

    if (playfield->isCorridor(player->i(), player->j()))
    {
        nearestCorr = new Site(player->i(), player->j());
        cycle.push_back(nearestCorr);
    }

    if (playfield->isCorridor(monster->i(), monster->j()))
    {
        nearestCorr = new Site(monster->i(), monster->j());
        cycle.push_back(nearestCorr);
    }

    // Remove any vertices which have 1 or less than 1 vertices in their
    // adjacency list.
    removeDeadEndVertices(adjDisc);

    // If the monster is right behind the player, simply find a corridor site
    // in the current room with the highest number of adjacent corridor sites
    // to that particular corridor site.
    if (distMonster[player->i()][player->j()] < 5 && N > 20 && !cycle.empty())
    {
        run(player, nearestCorr, distPlayer, distMonster, cycle, adjDisc);
    }

    else if (distMonster[player->i()][player->j()] < 6 && N <= 20 && !cycle.empty())
    {
        run(player, nearestCorr, distPlayer, distMonster, cycle, adjDisc);
    }

    else
    {

        // If the monster is not right behind the player, then find the nearest corridor site.
        it = adjDisc.begin();
        shortestDist = distPlayer[it->first->i()][it->first->j()];
        nearestCorr = it->first;
        while (it != adjDisc.end())
        {
            if (distPlayer[it->first->i()][it->first->j()] <= shortestDist)
            {
                shortestDist = distPlayer[it->first->i()][it->first->j()];
                nearestCorr = it->first;

            }
            it++;
        }

        // Find that corridor site's adjacency list.
        it = adjDisc.begin();
        while (it != adjDisc.end() && (it->first->i() != nearestCorr->i() || it->first->j() != nearestCorr->j()))
            it++;


        // Look at all of the corridor vertices in the room. Take the path to the corridor site
        // which is the farthes away from the monster's location.
        vectIt = it->second.begin();
        longestDist = distMonster[(*vectIt)->i()][(*vectIt)->j()];
        nearestCorr = *vectIt;
        while (vectIt != it->second.end())
        {
            if (distMonster[(*vectIt)->i()][(*vectIt)->j()] >= longestDist )
            {
                longestDist = distMonster[(*vectIt)->i()][(*vectIt)->j()];
                nearestCorr = *vectIt;
            }

            vectIt++;
        }

    }

    // Calculate the path to that corridor site.
    i = nearestCorr->i();
    j = nearestCorr->j();

    if (i == player->i() && j == player->j())
        return nearestCorr;



    // Starting distance is not 1
    if (distPlayer[i][j] != 1)
    {

        while (distPlayer[i][j] != 1)
        {
            nearestCorr = prevPlayer[i][j];
            i = nearestCorr->i();
            j = nearestCorr->j();
        }
        return nearestCorr;

    }

    // Starting distance is 1 ; therefore, the player only has to take one move
    // to reach its destination.
    else
    {

        return nearestCorr;
    }

}

void Player::run(const Site* player, Site* &nearestCorr, int **&distPlayer, int **&distMonster, vector<Site*> &cycle, map<Site*, vector<Site*>> &adjDisc)
{
    int i = player->i();
    int j = player->j();
    unsigned int a;
    unsigned int b;
    int longestDist;
    vector<Site*> vectDisc;
    vector<Site*>::iterator vectIt;
    vector<Site*>::iterator prevIt;
    vector<Site*>::iterator erase;
    map<Site*, vector<Site*>>::iterator it;
    map<int, Site*> vertices;
    map<int, Site*>::iterator myVertices;


    Site* site = new Site(player->i(), player->j());


    if (playfield->isRoom(player))
    {

        // Set i and j to bottom right of room in order to search
        // for adjacent corridor sites to the room.
        while (playfield->isRoom(i, j))
            j++;
        j--;

        while (playfield->isRoom(i, j))
            i++;

        i--;

        // Now search the room.
        search(vectDisc, site, i, j);
        checkDuplicates(vectDisc, site);



        prevIt = cycle.end();
        prevIt--;


        // Delete the previous corridor site from the monster's list of decisions.
        a = (*prevIt)->i();
        b = (*prevIt)->j();


        vectIt = vectDisc.begin();
        while (vectIt != vectDisc.end())
        {
            if ((*vectIt)->i() == a && (*vectIt)->j() == b)
            {
                erase = vectIt;
                vectDisc.erase(erase);
                vectIt = vectDisc.end();

            }

            else
                vectIt++;
        }



        // Check each corridor site adjacent to the current room the player is in.
        // Take the path to the one with the highest number of adjacent vertices.
        vectIt = vectDisc.begin();
        while (vectIt != vectDisc.end())
        {
            a = (*vectIt)->i();
            b = (*vectIt)->j();

            it = adjDisc.begin();
            while (it != adjDisc.end())
            {
                if (it->first->i() == a && it->first->j() == b)
                {
                    vertices.insert({it->second.size(), *vectIt});

                }

                it++;
            }

            vectIt++;
        }

        myVertices = vertices.begin();

        while (myVertices != vertices.end())
            myVertices++;

        myVertices--;
        nearestCorr = myVertices->second;

    }

    // If the player is on a corridor site, then enter the next room by looking at which
    // room site has a higher distance away from the monster.
    else
    {
        longestDist = distMonster[i][j];
        if (i + 1 < N)
            if (distMonster[i + 1][j] > longestDist && playfield->isRoom(i + 1, j))
                nearestCorr = new Site(i + 1, j);


        if (i - 1 > -1)
            if (distMonster[i - 1][j] > longestDist && playfield->isRoom(i - 1, j))
                nearestCorr = new Site(i - 1, j);

        if (j + 1 < N)
            if (distMonster[i][j + 1] > longestDist && playfield->isRoom(i, j + 1))
                nearestCorr = new Site(i, j + 1);

        if (j - 1 > -1)
            if (distMonster[i][j - 1] > longestDist && playfield->isRoom(i, j - 1))
                nearestCorr = new Site(i, j - 1);


    }

}

void Player::removeDeadEndVertices(map<Site*, vector<Site*>> &adjDisc)
{
    map<Site*, vector<Site*>>::iterator it;
    map<Site*, vector<Site*>>::iterator eraseMap;
    map<Site*, vector<Site*>>::iterator mapIt;
    vector<Site*>::iterator vectIt;
    vector<Site*>::iterator eraseVect;
    Site* test;
    unsigned int i;
    unsigned int j;
    bool exist;

    // Remove any vertices which have 1 or less than 1 adjacent vertices
    // in their adjacency list.
    it = adjDisc.begin();
    while (it != adjDisc.end())
    {
        if (it->second.size() <= 1)
        {
            eraseMap = it;
            ++it;
            adjDisc.erase(eraseMap);
        }
        else
            it++;

    }

    // Since these vertices no longer exist, i.e. we can no longer access their adjacency
    // list, we must remove them from other vertices' adjacency lists.
    it = adjDisc.begin();
    while (it != adjDisc.end())
    {
        vectIt = it->second.begin();
        while (vectIt != it->second.end())
        {
            exist = false;
            test = *vectIt;
            i = test->i();
            j = test->j();
            mapIt = adjDisc.begin();
            while (mapIt != adjDisc.end())
            {
                if (i == mapIt->first->i() && j == mapIt->first->j())
                    exist = true;
                mapIt++;
            }

            if (!exist)
            {
                eraseVect = vectIt;
                ++vectIt;
                (it->second).erase(eraseVect);
            }

            else
                vectIt++;
        }

        ++it;
    }

}

map<Site*, vector<Site*>> Player::getAdjListDisc(map<Site*, vector<Site*>> &adj, vector<Site*> &allocatedMemory)
{
    map<Site*, vector<Site*>>::iterator it;
    vector<Site*>::iterator memIt;
    map<Site*, vector<Site*>> adjDisc;
    vector<Site*> vectDisc;
    Site* site;
    int i;
    int j;

    // First, create a map of only the disconnected vertices.
    it = adj.begin();
    while (it != adj.end())
    {
        if (it->second.empty())
        {
            adjDisc.insert({it->first, it->second});
        }

        it++;
    }

    // Next, check each disconnected vertex to find their adjacent vertices.
    // Adjacency is redefined in this context to be corridor sites that have adjacent
    // rooms in common.
    it = adjDisc.begin();
    while (it != adjDisc.end())
    {
        site = it->first;
        i = site->i();
        j = site->j();

        if (playfield->isRoom(i, j - 1))
        {

            // Set i and j to bottom right of room.
            j--;
            while (playfield->isRoom(i, j))
                i++;

            i--;

            // Now begin search.
            search(vectDisc, site, i, j);
            checkDuplicates(vectDisc, site);

            i = site->i();
            j = site->j();
        }

        if (playfield->isRoom(i, j + 1))
        {

            // Set i and j to bottom right.
            j++;
            while (playfield->isRoom(i, j))
                i++;

            i--;

            while (playfield->isRoom(i, j))
                j++;

            j--;

            // Now search the room.
            search(vectDisc, site, i, j);
            checkDuplicates(vectDisc, site);

            i = site->i();
            j = site->j();
        }

        if (playfield->isRoom(i + 1, j))
        {

            // Set i and j to bottom right.
            i++;

            while (playfield->isRoom(i, j))
                j++;
            j--;

            while (playfield->isRoom(i, j))
                i++;

            i--;

            // Now search the room.
            search(vectDisc, site, i, j);
            checkDuplicates(vectDisc, site);

            i = site->i();
            j = site->j();
        }

        if (playfield->isRoom(i - 1, j))
        {

            // Set i and j to bottom right of room.
            i--;

            while (playfield->isRoom(i, j))
                j++;

            j--;

            // Search room for adjacent corridor sites.
            search(vectDisc, site, i, j);
            checkDuplicates(vectDisc, site);

            i = site->i();
            j = site->j();
        }

        it->second = vectDisc;
        memIt = vectDisc.begin();
        while (memIt != vectDisc.end())
        {
            allocatedMemory.push_back((*memIt));
            memIt++;
        }
        vectDisc.clear();
        it++;
    }

    return adjDisc;

}

void Player::checkDuplicates(vector<Site*> &vectDisc, Site* site)
{
    // Check to make sure that the current corridor site the player
    // is standing on is NOT apart of the adjacency list.
    vector<Site*>::iterator it;
    it = vectDisc.begin();
    while (it != vectDisc.end())
    {
        if ((*it)->i() == site->i() && (*it)->j() == site->j())
        {
            it = vectDisc.erase(it);
        }

        else
            it++;
    }


}

void Player::search(vector<Site*> &vectDisc, Site* site, int &i, int &j)
{

    int count = 0;
    Site* newSite;


    // Search a room starting from the bottom right looking
    // for adjacent corridor sites.

    // Search from bottom right to bottom left.
    while (playfield->isRoom(i, j))
    {
        if (playfield->isCorridor(i + 1, j))
        {
            count = 0;

            if (!playfield->isCorridor(i, j))
                count++;

            if (!playfield->isCorridor(i + 2, j))
                count++;

            if (!playfield->isCorridor(i + 1, j - 1))
                count++;

            if (!playfield->isCorridor(i + 1, j + 1))
                count++;

            if (count == 4 )
            {
                newSite = new Site(i + 1, j);
                vectDisc.push_back(newSite);
            }

        }

        j--;
    }

    // Search from bottom left to to top left.

    j++;
    while (playfield->isRoom(i, j))
    {
        if (playfield->isCorridor(i, j - 1))
        {
            count = 0;

            if (!playfield->isCorridor(i, j))
                count++;

            if (!playfield->isCorridor(i - 1, j - 1))
                count++;

            if (!playfield->isCorridor(i + 1, j - 1))
                count++;

            if (!playfield->isCorridor(i + 1, j - 1))
                count++;

            if (count == 4 )
            {
                newSite = new Site(i , j - 1);
                vectDisc.push_back(newSite);
            }

        }

        i--;
    }

    i++;

    // Search from top left to top right.
    while (playfield->isRoom(i, j))
    {
        if (playfield->isCorridor(i - 1, j))
        {
            count = 0;

            if (!playfield->isCorridor(i, j))
                count++;

            if (!playfield->isCorridor(i  - 2, j))
                count++;

            if (!playfield->isCorridor(i - 1, j - 1))
                count++;

            if (!playfield->isCorridor(i - 1, j + 1))
                count++;

            if (count == 4  )
            {
                newSite = new Site(i - 1, j);
                vectDisc.push_back(newSite);
            }

        }

        j++;
    }

    // Search from top right to bottom right.

    j--;
    while (playfield->isRoom(i, j))
    {
        if (playfield->isCorridor(i, j + 1))
        {
            count = 0;

            if (!playfield->isCorridor(i, j))
                count++;

            if (!playfield->isCorridor(i - 1, j + 1))
                count++;

            if (!playfield->isCorridor(i + 1, j + 1))
                count++;

            if (!playfield->isCorridor(i, j + 2))
                count++;

            if (count == 4  )
            {
                newSite = new Site(i, j + 1);
                vectDisc.push_back(newSite);
            }

        }

        i++;
    }

    i--;

}

void Player::removeDeadEnds(map<Site*, vector<Site*>> &adj)
{
    map<Site*, vector<Site*>>::iterator it;
    map<Site*, vector<Site*>>::iterator erase;
    it = adj.begin();
    int i;
    int j;
    Site* site;
    int count;

    // Erase corridor sites that are dead ends.
    while (it != adj.end())
    {
        count = 0;
        site = it->first;
        i = site->i();
        j = site->j();

        if (playfield->isRoom(i - 1, j))
        {
            count++;
        }

        if (playfield->isRoom(i + 1, j))
        {
            count++;
        }

        if (playfield->isRoom(i , j - 1))
        {
            count++;
        }

        if (playfield->isRoom(i , j + 1))
        {
            count++;
        }

        if (count < 2)
        {
            if (it->second.empty())
            {
                erase = it;
                ++it;
                adj.erase(erase);

            }
            else
                it++;
        }
        else
            it++;
    }

}

int Player::countDisconnectedComponents(map<Site*, vector<Site*>> &adj)
{
    int count = 0;
    map<Site*, vector<Site*>>::iterator it;

    it = adj.begin();
    while (it != adj.end())
    {
        if (it->second.empty())
            count++;

        it++;
    }

    return count;

}

Site* Player::keepDistance(int **&distMonster, const Site* player, vector<Site*> &allocatedMemory)
{
    unsigned int i;
    unsigned int j;
    int longestDist = distMonster[player->i()][player->j()];
    Site* nextMove = new Site(player->i(), player->j());
    allocatedMemory.push_back(nextMove);

    // Calculate longest path for monster. Assumes continuity between room sites.
    for (i = player->i() - 1; i < player->i() + 2; i++)
        for (j = player->j() - 1; j < player->j() + 2; j++)
            if (playfield->isRoom(i, j))
            {
                if (distMonster[i][j] > longestDist)
                {
                    longestDist = distMonster[i][j];
                    nextMove = new Site(i, j);
                    allocatedMemory.push_back(nextMove);

                }
            }

    return nextMove;


}

bool Player::doWallsExist()
{
    int i;
    int j;
    bool walls = false;

    for (i = 0; i < N; i++)
        for (j = 0; j < N; j++)
            if (playfield->isWall(i, j))
                walls = true;

    return walls;

}

bool Player::doCorridorsExist()
{
    int i;
    int j;
    bool corridors = false;

    for (i = 0; i < N; i++)
        for (j = 0; j < N; j++)
            if (playfield->isCorridor(i, j))
                corridors = true;

    return corridors;

}

Site* Player::returnToStart(map<Site*, vector<Site*>> &adj, int **&distPlayer, Site* **&prevPlayer, vector<Site*> &allocatedMemory, Site* &start, const Site* player)
{

    Site* nextMove = nullptr;
    unsigned int i;
    unsigned int j;
    int shortestDist;
    map<Site*, vector<Site*>>::iterator it;

    //Dynamically allocate the player's site.
    nextMove = new Site(player->i(), player->j());
    allocatedMemory.push_back(nextMove);

    i = player->i();
    j = player->j();

    // If cycle hasn't been traversed at least one time.
    if (start == nullptr)
    {
        // Initialize shortestDist to first vertex in the adjacency list.
        it = adj.begin();
        shortestDist = distPlayer[(it->first)->i()][(it->first)->j()];
        nextMove = new Site((it->first)->i(), (it->first)->j());
        allocatedMemory.push_back(nextMove);

        // Calculate the shortest distance to the nearest corridor.
        while (it != adj.end() && ((it->first)->i() != i || (it->first)->j() != j))
        {
            if (distPlayer[(it->first)->i()][(it->first)->j()] < shortestDist)
            {
                shortestDist = distPlayer[(it->first)->i()][(it->first)->j()];
                nextMove = new Site((it->first)->i(), (it->first)->j());
                allocatedMemory.push_back(nextMove);
            }
            it++;
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

    }

    else if (start != nullptr)
    {


        nextMove = start;
        i = start->i();
        j = start->j();


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


    }

}

Site* Player::findCorridorCycle(map<Site*, vector<Site*>> connectedCycle, int **&distMonster, int **&distPlayer, Site* **&prevPlayer, const Site* player, const Site* monster)
{
	map<Site*, vector<Site*>>::iterator it;
	vector<Site*>::iterator vectIt;
	Site* nextMove = nullptr;
	map<Site*, vector<Site*>> adjList;
	bool inCycle;
	unsigned int i;
	unsigned int j;
	int longestDist;

	inCycle = isPlayerInCorridorCycle(connectedCycle, player);

	cout << "inCycle: " << inCycle << endl;

	if (!inCycle)
	{
	it = connectedCycle.begin();
	while (it != connectedCycle.end())
	{
		vectIt = it->second.begin();
		while (vectIt != it->second.end())
		{
			if (distPlayer[(*vectIt)->i()][(*vectIt)->j()] < distMonster[(*vectIt)->i()][(*vectIt)->j()])
			{
				nextMove = *vectIt;
			}

			vectIt++;
		}

		it++;
	}

	}

	else
	{
		i = player->i();
		j = player->j();
		it = connectedCycle.begin();
		while (it != connectedCycle.end())
		{
			vectIt = it->second.begin();
			while (vectIt != it->second.end())
			{
				if ((*vectIt)->i() == i && (*vectIt)->j() == j)
				{
					adjList = findAdjLists(it->second);
				}

				vectIt++;
			}

			it++;
		}

		it = adjList.begin();
		while (it != adjList.end())
		{
			if (i == it->first->i() && j == it->first->j())
			{
				longestDist = distMonster[i][j];
				nextMove = it->first;
				vectIt = it->second.begin();
				while (vectIt != it->second.end())
				{
					if (distMonster[(*vectIt)->i()][(*vectIt)->j()] > longestDist)
					{
						longestDist = distMonster[(*vectIt)->i()][(*vectIt)->j()];
						nextMove = *vectIt;
					}

					vectIt++;
				}
			}

			it++;
		}
	}

	i = nextMove->i();
	j = nextMove->j();

	if (i == player->i() && j == player->j())
		return nextMove;


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

        // Starting distance is 1 ; therefore, the player only has to take
	// one more move to reach the final destination.
        else
        {

            return nextMove;
        }

}

bool Player::isPlayerInCorridorCycle(map<Site*, vector<Site*>> connectedCycle, const Site* player)
{
		map<Site*, vector<Site*>>::iterator it;
		vector<Site*>::iterator vectIt;
		unsigned int i;
		unsigned int j;

		i = player->i();
		j = player->j();

		it = connectedCycle.begin();
	while (it != connectedCycle.end())
	{
		vectIt = it->second.begin();
		while (vectIt != it->second.end())
		{
			if ((*vectIt)->i() == i && (*vectIt)->j() == j)
				return true;

			vectIt++;
		}

		it++;
	}

	return false;

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

