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
   const Site* monster   = playfield->getMonsterSite();
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
}
