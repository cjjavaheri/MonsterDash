#include "playfield.h"


/**************************************************************************//**
 * @author Paul Hinker
 *
 * @par Description:
 *    Constructor for the playfield
 *
 * @param[in] board - the NxN board representing the input file
 * @param[in] size - the x (or y) dimension of the playfield
 *****************************************************************************/
Playfield::Playfield(char **board, int size)
{
   N = size;
   Room     = new bool*[N];
   Corridor = new bool*[N];

   for (unsigned int i = 0; i < N; i++) 
   {
      Room[i] = new bool[N];
      Corridor[i] = new bool[N];
      for (unsigned int j = 0; j < N; j++) 
      {
         Room[i][j] = false;
         Corridor[i][j] = false;
         if (board[i][j] == '.') 
            Room[i][j] = true;
         else if (board[i][j] == '+') 
            Corridor[i][j] = true;
         else if (board[i][j] == '@')
         {
            playerSite = new Site(i,j);
            Room[i][j] = '.';
         }
         else if (board[i][j] >= 'A' && board[i][j] <= 'Z')
         {
            monsterSite = new Site(i,j);
            Room[i][j] = '.';
         }
      }
    }
}

/**************************************************************************//**
 * @author Paul Hinker
 *
 * @par Description:
 *    Check to see if a site is a corridor site
 *
 * @param[in] v - the site to query about corridorness
 *****************************************************************************/
bool Playfield::isCorridor(const Site *v) const
{
        unsigned int i = v->i();
        unsigned int j = v->j();
        if (i < 0 || j < 0 || i >= N || j >= N) 
           return false;
        return Corridor[i][j];
}

bool Playfield::isCorridor(const unsigned int i, const unsigned int j) const
{
   if (i < 0 || j < 0 || i >= N || j >= N)
      return false;
   return Corridor[i][j];
}

/**************************************************************************//**
 * @author Paul Hinker
 *
 * @par Description:
 *    Check to see if a site is a room site
 *
 * @param[in] v - the site to query about roomness
 *****************************************************************************/
bool Playfield::isRoom(const Site* v) const
{
   unsigned int i = v->i();
   unsigned int j = v->j();
   if (i < 0 || j < 0 || i >= N || j >= N) 
      return false;
   return Room[i][j];
}

bool Playfield::isRoom(const unsigned int i, const unsigned int j) const
{
   if (i < 0 || j < 0 || i >= N || j >= N) 
      return false;
   return Room[i][j];
}

/**************************************************************************//**
 * @author Paul Hinker
 *
 * @par Description:
 *    Check to see if a site is a wall site
 *
 * @param[in] v - the site to query about wallness
 *****************************************************************************/
bool Playfield::isWall(const Site *v) const
{
   return (!isRoom(v) && !isCorridor(v));
}

bool Playfield::isWall(const unsigned int i, const unsigned int j) const
{
   return (!isRoom(i, j) && !isCorridor(i, j));
}

/**************************************************************************//**
 * @author Paul Hinker
 *
 * @par Description:
 *    Check to see if a site is a wall site
 *
 * @param[in] v - the site to query about wallness
 *****************************************************************************/
bool Playfield::isLegalMove(const Site* v, const Site* w) const
{
   unsigned int i1 = v->i();
   unsigned int j1 = v->j();
   unsigned int i2 = w->i();
   unsigned int j2 = w->j();

   if (i1 < 0 || j1 < 0 || i1 >= N || j1 >= N) 
      return false;

   if (i2 < 0 || j2 < 0 || i2 >= N || j2 >= N)
      return false;

   if (isWall(v) || isWall(w)) 
      return false;

   if (((i1 > i2) ? (i1 - i2) : (i2 - i1)) > 1)  
      return false;

   if (((j1 > j2) ? (j1 - j2) : (j2 - j1)) > 1)  
      return false;

   if (isRoom(v) && isRoom(w)) 
      return true;

   if (i1 == i2)               
      return true;

   if (j1 == j2)               
      return true;

   return false;
}

/**************************************************************************//**
 * @author Paul Hinker
 *
 * @par Description:
 *    Move the player to a new site
 *
 * @param[in] v - the site to the player wants to move
 *
 * Throws runtime_error if the player attempts to move to an illegal spot
 *****************************************************************************/
void Playfield::setPlayerSite(const Site *v)
{
   if (isLegalMove(playerSite, v))
      playerSite = v;
   else 
      throw std::runtime_error("Player caught cheating");
}

/**************************************************************************//**
 * @author Paul Hinker
 *
 * @par Description:
 *    Move the monster to a new site
 *
 * @param[in] v - the site to the monster wants to move
 *
 * Throws runtime_error if the monster attempts to move to an illegal spot
 *****************************************************************************/
void Playfield::setMonsterSite(const Site *v)
{
   if (isLegalMove(monsterSite, v))
      monsterSite = v;
   else 
      throw std::runtime_error("Monster caught cheating");
}

/**************************************************************************//**
 * @author Paul Hinker
 *
 * @par Description:
 *    Overloaded output for playfield
 *
 * @param[in] os - the output stream
 * @param[in] play - the playfield
 *****************************************************************************/
ostream& operator<<(ostream& os, Playfield *play)
{
   string line = "";
   const Site *playerSite = play->playerSite;
   const Site *monsterSite = play->monsterSite;

   for (unsigned int i = 0 ; i < play->N ; i++)
   {
      for (unsigned int j = 0 ; j < play->N ; j++)
      {
         Site *site = new Site(i, j);
         if (playerSite->equals(monsterSite) &&
             playerSite->equals(site)) line += "* ";
         else if (playerSite->equals(site)) line += "@ ";
         else if (monsterSite->equals(site)) line += "M ";
         else if (play->isRoom(site)) line += ". ";
         else if (play->isCorridor(site)) line += "+ ";
         else if (play->isWall(site)) line += "  ";
      }
      line += "\n";
   }
   os << line;
   return os;
}
   
/**************************************************************************//**
 * @author Paul Hinker
 *
 * @par Description:
 *    Overloaded output for Site
 *
 * @param[in] os - the output stream
 * @param[in] site - the site to output
 *****************************************************************************/
ostream& operator<<(ostream& os, Site* site)
{
   os << "Site[" << site->i() << ", " << site->j() << "]" << endl;
   return os;
}
