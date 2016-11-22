#include "game.h"

/***************************************************************************//**
 * @author Paul Hinker
 *
 * @par Description:
 *    Constructor
 ******************************************************************************/
Game::Game() : moveCount(0), gameOver(NO_DECISION), skipPlayer(false), skipMonster(false)
{ }

/***************************************************************************//**
 * @author Paul Hinker
 *
 * @par Description:
 *    Member function that reads in the playfield datafile.  It is expected
 * that the file starts with a single integer (N) that describes the dimensions
 * of the playfield (N x N).  Room spaces are denoted by '.', corridor spaces
 * '+', the player as '@', the monster as a capital letter from A-Z, and all
 * others as wall.
 *    While the playfield is NxN, the data file should be of the form N rows
 * of 
 ******************************************************************************/
void Game::readFile(const char *fileName)
{
   int N;
   ifstream fin(fileName);
   if (!fin.is_open())
   {
      cerr << "Cannot open file " << fileName << " for reading.\n";
      exit(-1);
   }
   fin >> N >> winMoves;
   cout << "Winning Moves: " << winMoves << endl;
   char **board = new char*[N];
   char chRow[256];

   fin.ignore();
   for (int i = 0; i < N; i++) 
   {
      fin.getline(chRow, 256);
      board[i] = new char[N];
      for (int j = 0; j < N; j++) 
      {
          board[i][j] = chRow[2*j];
      }
    }
    fin.close();
    playfield = new Playfield(board, N);
    monster = new Monster(playfield);
    player   = new Player(playfield);
}

static const Site* threadedMMove(Monster *monster);
static const Site* threadedPMove(Player *player);

const Site* threadedMMove(Monster *monster)
{
   return monster->move();
}

const Site* threadedPMove(Player *player)
{
   return player->move();
}

/***************************************************************************//**
 * @author Paul Hinker
 *
 * @par Description:
 *    This function attempts to execute on turn of the chase.  The monster
 * moves first checking to see if it has intercepted the player.  The player
 * moves second and it checks to see if the player steps into the same
 * square as the monster.
 *
 * If either case is true, the gameOver member data is set to true so that
 * the event loop can be exited in utilityCentral.
 *
 * A side-effect of this function is that it increments the member data
 * moveCount to keep track of the number of turns that have passed.
 ******************************************************************************/
void Game::executeMove(ofstream& ResultsFile)
{
   auto startTime = chrono::system_clock::now();
   std::chrono::duration<double> diff;
   static int playerDelayCount = 0;
   static int monsterDelayCount = 0;
#ifdef DEBUG
   const float RedCardDelay = 60.0;
#else
   const float RedCardDelay = 10.0;
#endif
   // monster moves
   if (!skipMonster)
   {
      if (getMonsterSite()->equals(getPlayerSite()))
         gameOver = MONSTER_WINS;
      else
      {
         startTime = chrono::system_clock::now();
         future<const Site *> mMove = async(launch::async, threadedMMove,monster);

         while (mMove.wait_for(chrono::milliseconds(0)) != future_status::ready
             && ((diff = chrono::system_clock::now() - startTime).count() < RedCardDelay)) { }

         diff = chrono::system_clock::now() - startTime;
         if (diff.count() > RedCardDelay)
         {
            ResultsFile << "Monster loses!  Redcarded for too much time!" 
                        << endl;
            gameOver = MONSTER_TIMEOUT;
            ResultsFile.close();
            exit(-1);
         }
         else
         {
            const Site* result = mMove.get();
            try
            {
               playfield->setMonsterSite(result);
            }
            catch(...)
            {
               gameOver = MONSTER_CHEATING;
            }
         } 

         if (diff.count() < RedCardDelay && diff.count() > 1.0)
         { 
            skipMonster = true;
            cout << "Monster will lose next turn!  Delay of game!\n";
            monsterDelayCount++;
            if (monsterDelayCount > 7) gameOver = MONSTER_TIMEOUT;
         }
      }
   }
   else
      skipMonster = false;

   // player moves
   if (!skipPlayer)
   {
      if (getMonsterSite()->equals(getPlayerSite()))
         gameOver = MONSTER_WINS;
      else
      {
         startTime = chrono::system_clock::now();
         future<const Site *> mMove = async(launch::async, threadedPMove,player);

         while (mMove.wait_for(chrono::milliseconds(0)) != future_status::ready
            && ((diff = chrono::system_clock::now() - startTime).count() < RedCardDelay)){}

         diff = chrono::system_clock::now() - startTime;

         if (diff.count() > RedCardDelay)
         {
            ResultsFile << "Player loses! Redcarded for too much time!" 
                        << endl;
            gameOver = PLAYER_TIMEOUT;
            ResultsFile.close();
            exit(-1);
         }
         else
         {
            const Site* result = mMove.get();
            try
            {
               playfield->setPlayerSite(result); 
            }
            catch (...)
            {
               gameOver = PLAYER_CHEATING;
            }
         }

         if (diff.count() < RedCardDelay && diff.count() > 1.0)
         { 
            skipPlayer = true;
            cout << "Player will lose next turn!  Delay of game!\n";
            playerDelayCount++;
            if (playerDelayCount > 7) gameOver = PLAYER_TIMEOUT;
         }
      }
   }
   else
      skipPlayer = false;

   if (moveCount == winMoves)
      gameOver = PLAYER_WINS;

   moveCount++;
}
