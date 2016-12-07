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
    Player(Playfield* p);
    const Site* move();

    vector<Site*> findCorridors(vector<Site*> &corridors);

    map<Site*, vector<Site*>> findAdjLists(vector<Site*> corridors);


    void bfs(bool **&marked, Site* **&prev, int **&dist, const Site* player, vector<Site*> &allocatedMemory);

    void allocateStorage(bool **&marked, Site* **&prev, int **&dist);

    void checkAdjacentRoomSquares(bool **marked, Site* **prev, int **dist, queue<Site*> &myqueue, Site* temp, vector<Site*> &allocatedMemory);


    void checkAdjacentCorridorSquares(bool **marked, Site* **prev, int **dist, queue<Site*> &myqueue, Site* temp, vector<Site*> &allocatedMemory);



    void deallocateStorage(bool **&markedMonster, Site* **&prevMonster, int **&distMonster, bool **&markedPlayer, Site* **&prevPlayer, int **&distPlayer, vector<Site*> &allocatedMemory);

    Site* getNextMove(bool **&markedMonster, Site* **&prevMonster, int **&distMonster, bool **&markedPlayer, Site* **&prevPlayer, int **&distPlayer, const Site* monster, const Site* player, vector<Site*> &allocatedMemory, map<Site*, vector<Site*>> &adj);



    Site* findCorridorCycle(map<Site*, vector<Site*>> connectedCycle, map<Site*, vector<Site*>> adjConn, int **&distMonster, int **&distPlayer, Site* **&prevPlayer, const Site* player, const Site* monster);



    bool doCorridorsExist();

    bool doWallsExist();

    int countDisconnectedComponents(map<Site*, vector<Site*>> &adj);


    map<Site*, vector<Site*>> getAdjListDisc(map<Site*, vector<Site*>> &adj, vector<Site*> &allocatedMemory);

    void removeDeadEnds(map<Site*, vector<Site*>> &adj);

    void search(vector<Site*> &vectDisc, Site* site, int &i, int &j);

    void checkDuplicates(vector<Site*> &vectDisc, Site* site);


    Site* calculateNextRoom(map<Site*, vector<Site*>> &adjDisc, int **&distMonster, int**&distPlayer, Site* **&prevPlayer, const Site* player, const Site* monster, map<Site*, vector<Site*>> connectedCycle);

    void removeDeadEndVertices(map<Site*, vector<Site*>> &adjDisc);

    void run(const Site* player, Site* &nearestCorr, int **&distPlayer, int **&distMonster, vector<Site*> &cycle, map<Site*, vector<Site*>> &adjDisc);

    map<Site*, vector<Site*>> findConnectedComponents(map<Site*, vector<Site*>> adj);

    void getMarkedArray(bool **&marked);

    void freeMarkedArray(bool **&marked);

    map<Site*, vector<Site*>> getCyclesWithinCorridors(map<Site*, vector<Site*>> adjConn);

    bool isPlayerInCorridorCycle(map<Site*, vector<Site*>> connectedCycle, const Site* player);


    map<Site*,vector<Site*>> checkForConnectedDeadEnds(map<Site*, vector<Site*>> adjConn);

    bool hasAdjacentRoom(Site* site);

    Site* findCyclesBetweenRooms(map<Site*, vector<Site*>> cycleBetweenRooms, map<Site*, vector<Site*>> adj, int **&distMonster, int **&distPlayer, Site* **&prevPlayer, const Site* player, const Site* monster);


    void searchAllAdjCorrSites(vector<Site*> &vectDisc, Site* site, int &i, int &j);

    vector<Site*> getCycleChoices(map<Site*, vector<Site*>> cycleBetweenRooms, const Site* player);

    Site* chooseNextCorridor(map<Site*, vector<Site*>> cycleBetweenRooms, int **&distPlayer,  int **&distMonster, const Site* player);

    Site* getAdjacentRoomSite(Site* site);

    void setCoordinates(int &i, int &j);

    vector<Site*> removeSitesWithOneAdjacentSite(vector<Site*> myvector);

    Site* scanAdjacentCorridorSites(const Site* player, int **&distMonster);

    Site* stayAliveAsLongAsPossible(int **&distMonster, int **&distPlayer);

    Site* calculateFinalDestination(Site* nextMove, int**&distPlayer, Site* **&prevPlayer, const Site* player);

    vector<Site*> removeDiagonalRooms(vector<Site*> roomCycle);

    Site* getRoomCycle(int **&distMonster, int **&distPlayer, Site* **&prevPlayer, const Site* player);

    bool checkPerimeterForAdjWalls(Site* site);

    multimap<Site*, vector<Site*>> findAllRoomCycles(map<Site*, vector<Site*>> adjConn);

    Site* chooseNextRoom(int **&distMonster, int **&distPlayer, Site* **&prevPlayer, vector<Site*> roomCycle, const Site* player);

    map<Site*, vector<Site*>> findAdjRoomLists(vector<Site*> rooms);

    vector<Site*> getCorrectCycle(multimap<int, vector<Site*>> vectorSizes, int **&distPlayer, int **&distMonster);

    bool isPlayerInConnectedRoomCycle(map<Site*, vector<Site*>> cycleBetweenRooms, const Site* player);



};
#endif
