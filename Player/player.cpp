
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

    if (!markedPlayer[monster->i()][monster->j()])
        return player;

    // Store all corridor sites in a vector.
    findCorridors(corridors);

    // Get the list of adjacent corridor sites to each corridor site.
    adj = findAdjLists(corridors);

    nextMove = getNextMove(markedMonster, prevMonster, distMonster, markedPlayer, prevPlayer, distPlayer, monster, player, allocatedMemory, adj);

    tryMove = new Site(nextMove->i(), nextMove->j());

    // Free any dynamically allocated memory up.
    deallocateStorage(markedMonster, prevMonster, distMonster, markedPlayer, prevPlayer, distPlayer, allocatedMemory);

    return tryMove;

}

map<Site*, vector<Site*>> Player::findAdjRoomLists(vector<Site*> rooms)
{
    vector<Site*>::iterator it;
    vector<Site*>::iterator curr;
    vector<Site*> adjacentVertices;
    Site* currSite;
    Site* compareSite;
    map<Site*, vector<Site*>> adj;

    curr = rooms.begin();
    // For each corridor site, check all of the corridor sites to see which ones are adjacent.
    while (curr != rooms.end())
    {
        currSite = (*curr);
        it = rooms.begin();
        adjacentVertices.clear();
        while (it != rooms.end())
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


            // Check the lower right site.
            if (currSite->i() + 1 == compareSite->i() && currSite->j() + 1 == 				compareSite->j())
                adjacentVertices.push_back(compareSite);

            // Check the lower left side.
            if (currSite->i() - 1 == compareSite->i() && currSite->j() - 1 == 				compareSite->j())
                adjacentVertices.push_back(compareSite);

            // Check the upper left side.
            if (currSite->i() - 1 == compareSite->i() && currSite->j() - 1 == 				compareSite->j())
                adjacentVertices.push_back(compareSite);

            // Check the upper right side.
            if (currSite->i() - 1 == compareSite->i() && currSite->j() + 1 == 				compareSite->j())
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
                   
                    corridors.push_back(addSite);
                }
            }

    return corridors;

}





Site* Player::getNextMove(bool **&markedMonster, Site* **&prevMonster, int **&distMonster, bool **&markedPlayer, Site* **&prevPlayer, int **&distPlayer, const Site* monster, const Site* player, vector<Site*> &allocatedMemory, map<Site*, vector<Site*>> &adj)
{
    map<Site*, vector<Site*>> mapRoomCycle;
    map<Site*, vector<Site*>>::iterator it;
    map<Site*, vector<Site*>>::iterator adjDiscIt;
    static map<Site*, vector<Site*>> adjDisc;
    static map<Site*, vector<Site*>> adjConn;
    static map<Site*, vector<Site*>> connectedCycle;
    static map<Site*, vector<Site*>> cycleBetweenRooms;
    static map<Site*, vector<Site*>> temp;
    vector<Site*>::iterator adjDiscVectIt;
    vector<Site*> adjVect;
    static vector<Site*> roomCycle;
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
                nextMove = stayAliveAsLongAsPossible(distMonster, distPlayer);
                return calculateFinalDestination(nextMove, distPlayer, prevPlayer, player);
            }

            else
            {
                return getRoomCycle(distMonster, distPlayer, prevPlayer, player);
            }


        }
    }

    disc = countDisconnectedComponents(adj);
 


    if (adjConn.empty())
        adjConn = findConnectedComponents(adj);


    if (connectedCycle.empty())
        connectedCycle = getCyclesWithinCorridors(adjConn);


    if (disc != 0)
    {

        temp = adj;

        removeDeadEnds(adj);

        disc = countDisconnectedComponents(adj);
   

        if (disc != 0)
        {
            adjDisc = getAdjListDisc(adj, allocatedMemory);
            nextMove =  calculateNextRoom(adjDisc, distMonster, distPlayer, prevPlayer, player, monster, connectedCycle);
            if (nextMove != nullptr)
                return nextMove;


            adj = temp;

        }
    }


    if (!connectedCycle.empty())
    {
        adjConn = checkForConnectedDeadEnds(adjConn);

        nextMove = findCorridorCycle(connectedCycle, adjConn, distMonster, distPlayer, prevPlayer, player, monster);

        if (nextMove != nullptr)
            return nextMove;
    }

  
    adjConn = checkForConnectedDeadEnds(adjConn);
    nextMove = findCyclesBetweenRooms(adjConn, adj, distMonster, distPlayer, prevPlayer, player, monster);

    it = adjConn.begin();
    while (it != adjConn.end())
    {
       
        vectIt = it->second.begin();
        while (vectIt != it->second.end())
        {
            
            vectIt++;
        }

        it++;
    }

    return nextMove;


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


Site* Player::chooseNextRoom(int **&distMonster, int **&distPlayer, Site* **&prevPlayer, vector<Site*> roomCycle, const Site* player)
{
    vector<Site*> possibleMoves;
    vector<Site*>::iterator vectIt;
    unsigned int i;
    unsigned int j;
    multimap<int, Site*> decisions;
    multimap<int, Site*>::iterator decIt;
    Site* nextMove = nullptr;
    map<Site*, vector<Site*>> adjRooms;
    map<Site*, vector<Site*>>::iterator adjIt;
    int farthestDistance;

    vectIt = roomCycle.begin();
    while (vectIt != roomCycle.end())
    {
        i = (*vectIt)->i();
        j = (*vectIt)->j();

      
        if (distPlayer[i][j] < distMonster[i][j] )
        {

            decisions.insert({distMonster[i][j], *vectIt});
        }

        vectIt++;

    }

    

    if (decisions.size() != 0)
    {
        decIt = decisions.end();
        decIt--;

        farthestDistance = decIt->first;
        while (decIt != decisions.begin())
        {
            if (decIt->first == farthestDistance)
                possibleMoves.push_back(decIt->second);
            decIt--;
        }

        vectIt = possibleMoves.begin();
        while (vectIt != possibleMoves.end())
        {
            if ((*vectIt)->i() != player->i() && (*vectIt)->j() != player->j())
                nextMove = *vectIt;

            vectIt++;
        }

        if (nextMove == nullptr)
        {
            decIt = decisions.end();
            decIt--;
            nextMove = decIt->second;
        }

       
        nextMove = calculateFinalDestination(nextMove, distPlayer, prevPlayer, player);


    }

    else
    {
        vectIt = roomCycle.begin();
        while (vectIt != roomCycle.end())
        {
            i = (*vectIt)->i();
            j = (*vectIt)->j();

            decisions.insert({distPlayer[i][j], *vectIt});

            vectIt++;
        }

        decIt = decisions.begin();

        nextMove = decIt->second;
        nextMove = calculateFinalDestination(nextMove, distPlayer, prevPlayer, player);
    }



    if (nextMove->i() == player->i() && nextMove->j() == player->j())
    {
        decisions.clear();
        adjRooms = findAdjRoomLists(roomCycle);
        adjIt = adjRooms.begin();
        while (adjIt != adjRooms.end() && (adjIt->first->i() != player->i() || adjIt->first->j() != player->j()))
            adjIt++;

        vectIt = adjIt->second.begin();
        while (vectIt != adjIt->second.end())
        {
            i = (*vectIt)->i();
            j = (*vectIt)->j();

            decisions.insert({distMonster[i][j], *vectIt});

            vectIt++;
        }

        decIt = decisions.end();
        decIt--;

        nextMove = decIt->second;
    }


    return nextMove;

}


Site* Player::getRoomCycle(int **&distMonster, int **&distPlayer, Site* **&prevPlayer, const Site* player)
{
    static vector<Site*> roomCycle;
    vector<Site*>::iterator vectIt;
    vector<Site*>::iterator erase;
    map<Site*, vector<Site*>> adjConn;
    map<Site*, vector<Site*>> adj;
    map<Site*, vector<Site*>>::iterator it;
    multimap<Site*, vector<Site*>> allCycles;
    multimap<Site*, vector<Site*>>::iterator multiIt;
    multimap<int, vector<Site*>> vectorSizes;
    Site* site;
    int i;
    int j;

    if (roomCycle.empty())
    {

        for (i = 0; i < N; i++)
            for (j = 0; j < N; j++)
                if (playfield->isRoom(i, j))
                {
                    site = new Site(i, j);
                    roomCycle.push_back(site);
                }


        roomCycle = removeDiagonalRooms(roomCycle);

        adj = findAdjLists(roomCycle);

        adjConn = findConnectedComponents(adj);

        allCycles = findAllRoomCycles(adjConn);

        multiIt = allCycles.begin();
        while (multiIt != allCycles.end())
        {
            vectorSizes.insert({multiIt->second.size(), multiIt->second});
            multiIt++;
        }

        roomCycle = getCorrectCycle(vectorSizes, distPlayer, distMonster);

        roomCycle = removeSitesWithOneAdjacentSite(roomCycle);


        vectIt = roomCycle.begin();
        adjConn.clear();
        adjConn.insert({*vectIt, roomCycle});

        it = adjConn.begin();
        while (it != adjConn.end())
        {
            vectIt = it->second.begin();
            while (vectIt != it->second.end())
            {
              
                vectIt++;
            }

            it++;
        }

    }

    return chooseNextRoom(distMonster, distPlayer, prevPlayer, roomCycle, player);

}

vector<Site*> Player::getCorrectCycle(multimap<int, vector<Site*>> vectorSizes, int **&distPlayer, int **&distMonster)
{
    multimap<int, vector<Site*>>::iterator it;
    vector<Site*>::iterator vectIt;
    unsigned int i;
    unsigned int j;

    it = vectorSizes.end();
    it--;

    while (it != vectorSizes.begin())
    {
        vectIt = it->second.begin();
        while (vectIt != it->second.end())
        {
            i = (*vectIt)->i();
            j = (*vectIt)->j();

            if (distPlayer[i][j] < distMonster[i][j])
            {
               
                return it->second;
            }

            vectIt++;
        }

        it--;
    }

    return it->second;

}


multimap<Site*, vector<Site*>> Player::findAllRoomCycles(map<Site*, vector<Site*>> adjConn)
{
    multimap<Site*, vector<Site*>> cycle;
    multimap<Site*, vector<Site*>>::iterator multiIt;
    map<Site*, vector<Site*>>::iterator it;
    vector<Site*>::iterator vectIt;
    vector<Site*>::iterator trav;
    vector<Site*> store;
    Site* site;

    // For each connected component, look at all of the vertices in that component.
    it = adjConn.begin();
    while (it != adjConn.end())
    {
        vectIt = it->second.begin();
        // For each vertex in a particular component, determine if a particular vertex
        // is found twice.
        while (vectIt != it->second.end())
        {
            site = *vectIt;
            trav = vectIt;
            store.push_back(*trav);
            trav++;
            // If a particular vertex is found twice, then it is a cycle.
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


    multiIt = cycle.begin();
    while (multiIt != cycle.end())
    {
       
       
        vectIt = multiIt->second.begin();
        while (vectIt != multiIt->second.end())
        {
            
            vectIt++;
        }
       
        multiIt++;
    }

    return cycle;

}


vector<Site*> Player::removeDiagonalRooms(vector<Site*> roomCycle)
{
    vector<Site*>::iterator vectIt;
    vector<Site*>::iterator trav;
    vector<Site*>::iterator erase;
    vector<Site*> remove;
    Site* site;
    int i;
    int j;
    bool adjWall;

    vectIt = roomCycle.begin();
    while (vectIt != roomCycle.end())
    {
        adjWall = false;

        i = (*vectIt)->i();
        j = (*vectIt)->j();

        if (i == 0 || i == N - 1 || j == 0 || j == N - 1)
            adjWall = checkPerimeterForAdjWalls(*vectIt);

        else
        {
            if (playfield->isWall(i - 1, j - 1) || playfield->isWall(i - 1, j) || playfield->isWall(i - 1, j + 1) || playfield->isWall(i, j - 1) || playfield->isWall(i, j) || playfield->isWall(i, j + 1) || playfield->isWall(i + 1, j - 1) || playfield->isWall(i + 1, j) || playfield->isWall(i + 1, j + 1))
                adjWall = true;

        }

        if (!adjWall)
        {
            i = (*vectIt)->i();
            j = (*vectIt)->j();

            if (playfield->isRoom(i + 1, j + 1) || playfield->isRoom(i + 1, j - 1) || playfield->isRoom(i - 1, j - 1) || playfield->isRoom(i - 1, j + 1))
            {
                site = new Site(i, j);
                remove.push_back(site);
            }



        }

        vectIt++;
    }

    vectIt = remove.begin();
    while ( vectIt != remove.end())
    {
        trav = roomCycle.begin();
        while (trav != roomCycle.end())
        {
            if ((*trav)->i() == (*vectIt)->i() && (*trav)->j() == (*vectIt)->j())
            {
                
                erase = trav;
                ++trav;
                roomCycle.erase(erase);
            }

            else
                trav++;
        }

        vectIt++;
    }

    vectIt = roomCycle.begin();
    while (vectIt != roomCycle.end())
    {
    
        vectIt++;
    }

    return roomCycle;

}

bool Player::checkPerimeterForAdjWalls(Site* site)
{
    int i = site->i();
    int j = site->j();

    if (i == 0 && j == 0)
        if (playfield->isWall(i + 1, j + 1) || playfield->isWall(i + 1, j) || playfield->isWall(i, j + 1))
            return true;

    if (i == 0 && j == N - 1)
        if (playfield->isWall(i + 1, j - 1) || playfield->isWall(i + 1, j) || playfield->isWall(i, j - 1))
            return true;

    if (i == N - 1 && j == N - 1)
        if (playfield->isWall(i - 1, j) || playfield->isWall(i - 1, j - 1) || playfield->isWall(i, j - 1))
            return true;

    if (i == N - 1 && j == 0)
        if (playfield->isWall(i - 1, j) || playfield->isWall(i - 1, j - 1) || playfield->isWall(i, j - 1))
            return true;




    if (i == 0)
    {

        if (playfield->isWall(i + 1, j + 1) || playfield->isWall(i + 1, j) || playfield->isWall(i + 1, j - 1) || playfield->isWall(i, j - 1) || playfield->isWall(i, j + 1))
            return true;

        return false;
    }

    if (i == N - 1)
    {

        if (playfield->isWall(i - 1, j - 1) || playfield->isWall(i - 1, j) || playfield->isWall(i - 1, j + 1) || playfield->isWall(i, j - 1) || playfield->isWall(i, j + 1))
            return true;

        return false;
    }


    if (j == 0)
    {
        if (playfield->isWall(i - 1, j) || playfield->isWall(i + 1, j) || playfield->isWall(i - 1, j + 1) || playfield->isWall(i, j + 1) || playfield->isWall(i + 1, j + 1))
            return true;

        return false;
    }

    if (j == N - 1)
    {
        if (playfield->isWall(i - 1, j) || playfield->isWall(i + 1, j) || playfield->isWall(i - 1, j - 1) || playfield->isWall(i, j - 1) || playfield->isWall(i + 1, j - 1))
            return true;
    }

    return false;



}

Site* Player::calculateFinalDestination(Site* nextMove, int**&distPlayer, Site* **&prevPlayer, const Site* player)
{
    unsigned int i;
    unsigned int j;

    // Calculate a path to the final destination by using the prev array used in bfs.
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

Site* Player::stayAliveAsLongAsPossible(int **&distMonster, int **&distPlayer)
{
    Site* nextMove = nullptr;
    Site* site;
    map<int, Site*> decisions;
    map<int, Site*>::iterator it;
    int i;
    int j;

    for (i = 0; i < N; i++)
        for (j = 0; j < N; j++)
        {
            if (playfield->isRoom(i, j) || playfield->isCorridor(i, j))
            {
                if (distPlayer[i][j] < distMonster[i][j])
                {
                    site = new Site(i, j);
                    decisions.insert({distMonster[i][j], site});
                }
            }
        }

    it = decisions.end();
    it--;
    nextMove = it->second;
    return nextMove;

}

Site* Player::findCyclesBetweenRooms(map<Site*, vector<Site*>> cycleBetweenRooms, map<Site*, vector<Site*>> adj, int **&distMonster, int **&distPlayer, Site* **&prevPlayer, const Site* player, const Site* monster)
{
    vector<Site*> corridors;
    vector<Site*>::iterator vectIt;
    map<Site*, vector<Site*>>::iterator it;
    map<Site*, vector<Site*>> adjList;
    static vector<Site*> move;
    multimap <int, Site*> decisions;
    multimap<int, Site*>::iterator decIt;
    multimap<int, Site*>::iterator erase;
    Site* nextMove = nullptr;
    Site* site;
    unsigned int i;
    unsigned int j;
    int longestDist;
    bool inCycle;

    inCycle = isPlayerInConnectedRoomCycle(cycleBetweenRooms, player);

   


    if (playfield->isCorridor(player))
    {
        site = new Site(player->i(), player->j());
        move.push_back(site);
    }

    if (playfield->isCorridor(monster))
    {
        site = new Site(monster->i(), monster->j());
        move.push_back(site);
    }

    if (playfield->isRoom(monster) && playfield->isRoom(player))
        move.clear();

    if (playfield->isRoom(player))
    {
        // Get all corridor sites adjacent to the room of the player.
        corridors = getCycleChoices(cycleBetweenRooms, player);
        vectIt = corridors.begin();
        while (vectIt != corridors.end())
        {
            // Choose an adjacent corridor site that the player can reach before
            // it is caught by the monster.
            if (distPlayer[(*vectIt)->i()][(*vectIt)->j()] < distMonster[(*vectIt)->i()][(*vectIt)->j()])
                nextMove = *vectIt;


            vectIt++;
        }

        if (nextMove == nullptr)
        {

            if (corridors.size() > 1)
            {
                vectIt = corridors.begin();
                while (vectIt != corridors.end())
                {
                    decisions.insert({distPlayer[(*vectIt)->i()][(*vectIt)->j()], *vectIt});
                    vectIt++;
                }

                decIt = decisions.begin();
                nextMove = decIt->second;
                decisions.clear();

            }

            else
                nextMove = stayAliveAsLongAsPossible(distMonster, distPlayer);

        }

    }

    else
    {

        nextMove = nullptr;
        if (!inCycle)
        {
            it = cycleBetweenRooms.begin();
            while (it != cycleBetweenRooms.end())
            {
                vectIt = it->second.begin();
                while (vectIt != it->second.end())
                {
                    i = (*vectIt)->i();
                    j = (*vectIt)->j();

                    if (distPlayer[i][j] < distMonster[i][j] && distPlayer[i][j] != 0)
                        nextMove = *vectIt;

                    vectIt++;
                }

                it++;
            }

        }

        if (nextMove == nullptr)
        {
            // Player must be on a corridor site.
            i = player->i();
            j = player->j();

            longestDist = distMonster[i][j];
            nextMove = new Site(i, j);

            // Check if there is an adjacent room to the corridor site the player is standing on.
            if (hasAdjacentRoom(nextMove))
            {
                // For each of the 4 possible choices, check to make sure it's legal
                // and it's a larger distance away from the monster.
                if (playfield->isRoom(i + 1, j) || playfield->isCorridor(i + 1, j))
                {
                    if (distMonster[i + 1][j] > longestDist)
                    {
                        longestDist = distMonster[i + 1][j];
                        nextMove = new Site(i + 1, j);
                    }
                }

                if (playfield->isRoom(i - 1, j) || playfield->isCorridor(i - 1, j))
                {
                    if (distMonster[i - 1][j] > longestDist)
                    {
                        longestDist = distMonster[i - 1][j];
                        nextMove = new Site(i - 1, j);
                    }
                }

                if (playfield->isRoom(i, j + 1) || playfield->isCorridor(i, j + 1))
                {
                    if (distMonster[i][j + 1] > longestDist)
                    {
                        longestDist = distMonster[i][j + 1];
                        nextMove = new Site(i, j + 1);
                    }
                }

                if (playfield->isRoom(i, j - 1) || playfield->isCorridor(i, j - 1))
                {
                    if (distMonster[i][j - 1] > longestDist)
                    {
                        longestDist = distMonster[i][j - 1];
                        nextMove = new Site(i, j - 1);
                    }
                }
            }

            else
            {
                // The player is standing on a corridor site that does not have
                // an adjacent room next to it.
                nextMove = chooseNextCorridor(cycleBetweenRooms, distPlayer, distMonster, player);
            }

        }
    }



    return calculateFinalDestination(nextMove, distPlayer, prevPlayer, player);
}

bool Player::isPlayerInConnectedRoomCycle(map<Site*, vector<Site*>> cycleBetweenRooms, const Site* player)
{
    map<Site*, vector<Site*>>::iterator it;
    vector<Site*>::iterator vectIt;
    unsigned int i;
    unsigned int j;

    it = cycleBetweenRooms.begin();
    while (it != cycleBetweenRooms.end())
    {
        vectIt = it->second.begin();
        while (vectIt != it->second.end())
        {
            i = (*vectIt)->i();
            j = (*vectIt)->j();

            if (i == player->i() && j == player->j())
                return true;

            vectIt++;
        }

        it++;
    }



    return false;

}

Site* Player::chooseNextCorridor(map<Site*, vector<Site*>> cycleBetweenRooms, int **&distPlayer,  int **&distMonster, const Site* player)
{

    vector<Site*> corridors;
    vector<Site*>::iterator vectIt;
    map<Site*, vector<Site*>>::iterator it;
    map<Site*, vector<Site*>> adjList;
    static vector<Site*> move;
    multimap <int, Site*> decisions;
    multimap<int, Site*>::iterator decIt;
    multimap<int, Site*>::iterator erase;
    Site* nextMove = nullptr;
    unsigned int i = player->i();
    unsigned int j = player->j();

    nextMove = new Site(i, j);


    // Find the adjacency list of the corridor site the player
    // is currently standing on. This also gives the connected
    // component the player is standing on.
    it = cycleBetweenRooms.begin();
    while (it != cycleBetweenRooms.end())
    {
  
        //Make sure the representative of the cycle is included in the list of
        // vertices.
        it->second.push_back(it->first);
        if (i == it->first->i() && j == it->first->j())
            adjList = findAdjLists(it->second);

        vectIt = it->second.begin();
        while (vectIt != it->second.end())
        {
          
            if (i == (*vectIt)->i() && j == (*vectIt)->j())
                adjList = findAdjLists(it->second);

            vectIt++;
        }

        it++;
    }

    // For each vertex in the adjacency list, determine if it has a room next to it
    // (a way out for the player). Make a list of these as decisions storing distance
    // from the monster to the site as well.
    it = adjList.begin();
    while (it != adjList.end())
    {

        if (hasAdjacentRoom(it->first))
        {
            decisions.insert({ distMonster[it->first->i()][it->first->j()], it->first});
        }

        it++;
    }

    // Monster or player is on a corridor. Delete these corridors that the monster or
    // player has just passed through so that the A.I. doesn't make the mistake
    // of turning around and heading backwards when the monster is right behind it.
    if (!move.empty())
    {
        vectIt = move.begin();
        while (vectIt != move.end())
        {
            decIt = decisions.begin();
            while (decIt != decisions.end())
            {
                if ((*vectIt)->i() == decIt->second->i() && (*vectIt)->j() == decIt->second->j())
                {
                    erase = decIt;
                    ++decIt;
                    decisions.erase(erase);
                }

                else
                    decIt++;

            }

            vectIt++;
        }

    }

    // Look at all possible decisions and choose one that the player can reach
    // before the monster can reach the same site.
    decIt = decisions.begin();
    while (decIt != decisions.end())
    {
        if (distPlayer[decIt->second->i()][decIt->second->j()] < distMonster[decIt->second->i()][decIt->second->j()])
            nextMove = decIt->second;

        decIt++;
    }


    // If after all of this work has been done, the decision chosen might have
    // been the player's same location! So go back through the list of decisions,
    // and choose the one which is the largest distance away from the monster instead.
    if (nextMove->i() == player->i() && nextMove->j() == player->j())
    {
        if (!decisions.empty())
        {
            decIt = decisions.end();
            decIt--;
            nextMove = decIt->second;
        }

        else
        {
            nextMove = stayAliveAsLongAsPossible(distMonster, distPlayer);
        }
    }

    // If the monster can actually reach this particular site before the player can
    // then as a last resort the player can scan all adjacent corridor sites to the 		//player and choose the one which is the farthest away from the monster.
    if (distMonster[nextMove->i()][nextMove->j()] < distPlayer[nextMove->i()][nextMove->j()])
    {
        nextMove = scanAdjacentCorridorSites(player, distMonster);
    }

    return nextMove;
}

Site* Player::scanAdjacentCorridorSites(const Site* player, int **&distMonster)
{
    Site* nextMove = nullptr;
    int longestDist;
    int i;
    int j;


    i = player->i();
    j = player->j();

    

    longestDist = distMonster[i][j];

    if (playfield->isCorridor(i + 1, j))
    {
        if (distMonster[i + 1][j] > longestDist)
        {
            nextMove = new Site(i + 1, j);
            longestDist = distMonster[i + 1][j];
        }
    }

    if (playfield->isCorridor(i - 1, j))
    {
        if (distMonster[i - 1][j] > longestDist)
        {
            nextMove = new Site(i - 1, j);
            longestDist = distMonster[i - 1][j];
        }
    }

    if (playfield->isCorridor(i, j + 1))
    {
        if (distMonster[i][j + 1] > longestDist)
        {
            nextMove = new Site(i, j + 1);
            longestDist = distMonster[i][j + 1];
        }
    }

    if (playfield->isCorridor(i, j - 1))
    {
        if (distMonster[i][j - 1] > longestDist)
        {
            nextMove = new Site(i, j - 1);
            longestDist = distMonster[i][j - 1];
        }
    }

   



    return nextMove;
}

vector<Site*> Player::getCycleChoices(map<Site*, vector<Site*>> cycleBetweenRooms, const Site* player )
{

    map<Site*, vector<Site*>>::iterator it;
    vector<Site*>::iterator vectIt;
    vector<Site*>::iterator trav;
    vector<Site*>::iterator erase;
    vector<Site*> corridors;
    int i;
    int j;
    bool exist;
    Site* site = new Site(player->i(), player->j());

    i = player->i();
    j = player->j();


    // Set i and j to bottom right of room in order to search
    // for adjacent corridor sites to the room.
    while (playfield->isRoom(i, j))
        j++;

    j--;

    while (playfield->isRoom(i, j))
        i++;

    i--;

    // Now search the room.
    searchAllAdjCorrSites(corridors, site, i, j);
    checkDuplicates(corridors, site);

  


    // Delete any corridors that are not in the cycle between rooms and corridors.
    vectIt = corridors.begin();
    while (vectIt != corridors.end())
    {
        it = cycleBetweenRooms.begin();
        exist = false;
        while (it != cycleBetweenRooms.end())
        {
            // If the list of decisions contains a corridor in the cycle set exist to true.
            if (it->first->i() == (*vectIt)->i() && it->first->j() == (*vectIt)->j())
                exist = true;
            trav = it->second.begin();
            while (trav != it->second.end())
            {
                if ((*trav)->i() == (*vectIt)->i() && (*trav)->j() == (*vectIt)->j())
                    exist = true;

                trav++;
            }

            it++;

        }

        // Delete the corridor sites which are not a part of the cycle.
        if (!exist)
        {
            vectIt = corridors.erase(vectIt);
        }
        else
            vectIt++;

    }

    
    vectIt = corridors.begin();
    while (vectIt != corridors.end())
    {
      
        vectIt++;
    }
    return corridors;


}


void Player::searchAllAdjCorrSites(vector<Site*> &vectDisc, Site* site, int &i, int &j)
{

    Site* newSite;


    // Search a room starting from the bottom right looking
    // for adjacent corridor sites.

    // Search from bottom right to bottom left.
    while (playfield->isRoom(i, j))
    {
        if (playfield->isCorridor(i + 1, j))
        {

            newSite = new Site(i + 1, j);
            vectDisc.push_back(newSite);

        }

        j--;
    }

    // Search from bottom left to to top left.

    j++;
    while (playfield->isRoom(i, j))
    {
        if (playfield->isCorridor(i, j - 1))
        {
            newSite = new Site(i , j - 1);
            vectDisc.push_back(newSite);

        }

        i--;
    }

    i++;

    // Search from top left to top right.
    while (playfield->isRoom(i, j))
    {
        if (playfield->isCorridor(i - 1, j))
        {

            newSite = new Site(i - 1, j);
            vectDisc.push_back(newSite);

        }

        j++;
    }

    // Search from top right to bottom right.

    j--;
    while (playfield->isRoom(i, j))
    {
        if (playfield->isCorridor(i, j + 1))
        {

            newSite = new Site(i, j + 1);
            vectDisc.push_back(newSite);

        }

        i++;
    }

    i--;

}

map<Site*,vector<Site*>> Player::checkForConnectedDeadEnds(map<Site*, vector<Site*>> adjConn)
{
    map<Site*, vector<Site*>>::iterator it;
    map<Site*, vector<Site*>>::iterator eraseMap;
    vector<Site*>::iterator vectIt;
    int counter;

    // Check each connected component to make sure they lead back to a room.
    // If they don't have at least 2 room connections, it must be a dead end.
    // Therefore, remove it.
    it = adjConn.begin();
    while (it != adjConn.end())
    {
        counter = 0;
        if (hasAdjacentRoom(it->first))
            counter++;
        vectIt = it->second.begin();
        while (vectIt != it->second.end())
        {
            if (hasAdjacentRoom(*vectIt))
                counter++;

            vectIt++;
        }

        if (counter < 2)
        {
            eraseMap = it;
            ++it;
            adjConn.erase(eraseMap);
        }
        else
            it++;

    }

    return adjConn;

}

bool Player::hasAdjacentRoom(Site* site)
{
    int i = site->i();
    int j = site->j();

    if (playfield->isRoom(i + 1, j))
        return true;

    if (playfield->isRoom(i - 1, j))
        return true;

    if (playfield->isRoom(i, j - 1))
        return true;

    if (playfield->isRoom(i, j + 1))
        return true;

    return false;

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

    // Get a 2D array of bools all marked as false.
    getMarkedArray(marked);

    // Perform a depth first search on all of the adjacent sites.
    // This gives the connected components.

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
        // Insert each connected component with along with a vertex
        // in that component.
        adjConn.insert({it->first, connectedComp});
        connectedComp.clear();

        it++;
    }


    // Delete any components that don't have any adjacent vertices.
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
    	
    	
    	vectIt = it->second.begin();
    	while (vectIt != it->second.end())
    	{
    		
    		vectIt++;
    	}
    	
    	it++;
    }
    */

    freeMarkedArray(marked);
    return adjConn;

}

void Player::freeMarkedArray(bool **&marked)
{

    int i;

    for (i = 0; i < N; i++)
        delete[] marked[i];

    delete[] marked;
}



map<Site*, vector<Site*>> Player::getCyclesWithinCorridors(map<Site*, vector<Site*>> adjConn)
{
    map<Site*, vector<Site*>> cycle;
    map<Site*, vector<Site*>> adjList;
    map<Site*, vector<Site*>>::iterator adjIt;
    map<Site*, vector<Site*>>::iterator it;
    vector<Site*>::iterator vectIt;
    vector<Site*>::iterator prev;
    vector<Site*>::iterator trav;
    vector<Site*> store;
    vector<Site*> corridors;
    int empty;
    int connectedComponents;


    // Starting at each connected component, since each representative element
    // of that connected component is NOT already in the component list, simply
    // insert it into the list.
    it = adjConn.begin();
    while (it != adjConn.end())
    {
        it->second.push_back(it->first);
        it++;
    }


    // Remove any and all vertices that are not a part of the cycle. I.e. remove
    // any verties that only have 1 or 0 adjacent corridor sites in the connected
    // component.
    it = adjConn.begin();
    while (it != adjConn.end())
    {
        it->second = removeSitesWithOneAdjacentSite(it->second);
        it++;
    }

    cycle = adjConn;

    it = cycle.begin();
    // Empty and connectedComponents is used to determine the number of
    // empty connected components. If all of them are empty, clear
    // the cycle.
    empty = 0;
    connectedComponents = 0;
    while (it != cycle.end())
    {
        
        
        vectIt = it->second.begin();
        connectedComponents++;
        if (it->second.empty())
            empty++;
        while (vectIt != it->second.end())
        {
           
            vectIt++;
        }
        
        it++;
    }

    if (empty == connectedComponents)
        cycle.clear();

    return cycle;

}


vector<Site*> Player::removeSitesWithOneAdjacentSite(vector<Site*> myvector)
{
    map<Site*, vector<Site*>>::iterator it;
    map<Site*, vector<Site*>> adjList;
    map<Site*, vector<Site*>>::iterator adjIt;
    vector<Site*>::iterator vectIt;
    vector<Site*>::iterator erase;
    bool moreToDelete = false;
    int counter;
    bool enteredIf;

    do
    {
        // Get adjacency list
        adjList = findAdjLists(myvector);
        adjIt = adjList.begin();
        while (adjIt != adjList.end())
        {
            counter = 0;
            enteredIf = false;
            // Delete any vertices that have less than 2 adjacent
            // corridor sites.
            if (adjIt->second.size() < 2)
            {
                vectIt = myvector.begin();
                while (vectIt != myvector.end() && ((*vectIt)->i() != adjIt->first->i() || (*vectIt)->j() != adjIt->first->j()))
                {
                    vectIt++;
                }

                
                erase = vectIt;
                ++vectIt;
                myvector.erase(erase);
                counter++;
                moreToDelete = true;
                // adj list has changed because of deletion, so call it
                // again.
                adjList = findAdjLists(myvector);
                adjIt = adjList.begin();

                enteredIf = true;

                if (myvector.size() == 0)
                    return myvector;

            }
            // If no nodes are found, then no more to delete.
            if (counter == 0)
                moreToDelete = false;

            if (!enteredIf)
                adjIt++;

        }
    } while (moreToDelete);

    vectIt = myvector.begin();
    while (vectIt != myvector.end())
    {
        
        vectIt++;
    }

    return myvector;

}

void Player::setCoordinates(int &i, int &j)
{
    if (playfield->isRoom(i, j))
    {
        while (playfield->isRoom(i, j))
            j++;

        j--;

        while (playfield->isRoom(i, j))
            i++;

        i--;
    }

}

Site* Player::getAdjacentRoomSite(Site* site)
{
    int i;
    int j;
    Site* room = nullptr;

    i = site->i();
    j = site->j();

    if (playfield->isRoom(i + 1, j))
    {
        room = new Site(i + 1, j);
        return room;
    }

    if (playfield->isRoom(i - 1, j))
    {
        room = new Site(i - 1, j);
        return room;
    }

    if (playfield->isRoom(i, j + 1))
    {
        room = new Site(i, j + 1);
        return room;
    }

    if (playfield->isRoom(i, j - 1))
    {
        room = new Site(i, j - 1);
        return room;
    }

    return nullptr;

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

Site* Player::calculateNextRoom(map<Site*, vector<Site*>> &adjDisc, int **&distMonster, int**&distPlayer, Site* **&prevPlayer, const Site* player, const Site* monster, map<Site*, vector<Site*>> connectedCycle)
{
    int shortestDist;
    int longestDist;
    map<Site*, vector<Site*>>::iterator it;
    map<Site*, vector<Site*>>::iterator eraseMap;
    map<Site*, vector<Site*>>::iterator mapIt;
    static map<Site*, vector<Site*>> connectedComponents;
    vector<Site*>::iterator vectIt;
    vector<Site*>::iterator eraseVect;
    Site* nearestCorr;
    static vector<Site*> cycle;
    vector<Site*> corridors;
    unsigned int i;
    unsigned int j;



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


    it = connectedCycle.begin();
    while (it != connectedCycle.end())
    {
        vectIt = it->second.begin();
        while (vectIt != it->second.end())
        {
            i = (*vectIt)->i();
            j = (*vectIt)->j();

            if (distPlayer[i][j] <= distMonster[i][j])
                return nullptr;

            vectIt++;
        }

        it++;
    }

    if (connectedComponents.empty())
    {
        findCorridors(corridors);
        connectedComponents = findAdjLists(corridors);
        connectedComponents = findConnectedComponents(connectedComponents);
        connectedComponents = checkForConnectedDeadEnds(connectedComponents);

    }

    it = connectedComponents.begin();
    while (it != connectedComponents.end())
    {
        vectIt = it->second.begin();
        while (vectIt != it->second.end())
        {
            i = (*vectIt)->i();
            j = (*vectIt)->j();

            if (distPlayer[i][j] < distMonster[i][j])
                return nullptr;

            vectIt++;
        }

        it++;
    }



    removeDeadEndVertices(adjDisc);

    if (adjDisc.size() == 0)
    {
        
        return nullptr;
    }


    // If the monster is right behind the player, simply find a corridor site
    // in the current room with the highest number of adjacent corridor sites
    // to that particular corridor site.
    if (distMonster[player->i()][player->j()] < 5 && N > 20 && !cycle.empty())
    {
        run(player, nearestCorr, distPlayer, distMonster, cycle, adjDisc);
        if (nearestCorr == nullptr)
            return nullptr;
    }

    else if (distMonster[player->i()][player->j()] < 3 && N <= 20 && !cycle.empty())
    {
        run(player, nearestCorr, distPlayer, distMonster, cycle, adjDisc);
        if (nearestCorr == nullptr)
            return nullptr;
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
    return calculateFinalDestination(nearestCorr, distPlayer, prevPlayer, player);
}

void Player::run(const Site* player, Site* &nearestCorr, int **&distPlayer, int **&distMonster, vector<Site*> &cycle, map<Site*, vector<Site*>> &adjDisc)
{
    int i = player->i();
    int j = player->j();
    unsigned int a;
    unsigned int b;
    unsigned int initialI;
    unsigned int initialJ;
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

        if (vertices.size() != 0)
        {
            myVertices = vertices.begin();

            while (myVertices != vertices.end())
            {
               
                myVertices++;
            }

            myVertices--;
            nearestCorr = myVertices->second;

        }
        else
            nearestCorr = nullptr;

    }

    // If the player is on a corridor site, then enter the next room by looking at which
    // room site has a higher distance away from the monster.
    else
    {

        initialI = nearestCorr->i();
        initialJ = nearestCorr->j();

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

        if (nearestCorr->i() == player->i() && nearestCorr->j() == player->j())
            nearestCorr = nullptr;

        if (nearestCorr != nullptr)
            if (initialI == nearestCorr->i() && initialJ == nearestCorr->j())
                nearestCorr = nullptr;


    }

}

void Player::removeDeadEndVertices(map<Site*, vector<Site*>> &adjDisc)
{
    map<Site*, vector<Site*>>::iterator it;
    map<Site*, vector<Site*>>::iterator cycleIt;
    map<Site*, vector<Site*>>::iterator compIt;
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

                vectIt--;

                if (vectIt != it->second.end())
                    vectIt++;

                if (it->second.size() == 1 && vectIt != it->second.end() && vectIt != it->second.begin())
                    vectIt = it->second.begin();
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

Site* Player::findCorridorCycle(map<Site*, vector<Site*>> connectedCycle, map<Site*, vector<Site*>> adjConn, int **&distMonster, int **&distPlayer, Site* **&prevPlayer, const Site* player, const Site* monster)
{
    map<Site*, vector<Site*>>::iterator it;
    vector<Site*>::iterator vectIt;
    Site* nextMove = nullptr;
    map<Site*, vector<Site*>> adjList;
    bool inCycle;
    unsigned int i;
    unsigned int j;
    int longestDist;
    int shortestDist;
    multimap<int, Site*> decisions;
    multimap<int, Site*>::iterator decIt;
    static map<Site*, vector<Site*>> connectedComponents;
    vector<Site*> corridors;
    bool roomCycle = false;
    bool corridorCycle = false;


    if (connectedComponents.empty())
    {
        findCorridors(corridors);
        connectedComponents = findAdjLists(corridors);
        connectedComponents = findConnectedComponents(connectedComponents);
        connectedComponents = checkForConnectedDeadEnds(connectedComponents);

    }

    it = connectedComponents.begin();
    while (it != connectedComponents.end())
    {
        vectIt = it->second.begin();
        while (vectIt != it->second.end())
        {
            i = (*vectIt)->i();
            j = (*vectIt)->j();

            if (distPlayer[i][j] < distMonster[i][j])
                roomCycle = true;

            vectIt++;
        }

        it++;
    }

    it = connectedCycle.begin();
    while (it != connectedCycle.end())
    {
        vectIt = it->second.begin();
        while (vectIt != it->second.end())
        {
            i = (*vectIt)->i();
            j = (*vectIt)->j();

            if (distPlayer[i][j] <= distMonster[i][j])
                corridorCycle = true;

            vectIt++;
        }

        it++;
    }

    if (roomCycle && !corridorCycle)
        return nullptr;



    // Determine if the player is on a site in the cycle.
    inCycle = isPlayerInCorridorCycle(connectedCycle, player);

    

    if (!inCycle)
    {



        // If only one cycle, find shortest path to that cycle.
        if (connectedCycle.size() == 1)
        {

            it = connectedCycle.begin();

            while (it != connectedCycle.end())
            {
                vectIt = it->second.begin();

                while (vectIt != it->second.end())
                {
                    i = (*vectIt)->i();
                    j = (*vectIt)->j();

                    if (distPlayer[i][j] < distMonster[i][j])
                        decisions.insert({distMonster[i][j], *vectIt});

                    vectIt++;
                }
                it++;
            }

            if (!decisions.empty())
            {
                decIt = decisions.end();
                decIt--;
                nextMove = decIt->second;

            }

            else if (decisions.empty())
            {

                it = adjConn.begin();
                while (it != adjConn.end())
                {
                    vectIt = it->second.begin();
                    while (vectIt != it->second.end())
                    {
                        i = (*vectIt)->i();
                        j = (*vectIt)->j();

                        if (distPlayer[i][j] < distMonster[i][j])
                            return nullptr;

                        vectIt++;

                    }

                    it++;
                }
            }

            else
            {
                it = connectedCycle.begin();
                while (it != connectedCycle.end())
                {
                    vectIt = it->second.begin();
                    shortestDist = distPlayer[(*vectIt)->i()][(*vectIt)->j()];
                    nextMove = *vectIt;
                    while (vectIt != it->second.end())
                    {
                        if (distPlayer[(*vectIt)->i()][(*vectIt)->j()] < shortestDist)
                        {
                            shortestDist = distPlayer[(*vectIt)->i()][(*vectIt)->j()];
                            nextMove = *vectIt;
                        }

                        vectIt++;
                    }

                    it++;
                }

            }

        }

        else
        {
            // There's more than one cycle, so choose the cycle that the player
            // can reach by seeing if the cycle can be reached in less moves
            // than the monster can reach it with.




            it = connectedCycle.begin();

            while (it != connectedCycle.end())
            {
                vectIt = it->second.begin();

                while (vectIt != it->second.end())
                {
                    i = (*vectIt)->i();
                    j = (*vectIt)->j();

                    if (distPlayer[i][j] < distMonster[i][j])
                        decisions.insert({distMonster[i][j], *vectIt});

                    vectIt++;
                }
                it++;
            }

            if (!decisions.empty())
            {
                decIt = decisions.end();
                decIt--;
                nextMove = decIt->second;

            }
            else if (decisions.empty())
            {
                it = adjConn.begin();
                while (it != adjConn.end())
                {
                    vectIt = it->second.begin();
                    while (vectIt != it->second.end())
                    {
                        i = (*vectIt)->i();
                        j = (*vectIt)->j();

                        if (distPlayer[i][j] < distMonster[i][j])
                            return nullptr;

                        vectIt++;

                    }

                    it++;
                }
            }


            else
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

                if (nextMove == nullptr)
                    return nullptr;

            }

        }

    }

    else
    {
        i = player->i();
        j = player->j();

        // If player is on a site in the cycle, then
        // first get list of adjacent vertices in cycle.
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

        // Now choose a site in the cycle that's adjacent to the player
        // and that's the farthest distance away from the monster.
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

    return calculateFinalDestination(nextMove, distPlayer, prevPlayer, player);

}

bool Player::isPlayerInCorridorCycle(map<Site*, vector<Site*>> connectedCycle, const Site* player)
{
    map<Site*, vector<Site*>>::iterator it;
    vector<Site*>::iterator vectIt;
    unsigned int i;
    unsigned int j;

    i = player->i();
    j = player->j();

    // If the player is on a site which is apart of a cycle
    // then return true.
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

