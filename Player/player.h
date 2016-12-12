/**************************************************************************//**
 * @file player.h
 *
 * @brief The header file for the player class.
 *
 * @par Description
 *    This header file contains all of the functions which the player
 *	uses to both analyze the underlying graph and, determine which
 *	move it wants to make next.
 *
 * @author Cameron Javaheri
 *
 * @par Class:
 *    CSC300 Data Structures
 *
 * @date Fall 2015
 *
 *
 *****************************************************************************/



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

/***************************************************************************//**
 * @class Player
 *
 * @brief A class used to create a player.
 *
 ******************************************************************************/

class Player
{
    Playfield* playfield;
    int N;

public:

    /// Create a player object.
    Player(Playfield* p);

    /// Move a player object.
    const Site* move();

    /// Get all corridors.
    vector<Site*> findCorridors(vector<Site*> &corridors);

    /// Get adjacency list for corridors.
    map<Site*, vector<Site*>> findAdjLists(vector<Site*> corridors);


    /// Perform a breadth first search.
    void bfs(bool **&marked, Site* **&prev, int **&dist, const Site* player, vector<Site*> &allocatedMemory);

    /// Allocate memory for breadth first search.
    void allocateStorage(bool **&marked, Site* **&prev, int **&dist);

    /// Check room squares during breadth first search.
    void checkAdjacentRoomSquares(bool **marked, Site* **prev, int **dist, queue<Site*> &myqueue, Site* temp, vector<Site*> &allocatedMemory);


    /// Check room squares and corridor squares during breadth first search.
    void checkAdjacentCorridorSquares(bool **marked, Site* **prev, int **dist, queue<Site*> &myqueue, Site* temp, vector<Site*> &allocatedMemory);



    /// Deallocate memory used in the program.
    void deallocateStorage(bool **&markedMonster, Site* **&prevMonster, int **&distMonster, bool **&markedPlayer, Site* **&prevPlayer, int **&distPlayer, vector<Site*> &allocatedMemory);

    /// Calculate the player's next best move.
    Site* getNextMove(bool **&markedMonster, Site* **&prevMonster, int **&distMonster, bool **&markedPlayer, Site* **&prevPlayer, int **&distPlayer, const Site* monster, const Site* player, vector<Site*> &allocatedMemory, map<Site*, vector<Site*>> &adj);


    /// Find a cycle between ONLY corridor sites.
    Site* findCorridorCycle(map<Site*, vector<Site*>> connectedCycle, map<Site*, vector<Site*>> adjConn, int **&distMonster, int **&distPlayer, Site* **&prevPlayer, const Site* player, const Site* monster);



    /// Determine if the playfield has corridors.
    bool doCorridorsExist();

    /// Determine if the playfield has walls.
    bool doWallsExist();

    /// Count the number of disconnected components.
    int countDisconnectedComponents(map<Site*, vector<Site*>> &adj);


    /// Get the adjacency list for disconnected components.
    map<Site*, vector<Site*>> getAdjListDisc(map<Site*, vector<Site*>> &adj, vector<Site*> &allocatedMemory);

    /// Remove any vertices which lead to a dead end.
    void removeDeadEnds(map<Site*, vector<Site*>> &adj);

    /// Search a room for adjacent corridor sites.
    void search(vector<Site*> &vectDisc, Site* site, int &i, int &j);

    /// Erase duplicates that got stored when using the search function above.
    void checkDuplicates(vector<Site*> &vectDisc, Site* site);


    /// Determine the next best move when in a cycle between room sites and disconnected
    /// components.
    Site* calculateNextRoom(map<Site*, vector<Site*>> &adjDisc, int **&distMonster, int**&distPlayer, Site* **&prevPlayer, const Site* player, const Site* monster, map<Site*, vector<Site*>> connectedCycle);

    /// Remove vertices that lead to a dead end.
    void removeDeadEndVertices(map<Site*, vector<Site*>> &adjDisc);

    /// When the monster gets too close, run.
    void run(const Site* player, Site* &nearestCorr, int **&distPlayer, int **&distMonster, vector<Site*> &cycle, map<Site*, vector<Site*>> &adjDisc);


    /// Find all connected components.
    map<Site*, vector<Site*>> findConnectedComponents(map<Site*, vector<Site*>> adj);

    /// Get a 2D array of bools.
    void getMarkedArray(bool **&marked);

    /// Free a 2D array of bools.
    void freeMarkedArray(bool **&marked);

    /// Find the cycles betweeen ONLY corridor sites.
    map<Site*, vector<Site*>> getCyclesWithinCorridors(map<Site*, vector<Site*>> adjConn);

    /// Determine if the player is in a cycle of ONLY corridor sites.
    bool isPlayerInCorridorCycle(map<Site*, vector<Site*>> connectedCycle, const Site* player);

    /// Some connected components may have dead ends, remove those.

    map<Site*,vector<Site*>> checkForConnectedDeadEnds(map<Site*, vector<Site*>> adjConn);

    /// Determine if a site has a room next to it.
    bool hasAdjacentRoom(Site* site);

    /// Find cycles between room sites and connected components.
    Site* findCyclesBetweenRooms(map<Site*, vector<Site*>> cycleBetweenRooms, map<Site*, vector<Site*>> adj, int **&distMonster, int **&distPlayer, Site* **&prevPlayer, const Site* player, const Site* monster);

    /// Search a room for adjacent corridor sites to that room.
    void searchAllAdjCorrSites(vector<Site*> &vectDisc, Site* site, int &i, int &j);

    /// Get the decisions for the player to choose from given a cycle between the rooms.
    vector<Site*> getCycleChoices(map<Site*, vector<Site*>> cycleBetweenRooms, const Site* player);

    /// Choose an optimum corridor site from the data from getCycleChoices.
    Site* chooseNextCorridor(map<Site*, vector<Site*>> cycleBetweenRooms, int **&distPlayer,  int **&distMonster, const Site* player);

    ///  Find any adjacent room site.
    Site* getAdjacentRoomSite(Site* site);

    /// Set i and j to bottom right corner of a room.
    void setCoordinates(int &i, int &j);

    /// Remove sites that lead to a dead end.
    vector<Site*> removeSitesWithOneAdjacentSite(vector<Site*> myvector);

    /// See which corridor sites are feasable for the player to plot a path to.
    Site* scanAdjacentCorridorSites(const Site* player, int **&distMonster);

    /// Calculate the longest distance away from the monster.
    Site* stayAliveAsLongAsPossible(int **&distMonster, int **&distPlayer);

    /// Find a path to a specific site.
    Site* calculateFinalDestination(Site* nextMove, int**&distPlayer, Site* **&prevPlayer, const Site* player);

    /// Remove room sites that have adjacent diagonal rooms.
    vector<Site*> removeDiagonalRooms(vector<Site*> roomCycle);

    /// Find a cycle between ONLY room sites.
    Site* getRoomCycle(int **&distMonster, int **&distPlayer, Site* **&prevPlayer, const Site* player);

    /// Check perimeter of the playfield for walls.
    bool checkPerimeterForAdjWalls(Site* site);

    /// Find all possible cycles between ONLY room sites.
    multimap<Site*, vector<Site*>> findAllRoomCycles(map<Site*, vector<Site*>> adjConn);

    /// Make a decision given the data from findAllRoomCycles.
    Site* chooseNextRoom(int **&distMonster, int **&distPlayer, Site* **&prevPlayer, vector<Site*> roomCycle, const Site* player);

    /// Get the adjacency list for room sites.
    map<Site*, vector<Site*>> findAdjRoomLists(vector<Site*> rooms);

    /// Determine the correct cycle based upon the number of vertices in the cycle.
    vector<Site*> getCorrectCycle(multimap<int, vector<Site*>> vectorSizes, int **&distPlayer, int **&distMonster);

    /// Determine if the player has reached the cycle between ONLY room sites.
    bool isPlayerInConnectedRoomCycle(map<Site*, vector<Site*>> cycleBetweenRooms, const Site* player);

    /// Determine if the player can trick the monster on maps like blank.in
    void monsterTooClose(int **&distMonster, int **&distPlayer, Site* &nextMove, const Site* player, const Site* monster);

};
#endif
