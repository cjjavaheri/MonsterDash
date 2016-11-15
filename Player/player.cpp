#include <vector>
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
   int myI = player->i();
   int myJ = player->j();

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
}
