
/*************************************************************************//**
 * @file player.cpp
 *
 * @brief A program which contains a monster and player. Monster's goal is to catch the player.
 * The player's goal to is to avoid the monster.
 *
 * @mainpage Monster Dash
 *
 * @section course_section Course Information
 *
 * @author Cameron Javaheri
 *
 * @date due 12/12/2016
 *
 * @par Professor:
 *		Paul Hinker
 *
 * @par Course:
 *      CSC 300 - Fall 2016 - M002 - 2pm
 *
 * @par Location:
		McLaury - 310
 *
 *
 * @details This program has 2 components to it. There is both a monster and a player.
 * The monster's goal is simply to catch the player. The player's goal is to identify
 * any cycles in the underlying graph so that the player can simply run around in circles
 * forever, thereby evading the monster indefinitely. The game is over once the player is
 * either caught, which in that case the monster wins, or the player survives a specified
 * number of moves, in which case the player wins.
 *
 *  @section compile_section Compiling and Usage
 *
 * @par Compiling Instructions:
 *      None
 *
 * @par Usage:
   @verbatim
   % make
   % main Playfields/A.in
   @endverbatim
 *
 * @section todo_bugs_modification_section Todo, Bugs, and Modifications
 *
 * @bug none
 *
 * @todo none
 *
 * @par Modifications and Development Timeline:
   @verbatim
   Gitlab Repository: https://gitlab.mcs.sdsmt.edu/7346909/csc300_fall2016_project3.git
   @endverbatim
 *
 *
 ****************************************************************************/



#include "player.h"

using namespace std;

/***************************************************************************//**
 * @author Cameron Javaheri
 *
 *
 * @par Description
 *  This function is a constructor for a player object. It initializes the
 * playfield as well as the size of the playfield.
 *
 * @param[in] p - The playfield.
 *
 ******************************************************************************/

Player::Player(Playfield* p)
{
    playfield = p;
    N = playfield->size();
}

/***************************************************************************//**
 * @author Cameron Javaheri
 *
 *
 * @par Description
 *  This function returns the next move the player wants to make. The move must
 * be a legal move according to the rules of the game. The player can only move
 * one site at a time to an adjacent site. The directions are dependent on if
 * the site is a corridor or a room.
 *
 *
 * @return The site that the player wants to move to next.
 ******************************************************************************/

const Site* Player::move()
{

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


/***************************************************************************//**
 * @author Cameron Javaheri
 * @brief This function takes in a vector of rooms and returns the adjacency list
 * of the rooms.
 *
 * @par Description
 *   findAdjRoomLists finds all of the adjacent rooms to each vertex, then stores each vertex
 * in a map along with their adjacency list as a vector.
 *
 * @param[in] rooms - All of the rooms which you want to find the adjacency list for.
 * They must all be stored in a vector.
 *
 * @return A map containing the adjacency list for all of the room sites.
 ******************************************************************************/

map<Site*, vector<Site*>> Player::findAdjRoomLists(vector<Site*> rooms)
{
    vector<Site*>::iterator it;
    vector<Site*>::iterator curr;
    vector<Site*> adjacentVertices;
    Site* currSite;
    Site* compareSite;
    map<Site*, vector<Site*>> adj;

    curr = rooms.begin();
    // For each room, check all of the room sites to see which ones are adjacent.
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

/***************************************************************************//**
 * @author Cameron Javaheri
 * @brief This function takes in a EMPTY vector and stores every site on the playfield
 * into that vector.
 *
 * @par Description
 *   Stores all corridors on the playfield into a vector.
 *
 * @param[in, out] corridors - An EMPTY vector used to store the corridor sites.
 *
 * @return A vector containing every corridor site on the playfield.
 ******************************************************************************/

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


/***************************************************************************//**
 * @author Cameron Javaheri
 * @brief Calculates the next move the player should take.
 *
 * @par Description
 *   This function is similar to the "main" function. It does all of the heavy lifting
 * for the player by calling all of the functions necessary to analyze the entire
 * playfield. Once analyzed, the player can make the optimal move based on the data.
 *
 *
 * @param[in, out] markedMonster - A 2D array of bools. They are marked true if
 * the monster can reach the (i, j) position on the map, and false otherwise.
 * @param[in, out] prevMonster - An array containing site pointers from a
 * a breadth first search performed from the monster's site. They are the
 * exact locations the monster can move to.
 * @param[in, out] distMonster - A 2D array containing the shortest distances from the
 * monster's position to any reachable site on the map.
 * @param[in, out] prevPlayer - The locations obtained from performing a breadth
 * first search on the player.
 * @param[in, out] markedPlayer - Same as markedMonster, except they are marked
 * true if the player can reach the (i, j) position and false otherwise.
 * @param[in, out] distPlayer - The shortest path distances to each site
 * from the player.
 * @param[in] monster - The site which the monster is currently on.
 * @param[in] player - The site which the player is currently on.
 * @param[in, out] allocatedMemory - Stores any allocated memory for cleanup.
 * @param[in, out] adj - Contains the adjacency list for each and every corridor
 * site on the playfield.
 *
 * @return A legal move (the player's next move).
 ******************************************************************************/


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
                // Attempt to stay alive as long as possible.
                nextMove = stayAliveAsLongAsPossible(distMonster, distPlayer);
                monsterTooClose(distMonster, distPlayer, nextMove, player, monster);
                return calculateFinalDestination(nextMove, distPlayer, prevPlayer, player);
            }

            else
            {
                return getRoomCycle(distMonster, distPlayer, prevPlayer, player);
            }


        }
    }

    disc = countDisconnectedComponents(adj);



    // Try to find a cycle.
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
            // Find cycle between rooms and disconnected components.
            adjDisc = getAdjListDisc(adj, allocatedMemory);
            nextMove =  calculateNextRoom(adjDisc, distMonster, distPlayer, prevPlayer, player, monster, connectedCycle);
            if (nextMove != nullptr)
                return nextMove;


            adj = temp;

        }
    }


    // Find cycle between only corridors.
    if (!connectedCycle.empty())
    {
        adjConn = checkForConnectedDeadEnds(adjConn);

        nextMove = findCorridorCycle(connectedCycle, adjConn, distMonster, distPlayer, prevPlayer, player, monster);

        if (nextMove != nullptr)
            return nextMove;
    }

    // Find cycles between rooms and connected components.
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


/***************************************************************************//**
 * @author Cameron Javaheri
 * @brief Finds the adjacency list given a vector of corridors.
 *
 * @par Description
 *   This function will find the adjacency list given a vector of corridors.
 *
 * @param[in] corridors - A vector containing corridors.
 *
 * @return A map containing each corridor and their adjacency list as a vector.
 ******************************************************************************/


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


/***************************************************************************//**
 * @author Cameron Javaheri
 * @brief This function makes a decides the player's next move given a cycle
 * of ONLY room sites.
 *
 * @par Description
 *   The function decides what the best path to take is based upon knowing
 * a cycle between room sites, the distance from the monster to that cycle,
 * and the distance from the player to that cycle. If the distance from the
 * the player to that cycle is smaller than the distance of the monster to
 * that cycle, it stores those decisions into a map so that they can be sorted.
 * It then chooses the location that is the farthest distance away from the
 * monster without getting caught. If it is not possible to reach a cycle
 * before the monster can, then the player simply calculates the shortest
 * path to the cycle, hoping that the monster is not intelligent enough
 * to catch the player.
 *
 * @param[in, out] distMonster - A 2D array containing the shortest distances
 * to every site from the monster.
 * @param[in, out] distPlayer - A 2D array containing the shortest distances to
 * every site from the player.
 * @param[in, out] prevPlayer - The list of locations to a specific site obtained
 * from breadth first search.
 * @param[in] roomCycle - A cycle of ONLY room sites. NO corridors.
 * @param[in] player - The site the player is currently on.
 *
 * @return A site containing the player's next move.
 ******************************************************************************/

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


    return nextMove;

}


/***************************************************************************//**
 * @author Cameron Javaheri
 * @brief This function calculates a cycle between ONLY room sites.
 *
 * @par Description
 *   This function calculates a cycle between strictly room sites. To do this,
 * it first stores all of the rooms in the playfield into a vector. It then removes
 * any room sites which have an adjacent diagonal room site. It then finds the adjacency
 * list for these room sites. Then it's able to determine all possible room cycles.
 * Once given all possible room cycles, it chooses the cycle which has the largest
 * number of vertices in the cycle, AND the player can reach before the monster.
 * It finally removes dead ends before sending it off to another function to
 * determine the player's actual next move.
 *
 * @param[in, out] distMonster - The shortest distance from the monster to any site
 * on the playfield.
 * @param[in, out] distPlayer - The shortest distance from the player to any site
 * on the playfield.
 * @param[in, out] prevPlayer - A 2D array containing site pointers which were
 * obtained from breadth first search. Used to calculate final destination.
 * @param[in] player - The site the player is currently on.
 *
 * @return The player's next move.
 ******************************************************************************/


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


    // If cycle is empty, find a cycle between only room sites.
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

        checkWalls(roomCycle);

    }

    return chooseNextRoom(distMonster, distPlayer, prevPlayer, roomCycle, player);

}


/***************************************************************************//**
 * @author Cameron Javaheri
 * @brief Deletes unnecessary moves from a player's list of decisions.
 *
 * @par Description
 *   The function looks at all of the walls on the playfield, and then decides
 *	which walls have 8 adjacent room sites. It then deletes these sites
 *	from the player's list of decisions.
 *
 * @param[in, out] roomCycle - A cycle of all of the room sites.
 *
 *
 ******************************************************************************/

void Player::checkWalls(vector<Site*> &roomCycle)
{
    unsigned int i;
    unsigned int j;
    int a;
    int b;
    int counter;
    vector<Site*> walls;
    vector<Site*>::iterator vectIt;
    vector<Site*>::iterator roomIt;
    vector<Site*>::iterator erase;
    Site* site;

    for (a = 0; a < N; a++)
        for (b = 0; b < N; b++)
            if (playfield->isWall(a, b))
            {
                site = new Site(a, b);
                walls.push_back(site);
            }


    vectIt = walls.begin();
    while (vectIt != walls.end())
    {

        counter = 0;
        roomIt = roomCycle.begin();
        while (roomIt != roomCycle.end())
        {
            i = (*vectIt)->i();
            j = (*vectIt)->j();


            // Check all 8 adjacent sites.
            if (i + 1 == (*roomIt)->i() && j + 1 == (*roomIt)->j())
                counter++;


            if (i + 1 == (*roomIt)->i() && j  == (*roomIt)->j())
                counter++;


            if (i + 1 == (*roomIt)->i() && j - 1 == (*roomIt)->j())
                counter++;


            if (i == (*roomIt)->i() && j + 1 == (*roomIt)->j())
                counter++;


            if (i == (*roomIt)->i() && j - 1 == (*roomIt)->j())
                counter++;


            if (i - 1 == (*roomIt)->i() && j + 1 == (*roomIt)->j())
                counter++;


            if (i - 1 == (*roomIt)->i() && j == (*roomIt)->j())
                counter++;

            if (i - 1 == (*roomIt)->i() && j - 1 == (*roomIt)->j())
                counter++;



            roomIt++;
        }

        if (counter >= 8)
        {
            // Delete the vertical and horizontal sites.
            roomIt = roomCycle.begin();
            while (roomIt != roomCycle.end())
            {
                if (i + 1 == (*roomIt)->i() && j + 1 == (*roomIt)->j())
                {
                    erase = roomIt;
                    ++roomIt;
                    roomCycle.erase(erase);
                }

                else if (i + 1 == (*roomIt)->i() && j - 1 == (*roomIt)->j())
                {
                    erase = roomIt;
                    ++roomIt;
                    roomCycle.erase(erase);
                }


                else if (i - 1 == (*roomIt)->i() && j + 1 == (*roomIt)->j())
                {
                    erase = roomIt;
                    ++roomIt;
                    roomCycle.erase(erase);
                }


                else if (i - 1 == (*roomIt)->i() && j - 1 == (*roomIt)->j())
                {
                    erase = roomIt;
                    ++roomIt;
                    roomCycle.erase(erase);
                }

                else
                    roomIt++;
            }
        }

        vectIt++;
    }


}

/***************************************************************************//**
 * @author Cameron Javaheri
 * @brief Finds largest cycle between ONLY room sites.
 *
 * @par Description
 *   This function looks at all possible cycles containing only room sites. It chooses
 * the one which has the largest number of vertices AND the player can reach before
 * the monster catches it. If this is not possible, it keeps looking at other cycles
 * until it gets to the smallest cycle, in which case it just returns.
 *
 * @param[in] vectorSizes - A map containing the size of a cycle along with the
 * cycle itself. Must be a multimap because some cycles may have the same number
 * of duplicates.
 * @param[in, out] distPlayer - The shortest distances from the player to any
 * location on the playfield.
 * @param[in, out] distMonster - The shortes distances from the monster to any location
 * on the playfield.
 *
 * @return A vector containing a cycle between ONLY room sites.
 ******************************************************************************/

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

/***************************************************************************//**
 * @author Cameron Javaheri
 * @brief Finds all possible cycles between only room sites.
 *
 * @par Description
 *   Looks at each connected component, in order to determine any and all possible
 * cycles between only room sites.
 *
 * @param[in] adjConn - Contains all of the connected components composed
 * ONLY of room sites.
 *
 * @return All possible cycles between ONLY room sites.
 ******************************************************************************/

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


/***************************************************************************//**
 * @author Cameron Javaheri
 * @brief Removes all room sites that are diagonal.
 *
 * @par Description
 *  	Removes all room sites that are diagonal.
 *
 * @param[in] roomCycle - A vector containing room sites.
 *
 * @return A vector containing room sites that don't have room sites diagonal
 * to them.
 ******************************************************************************/


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


/***************************************************************************//**
 * @author Cameron Javaheri
 * @brief Determine if a site is adjacent to a wall on the perimeter of the playfield.
 *
 * @par Description
 *   Checks the entire perimeter of the playfield to see if the site in quesion is
 * adjacent to any of these walls on the perimeter of the playfield if they exist.
 *
 * @param[in] site - A site to determine if it's adjacent to any walls on the perimeter.
 *
 * @return true if the site is adjacent to any perimeter walls.
 * @return false if the site is not adjacent to any perimeter walls.
 ******************************************************************************/

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

/***************************************************************************//**
 * @author Cameron Javaheri
 * @brief Finds shortest path to a site on the playfield.
 *
 * @par Description
 *   An extremely useful function used to find the shortest path to the site nextMove.
 *
 * @param[in] nextMove - Any site on the playfield which the player wants to move to.
 * @param[in, out] distPlayer - The shortest distance from the player to any site
 * on the playfield.
 * @param[in, out] prevPlayer - The locations containing the shortest path to any
 * site on the playfield.
 * @param[in] player - The site which the player is currently located on.
 *
 * @return An adjacent square to the player, but is located along the shortest path
 * to the nextMove site.
 ******************************************************************************/

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


/***************************************************************************//**
 * @author Cameron Javaheri
 * @brief Used to run away from the monster as a last resort.
 *
 * @par Description
 *   Looks at all of the possible sites the player can reach before the monster can.
 *   Stores these, then takes the path to the site which is the farthest away from
 *   the monster.
 *
 * @param[in, out] distMonster - Contains all of the shortest distances from the
 * monster to any site on the playfield.
 * @param[in, out] distPlayer - Contains all of the shortest distances from the
 * player to any site on the playfield.
 *
 * @return The player's next move.
 ******************************************************************************/

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

    if (!decisions.empty())
    {
        it = decisions.end();
        it--;
        nextMove = it->second;
    }

    return nextMove;

}

/***************************************************************************//**
 * @author Cameron Javaheri
 * @brief Finds cycles between connected components of corridors and rooms.
 *
 * @par Description
 *   This function finds a cycle between connected components of corridor sites
 * and rooms. Therefore, the player may traverse the connected component, go through a
 * room, traverse another connected component, go through another room, etc, etc until it's
 * traversing a cycle.
 *
 * @param[in] cycleBetweenRooms - Contains connected components of corridors.
 * @param[in] adj - The adjacency list for ALL corridors on the playfield.
 * @param[in, out] distMonster - The shortest distance from the monster
 * to any site on the playfield.
 * @param[in, out] distPlayer - The shortest distance from the player
 * to any site on the playfield.
 * @param[in, out] prevPlayer - The locations obtained from b.f.s. done on
 * the player.
 * @param[in] player - The site which the player is currently on.
 * @param[in] monster - The site which the monster is currently on.
 *
 * @return A vector containing every corridor site on the playfield.
 ******************************************************************************/

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


/***************************************************************************//**
 * @author Cameron Javaheri
 *
 * @brief Determines if player is in cycle between rooms and connected components.
 *
 * @par Description
 *  Determine whether or not the player is currently on a site which belongs
 * to a connected component that creates a cycle in the graph between rooms
 * and connected components.
 *
 * @param[in] cycleBetweenRooms - All of the connected components which create
 * cycles between themselves and rooms.
 * @param[in] player - The site which the player is currently standing on.
 *
 * @return true if the player is on one of the connected componoents in the cycle.
 * @return false if the player is not on of the connected components in the cycle.
 ******************************************************************************/

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

/***************************************************************************//**
 * @author Cameron Javaheri
 *
 *
 * @par Description
 *  This function tries to make the best decision it can when the player is
 * on a connected component. For example, it has to make sure it does not
 * turn around when the monster is chasing right behind it, and it has to make
 * sure that it chooses a path that has a way out (no dead ends). Therefore,
 * in summary, this function chooses the best path it can when the player is located
 * on a connected component.
 *
 * @param[in] cycleBetweenRooms - All of the connected components which create
 * cycles between themselves and rooms.
 * @param[in, out] distPlayer - The shortest distance from the player to any
 * site on the graph.
 * @param[in, out] distMonster - The shortest distance from the monster to any
 * site on the graph.
 * @param[in] player - The site the player is currently standing on.
 *
 * @return The next move the player should make.
 ******************************************************************************/

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


/***************************************************************************//**
 * @author Cameron Javaheri
 *
 *
 * @par Description
 *  This function sees which adjacent corridor site the player can reach
 * before the monster can.
 *
 * @param[in] player - The site the player is currently on.
 * @param[in, out] distMonster - The distance from the monster to any site.
 *
 * @return The adjacent corridor site which the monster can't reach.
 ******************************************************************************/



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


/***************************************************************************//**
 * @author Cameron Javaheri
 *
 *
 * @par Description
 *  This function searches the current room the player is in to get the adjacent
 * corridors to the room. It deletes all of the corridor sites that are not apart of
 * the cycle between connected components and rooms.
 *
 * @param[in] cycleBetweenRooms - All of the connected components which create
 * cycles between themselves and rooms.
 * @param[in] player - The current site the player is standing on.
 *
 *
 * @return A vector containing valid corridor sites to move to that are
 * adjacent to the room the player is located in.
 ******************************************************************************/

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


    return corridors;


}


/***************************************************************************//**
 * @author Cameron Javaheri
 *
 *
 * @par Description
 *  This function scans the room starting at the bottom right. It moves along
 * the perimeter of the room, looking for corridor sites that are adjacent to
 * the room itself.
 *
 * @param[in, out] vectDisc - A vector to store the the corridors adjacent
 * to the room.
 * @param[in] site - The site to start searching from.
 * @param[in, out] i - The ith coordinate to start searching from.
 * @param[in, out] j - The jth coordinate to start searching from.
 *
 ******************************************************************************/


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

/***************************************************************************//**
 * @author Cameron Javaheri
 *
 *
 * @par Description
 *  This function looks at connected components, and determines if a connected
 * component leads to a dead end. This is determined by simply looking to see
 * if there's both a way in and a way out. This means that there has to be at
 * least 2 adjacent room sites to the connected component. If not, it's a dead end.
 *
 * @param[in] adjConn - A map containing a representative, and the list of vertices
 * in the connected component.
 *
 * @return All of the valid connected components, i.e. no connected components
 * with dead ends.
 ******************************************************************************/

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

/***************************************************************************//**
 * @author Cameron Javaheri
 *
 *
 * @par Description
 *  This function deteremines if a particular site has a room in a direction N, S,
 * E, or W.
 *
 * @param[in] site - Any site that needs to be determined if rooms are adjacent to it.
 *
 * @return true if the site has a room site adjacent to it.
 * @return false if the site does not have a room site adjacent to it.
 ******************************************************************************/

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


/***************************************************************************//**
 * @author Cameron Javaheri
 *
 *
 * @par Description
 *  Performs a depth first search on the graph in order to find the connected
 * components of corridor sites.
 *
 * @param[in] adj - An adjacency list containing corridor sites.
 *
 * @return The connected components along with their representatives.
 ******************************************************************************/

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

    freeMarkedArray(marked);
    return adjConn;

}


/***************************************************************************//**
 * @author Cameron Javaheri
 *
 *
 * @par Description
 *  This function frees up the memory allocated for a 2D array consisting of
 * booleans used to mark whether a vertex has been visited or not.
 *
 * @param[in, out] marked - The 2D array that needs to be freed up.
 *
 ******************************************************************************/

void Player::freeMarkedArray(bool **&marked)
{

    int i;

    for (i = 0; i < N; i++)
        delete[] marked[i];

    delete[] marked;
}


/***************************************************************************//**
 * @author Cameron Javaheri
 *
 *
 * @par Description
 *  This function finds cycles between ONLY corridor sites. It looks at all of
 * the connected components, and removes any and all vertices which are not
 * a part of the cycle.
 *
 * @param[in] adjConn - All of the connected components which create
 * cycles between corridor sites.
 *
 * @return A map containing cycles between ONLY corridor sites.
 ******************************************************************************/

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


/***************************************************************************//**
 * @author Cameron Javaheri
 *
 *
 * @par Description
 *  This function looks at all sites that only have 1 adjacent site. It
 * continues to delete these sites from a particular cycle until the
 * cycle only contains vertices with 2 adjacent sites. In other words,
 * it is a technique for removing dead ends in cycles.
 *
 * @param[in] myvector - A cycle that has dead ends.
 *
 * @return A cycle without dead ends.
 ******************************************************************************/


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

/***************************************************************************//**
 * @author Cameron Javaheri
 *
 *
 * @par Description
 *  This is a helper function for the search function. All it does is
 * set i and j to the bottom right of the room in order to start searching.
 *
 * @param[in, out] i - The initial x-coordinate.
 * @param[in, out] j - The initial y-coordinate.
 *
 ******************************************************************************/




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


/***************************************************************************//**
 * @author Cameron Javaheri
 *
 *
 * @par Description
 *  This function simply looks for ANY room site that's adjacent to a particular
 * site. It returns this adjacent room site.
 *
 * @param[in] site - The site which the user needs to see if it has an adjacent
 * room site.
 *
 * @return The adjacent room site.
 ******************************************************************************/

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

/***************************************************************************//**
 * @author Cameron Javaheri
 *
 *
 * @par Description
 *  This function creates a 2D array of bools and initializes them all to false.
 *
 * @param[in, out] marked - The 2D array of bools. Must be sent in null.
 *
 ******************************************************************************/


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


/***************************************************************************//**
 * @author Cameron Javaheri
 *
 *
 * @par Description
 *  This function looks at a connected cycle first. If it can reach a connected
 * cycle before the monster can, it calculates a path to that cycle. If it can't
 * then it finds a cycle between disconnected corridor sites. It then runs
 * around rooms travelling from disconnected corridor site to disconnected
 * corridor site.
 *
 * @param[in, out] adjDisc - The adjacency list containing corridor sites that
 * are not connected components. They are simply corridor sites with no adjacent
 * corridor vertices, but their adjacency list is treated as one.
 * @param[in, out] distMonster - The distance from the monster to any site on
 * the playfield.
 * @param[in, out] distPlayer - The distance from the player to any site on
 * the playfield.
 * @param[in, out] prevPlayer - Locations obtained from breadth first search
 * for player.
 * @param[in] player - The site the player is on.
 * @param[in] monster - The site the monster is on.
 * @param[in] connectedCycle - All of the cycles containing ONLY corridor
 * sites in them.
 *
 * @return The player's next move.
 ******************************************************************************/

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


/***************************************************************************//**
 * @author Cameron Javaheri
 *
 *
 * @par Description
 *  This function is called when the monster is right behind the player The idea
 * is to make the player run away from the monster by plotting a safe path instead
 * of just running away blindly. It looks at all of the adjacent disconnected
 * corridor sites of a room, and plots a path to whichever one has the highest
 * number of adjacent vertices.
 *
 * @param[in] player - The site the player is on.
 * @param[in, out] nearestCorr - The nearest corridor site that is adjacent
 * to the current room which the player should plot a path to.
 * @param[in, out] distPlayer - Distance to any site from player.
 * @param[in, out] distMonster - Distance to any site from monster.
 * @param[in, out] cycle - A vector used to store previous monster/player
 * decisions so that the player does not suddenly turn around and run
 * into the monster.
 * @param[in, out] adjDisc - The adjacency list for disconnected corridor sites.
 *
 ******************************************************************************/

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


/***************************************************************************//**
 * @author Cameron Javaheri
 *
 *
 * @par Description
 *  This function is designed to remove any vertices that have 1 or less than 1
 * adjacent vertices. This function only applies to disconnected corridor sites
 * (corridor sites that don't have ANY adjacent corridor sites).
 *
 * @param[in, out] adjDisc - The adjacency list for disconnected corridor sites.
 *
 ******************************************************************************/

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

/***************************************************************************//**
 * @author Cameron Javaheri
 *
 *
 * @par Description
 *  This function gets the adjacency list for disconnected corridor sites(sites that
 * don't have any adjacent corridor sites.
 *
 * @param[in, out] adj - The adjacency list for ALL corridor sites.
 * @param[in, out] allocatedMemory - A vector to store any memory allocated.
 *
 * @return The adjacency list for disconnected corridor sites.
 ******************************************************************************/

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


/***************************************************************************//**
 * @author Cameron Javaheri
 *
 *
 * @par Description
 *  This function deletes the corridor site the player is standing on
 * from the list of decisions the player has to make.
 *
 * @param[in, out] vectDisc - A vector containing the disconnected corridor sites.
 * @param[in] site - The site to make sure is not included in the disconnected
 * corridor site list.
 ******************************************************************************/

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

/***************************************************************************//**
 * @author Cameron Javaheri
 *
 *
 * @par Description
 *  This function searches the perimeter of the room starting from the bottom
 * right. It works its way left, then up, then right, then back down.

 * @param[in, out] vectDisc - A vector of disconnected corridor sites.
 * @param [in] site - The site the player is on.
 * @param[in, out] i - The x-coordinate the player is currently standing on.
 * @param[in, out] j - The y-coordinate the player is currently standing on.
 *
 ******************************************************************************/

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


/***************************************************************************//**
 * @author Cameron Javaheri
 *
 *
 * @par Description
 *  This function removes disconnected corridor sites that only have 1 adjacent
 * site.
 *
 * @param[in, out] adj - The adjacency list for ALL corridors.
 *
 * @return The adjacency list without disconnected corridor sites that only have
 * 1 adjacent corridor site.
 ******************************************************************************/

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


/***************************************************************************//**
 * @author Cameron Javaheri
 *
 *
 * @par Description
 *  This function counts disconnected corridor sites (sites that don't have any
 * adjacent corridor sites.)
 *
 * @param[in, out] adj - The adjacency list for ALL corridor sites.
 *
 * @return The number of disconnected corridor sites.
 ******************************************************************************/

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

/***************************************************************************//**
 * @author Cameron Javaheri
 *
 *
 * @par Description
 *  This function determines if the playfield has walls or not.
 *
 *
 * @return true if there are walls.
 * @return false if there are no walls.
 ******************************************************************************/

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

/***************************************************************************//**
 * @author Cameron Javaheri
 *
 *
 * @par Description
 *  This function determines if the playfield has corridors in it or not.
 *
 *
 * @return true if the playfield has corridors.
 * @return false if there are no corridors.
 ******************************************************************************/

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


/***************************************************************************//**
 * @author Cameron Javaheri
 *
 *
 * @par Description
 *  This function determines a path to a cycle that contains ONLY corridor
 * sites in it. It first checks to see if there's a cycle between connected
 * components. If there is no cycle between connected components and rooms,
 * it first determines whether or not a player is already in the cycle
 * between only corridor sites. If it's not, then find a path to a cycle
 * that the player can reach before the monster can. If the player is
 * already in a cycle, then determine a site that's adjacent to the player
 * that's also in the cycle and that's the farthest distance away from
 * the monster.
 *
 * @param[in] connectedCycle - A map containing all of the cycles between
 * ONLY corridor sites.
 * @param[in] adjConn - The adjacency list between connected components.
 * @param[in, out] distMonster - The distance from the monster to any site on
 * the playfield.
 * @param[in, out] distPlayer - The distance from the player to any site on
 * the playfield.
 * @param[in, out] prevPlayer - The paths from the player to any site on
 * the playfield.
 * @param[in] player - The site the player is currently on.
 * @param[in] monster - The site the monster is currently on.
 *
 * @return a site containing the player's next move.
 ******************************************************************************/

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


    // Determine if a room cycle exists.
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


    // If room cycle does not exist, determine if player is in cycle.

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

            // Choose the safest farthes move away from the monster.
            if (!decisions.empty())
            {
                decIt = decisions.end();
                decIt--;
                nextMove = decIt->second;

            }

            // Look for any site the player can actually reach.
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


                // If that doesn't work, calculate shortest path to cycle.
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

            else
            {
                // Calculate the shortest path to a cycle.
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


/***************************************************************************//**
 * @author Cameron Javaheri
 *
 *
 * @par Description
 *  This function determines if the playfield is in a cycle consisting of
 * ONLY corridor sites.
 *
 * @param[in] connectedCycle - The map containing the connected cycle consisting
 * of only corridor sites.
 * @param[in] player - The site the player is currently standing on.
 *
 * @return true if the player is in a cycle consisting of only corridor sites.
 * @return false if the player is not in a cycle consisting of only corridor sites.
 ******************************************************************************/

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

/***************************************************************************//**
 * @author Cameron Javaheri
 *
 *
 * @par Description
 *  This function performs a breadth first on the player.
 *
 * @param[in, out] marked - A 2D array of bools used to mark the playfield.
 * @param[in, out] prev - A 2D array to store the path.
 * @param[in, out] dist - The 2D array used to keep track of distances
 * to certain sites on the playfield.
 * @param[in] player - The site the player is currently located on.
 * @param[in, out] allocatedMemory - Any memory allocated is stored in this vector.
 *
 ******************************************************************************/


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

/***************************************************************************//**
 * @author Cameron Javaheri
 *
 *
 * @par Description
 *  A helper function for the bfs. Used to store sites that are N, S, E, or W.
 * Calculates previous sites and distances using breadth first search.
 *
 * @param[in, out]  marked - A 2D array containing bools. Marked true if the player can
 * reach a specific location.
 * @param[in, out]  prev - Stores the path to any site on the playfield.
 * @param[in, out]  dist - The shortest distances to a particular site on the playfield.
 * @param[in, out]  myqueue - A queue used for breadth first search.
 * @param[in] temp - The site that the breadth first search is currently examining.
 * @param[in, out] allocatedMemory - The vector used to store any memory that was
 * allocated.
 *
 ******************************************************************************/

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


/***************************************************************************//**
 * @author Cameron Javaheri
 *
 *
 * @par Description
 *  A helper function for the bfs. Used to store room sites that are NE, NW, SE, SW.
 * Calculates previous sites and distances using breadth first search.
 *
 *
 * @param[in, out]  marked - A 2D array containing bools. Marked true if the player can
 * reach a specific location.
 * @param[in, out]  prev - Stores the path to any site on the playfield.
 * @param[in, out]  dist - The shortest distances to a particular site on the playfield.
 * @param[in, out]  myqueue - A queue used for breadth first search.
 * @param[in] temp -  The site that the breadth first search is currently examining.
 * @param[in, out] allocatedMemory - The vector used to store any memory that was
 * allocated.
 *
 ******************************************************************************/



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

/***************************************************************************//**
 * @author Cameron Javaheri
 *
 *
 * @par Description
 *  A function used to deallocate memory used during the program.
 *
 * @param[in, out] markedMonster - The marked array for monster.
 * @param[in, out] prevMonster - The locations to sites from monster.
 * @param[in, out] distMonster - Shortest distances to sites from monster.
 * @param[in, out] markedPlayer - The marked array for player.
 * @param[in, out] prevPlayer - The locations to sites from the player.
 * @param[in, out] distPlayer - The distances from the player to different sites.
 * @param[in, out] allocatedMemory - A vector containing allocated memory.
 *
 ******************************************************************************/

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


/***************************************************************************//**
 * @author Cameron Javaheri
 *
 *
 * @par Description
 *  A function used to allocate memory for breadth first search.
 *
 * @param[in, out]  marked - A 2D array containing bools. Marked true if the player can
 * reach a specific location.
 * @param[in, out]  prev - Stores the path to any site on the playfield.
 * @param[in, out]  dist - The shortest distances to a particular site on the playfield.
 *
 ******************************************************************************/

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



/***************************************************************************//**
 * @author Cameron Javaheri
 * @brief This function attempts to trick the monster on maps like blank.in
 *
 * @par Description
 *   This function determines the monster's ith coordinate, then makes a decision
 * 	to move to the farthest ith coordinate away from the monster, and the middle
 * 	jth coordinate of the map, moves there, then calculates the farthest distance
 *	away from the monster, and moves to that location.
 * @param[in, out] distMonster - A 2D array containing the shortest distances
 * to every site from the monster.
 * @param[in, out] distPlayer - A 2D array containing the shortest distances to
 * every site from the player.
 * @param[in, out] nextMove - The location the monster should calculate a path
 *	to.
 *
 * @param[in] player - The site the player is currently located on.
 * @param[in] monster - The site the monster is currently located on.
 *
 ******************************************************************************/

void Player::monsterTooClose(int **&distMonster, int **&distPlayer, Site* &nextMove, const Site* player, const Site* monster)
{


    static bool reached = false;
    static Site* test = nullptr;

    // Attempt to plot a path towards the middle jth coordinate
    // but farthest ith coordinate away from the monster.

    if (!reached)
    {
        if (monster->i() < player->i())
        {
            test = new Site(N - 1, (N - 1) / 2);
        }

        else if (monster->i() > player->i())
        {
            test = new Site(0 , (N - 1) / 2);
        }

        if (test != nullptr)
        {
            if (player->i() == test->i() && player->j() == test->j())
                reached = true;

        }

        if (!reached && test != nullptr)
            nextMove = test;

    }


}

