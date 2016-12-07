/**************************************************************************//**
 * @file monster.h
 *
 * @brief The header file for the monster class.
 *
 * @par Description
 *    This header file contains all of the functions which the monster
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



#ifndef __MONSTER_H
#define __MONSTER_H
#include <stdlib.h>
#include <vector>
#include "site.h"
#include "playfield.h"
#include <queue>


/***************************************************************************//**
 * @class Monster
 *
 * @brief A class used to create a monster.
 *
 ******************************************************************************/

class Monster 
{
    Playfield* playfield;
    int N;
 
    public:
	/// Construct a monster.
       Monster(Playfield* play);

	/// Move monster.
       const Site* move();

	/// Perform a breadth first search.
       void bfs(bool **&marked, Site* **&prev, int **&dist, const Site* monster, vector<Site*> &allocatedMemory);

	/// Allocate memory for breadth first search.
       void allocateStorage(bool **&marked, Site* **&prev, int **&dist);

	/// Deallocate memory used in program.
       void deallocateStorage(bool **&marked, Site* **&prev, int **&dist, vector<Site*> &allocatedMemory);

	/// Look at room sites in breadth first search.
       void checkAdjacentRoomSquares(bool **marked, Site* **prev, int **dist, queue<Site*> &myqueue, Site* temp, vector<Site*> &allocatedMemory);


	/// Look at corridor sites and room sites in breadth first search.
	void checkAdjacentCorridorSquares(bool **marked, Site* **prev, int **dist, queue<Site*> &myqueue, Site* temp, vector<Site*> &allocatedMemory);


	/// Calculate the next best move.
	 Site* getNextMove(bool **&marked, Site* **&prev, int **&dist, const Site* monster, const Site* player, vector<Site*> &allocatedMemory);
};

#endif
