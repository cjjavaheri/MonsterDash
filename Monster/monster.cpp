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
   const Site* player = playfield->getPlayerSite();
   Site* test;
   cout << "Monster[i][j] " << monster->i() << " " << monster->j() << endl;
   cout << "PLayer[i][j] " << player->i() << " " << player->j() << endl;
   cout << endl << endl;

	if (playfield->isRoom(monster) == true)
	{
		test = moveRoom(monster, player);
		return test;
	}
	if (playfield->isCorridor(monster) == true)
	{
		test = moveCorridor(monster, player);
		return test;
	}

   return nullptr;

/*
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

*/
}



Site* Monster::moveRoom(const Site* monster, const Site* player)
{
	Site* test = nullptr;

			// Monster's row is smaller than player's row
		if (monster->i() < player->i())
		{
			//Monster's column is smaller than player's column
			if (monster->j() < player->j())
			{
				test = new Site(monster->i() + 1, monster->j() + 1);
				if (playfield->isLegalMove(monster, test))
				{
					return test;
				}

				test = new Site(monster->i() + 1, monster->j());
				if (playfield->isLegalMove(monster, test))
				{
					return test;
				}
				
				test = new Site(monster->i() , monster->j() + 1);
				if (playfield->isLegalMove(monster, test))
				{
					return test;
				}
				
			}
			//Monster's column is greater than player's column
		        if (monster->j() > player->j())
			{
				test = new Site(monster->i() + 1, monster->j() - 1);
				if (playfield->isLegalMove(monster, test))
				{
					return test;
				}

				test = new Site(monster->i() + 1, monster->j());
				if (playfield->isLegalMove(monster, test))
				{
					return test;
				}

				test = new Site(monster->i(), monster->j() - 1);
				if (playfield->isLegalMove(monster, test))
				{
					return test;
				}
			}
			//Monster's column is equal to player's column
			else
			{
				test = new Site(monster->i() + 1, monster->j());
				if (playfield->isLegalMove(monster, test))
				{
					return test;
				}
			}
		}
		//Monster's row is greater than player's row
	        if (monster->i() > player->i())
		{
				//Monster's column is smaller than player's column
			if (monster->j() < player->j())
			{
				test = new Site(monster->i() - 1, monster->j() + 1);
				if (playfield->isLegalMove(monster, test))
				{
					return test;
				}

				test = new Site(monster->i() - 1, monster->j());
				if (playfield->isLegalMove(monster, test))
				{
					return test;
				}

				test = new Site(monster->i(), monster->j() + 1);
				if (playfield->isLegalMove(monster, test))
				{
					return test;
				}
			}
			//Monster's column is greater than player's column
		        if (monster->j() > player->j())
			{
				test = new Site(monster->i() - 1, monster->j() - 1);
				if (playfield->isLegalMove(monster, test))
				{
					return test;
				}

				test = new Site(monster->i() - 1, monster->j());
				if (playfield->isLegalMove(monster, test))
				{
					return test;
				}

				test = new Site(monster->i(), monster->j() - 1);
				if (playfield->isLegalMove(monster, test))
				{
					return test;
				}
			}
			//Monster's column is equal to player's column
			else
			{
				test = new Site(monster->i() - 1, monster->j());
				if (playfield->isLegalMove(monster, test))
				{
					return test;
				}
			}
		}
		// Monster's row is equal to player's row
		else
		{
			//Monster's column is smaller than player's column
			if (monster->j() < player->j())
			{
				test = new Site(monster->i(), monster->j() + 1);
				if (playfield->isLegalMove(monster, test))
				{
					return test;
				}
			}
			//Monster's column is greater than player's column
		        if (monster->j() > player->j())
			{
				test = new Site(monster->i(), monster->j() - 1);
				if (playfield->isLegalMove(monster, test))
				{
					return test;
				}
			}
			//Monster's column is equal to player's column
			else
			{
				test = new Site(monster->i() , monster->j());
				if (playfield->isLegalMove(monster, test))
				{
					return test;
				}
			}
		}

}

Site* Monster::moveCorridor(const Site* monster, const Site* player)
{
	Site* test = nullptr;
			// Monster's row is smaller than player's row
		if (monster->i() < player->i())
		{
			test = new Site(monster->i() + 1, monster->j());
			if (playfield->isLegalMove(monster, test))
				return test;
			
			// Monster's column is smaller than player's column
			if (monster->j() < player->j())
			{
				test = new Site(monster->i(), monster->j() + 1 );
				if (playfield->isLegalMove(monster, test))
					return test;
				
			}
			//Monster's column is greater than player's column
			if (monster->j() > player->j())
			{
				test = new Site(monster->i(), monster->j() - 1);
				if (playfield->isLegalMove(monster, test))
					return test;
				
			}
		}
		//Monster's row is greater than player's row
	       if (monster->i() > player->i())
		{
			test = new Site(monster->i() - 1, monster->j());
			if (playfield->isLegalMove(monster, test))
				return test;

			// Monster's column is smaller than player's column
			if (monster->j() < player->j())
			{
				test = new Site(monster->i(), monster->j() + 1 );
				if (playfield->isLegalMove(monster, test))
					return test;
				
			}
			//Monster's column is greater than player's column
			if (monster->j() > player->j())
			{
				test = new Site(monster->i(), monster->j() - 1);
				if (playfield->isLegalMove(monster, test))
					return test;
				
			}
		}
		//Monster's row is equal to player's row
		if (monster->i() == player->i())
		{
			//Monster's column is smaller than player's column
			if (monster->j() < player->j())
			{
				test = new Site(monster->i(), monster->j() + 1);
				if (playfield->isLegalMove(monster, test))
					return test;
			}
			//Monster's column is greater than player's column
			if (monster->j() > player->j())
			{
				test = new Site(monster->i(), monster->j() - 1);
				if (playfield->isLegalMove(monster, test))
					return test;
			}

			//Monster's column is equal to player's column
			if (monster->j() == player->j())
			{
				test = new Site(monster->i(), monster->j());
				if (playfield->isLegalMove(monster, test))
					return test;
			}
		}

}
