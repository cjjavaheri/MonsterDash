#include "util.h"

string getName(const char *fileName)
{
   ifstream fin(fileName);
   string contents;

   getline(fin, contents);
   return contents;
}

/**************************************************************************//**
 * @author Paul Hinker
 *
 * @par Description:
 * This function gets called when an event happens in callbacks. Depending on 
 * the enum and data passed in it will do different things. When a mouse click 
 * is passed in, it sends that data to the click function in the container 
 * class screen. When a mouse drag is sent in, it sends it to the clickAndDrag 
 * function in screen.  It does the same for keyboard presses, sending them to 
 * keyboardAction in screen.
 *
 * @param[in] event - structure that contains data about the event to handle
 *****************************************************************************/
void utilityCentral(const Event & event) 
{
    static Game game; 
    static string PlayerName;
    static string MonsterName;
    static string PlayfieldName;
    static ofstream ResultsFile("ResultsFile.txt", ofstream::out | ofstream::app);
	switch (event.event) 
    {
		case INIT: 
        {
            game.readFile(event.fileName);
            PlayerName = getName("PlayerName.txt");
            MonsterName = getName("MonsterName.txt");
            PlayfieldName = getName("PlayfieldName.txt");
            ResultsFile << "Player::" << PlayerName << "::" <<
                           "Monster::" << MonsterName << "::" <<
                           "Playfield::" << PlayfieldName << "::";
			break;
		}
		case DISPLAY: 
        {
            drawPlayfield(game);
			break;
		}
		case KEYBOARD:
			switch (event.key) 
            {
				case 'q':
				case 'Q':
				case ESC:
					exit(0);
			}
			break;
		case MOUSECLICK:
        	glutPostRedisplay();
			break;
		case MOUSEDRAG:
			break;
		case RESHAPE:
			break;
        case TICKTOCK:
            game.executeMove(ResultsFile);
            if (game.isGameOver())
            {
               switch (game.getResults())
               {
                  case MONSTER_WINS:
                     ResultsFile << "Monster wins player at move "
                            << game.getMoveCount()
                            << " on square [" << game.getMonsterSite()->i()
                            << ", " << game.getMonsterSite()->j() << "]"<< endl;
                     break;
                  case PLAYER_WINS:
                     ResultsFile << "Player wins on time by staying alive for "
                              << game.getMoveCount() << " moves\n";
                     break;
                  case MONSTER_TIMEOUT:
                     ResultsFile << "Player wins due to monster timeout" <<endl;
                     break;
                  case PLAYER_TIMEOUT:
                     ResultsFile << "Monster wins due to player timeout" <<endl;
                     break;
                  case MONSTER_CHEATING:
                     ResultsFile << "Player wins due to monster cheating"<<endl;
                     break;
                  case PLAYER_CHEATING:
                     ResultsFile << "Player wins due to monster cheating"<<endl;
               }
               exit(0);
            }
            glutPostRedisplay();
            break;
	}
}

/**************************************************************************//**
 * @author Paul Hinker
 *
 * @par Description:
 * This function will take the current game board and render the playfield
 * using OpenGL.  It uses simple block graphics.
 *
 * @param[in] game - The game which contains the playfield to render
 ****************************************************************************/
void drawPlayfield(Game game)
{
   const Playfield *playfield = game.getPlayfield();

   // Do some maths to scale the playfield to fit into the current screen
   int size = playfield->size();
   const int border = 100;
   int xDist = glutGet(GLUT_WINDOW_WIDTH);
   int yDist = glutGet(GLUT_WINDOW_HEIGHT);
   int width = (xDist-border*2)/size;
   int height = (yDist-border*2)/size;

   int x = border;
   int y = yDist - border;

   // The player and monster are rendered differently so find their sites
   const Site *player = playfield->getPlayerSite();
   const Site *monster = playfield->getMonsterSite();

   // Display the current number of moves in the upper left corner of the screen
   stringstream ss;
   ss << "Move: " << game.getMoveCount();
   DrawTextString(ss.str().c_str(), 20, yDist-20, White);

   // The playfield is size x size, loop over all the sites and render a block
   // of color corresponding to the contents of the site.
   for (int i = 0 ; i < size ; i++)
   {
      for (int j = 0 ; j < size ; j++)
      {
         const Site *site = new Site(i,j);
         if (site->equals(player))
            DrawFilledRectangle(x, y, x + width, y - height, White);
         else if (site->equals(monster))
            DrawFilledRectangle(x, y, x + width, y - height, Red);
         else if (playfield->isRoom(site))
            DrawFilledRectangle(x, y, x + width, y - height, Blue);
         else if (playfield->isCorridor(site))
            DrawFilledRectangle(x, y, x + width, y - height, Green);
         else
            DrawFilledRectangle(x, y, x + width, y - height, Grey);
   
         DrawRectangle(x, y, x + width, y-height, Black);
         x += width;
      }
      x = border;
      y -= height;
   }
}

/**************************************************************************//**
 * @author Paul Hinker & Dr. Weiss
 *
 * @par Description:
 *    Draw a rectangle to the screen
 *
 * @param[in] x1 - x component of the lower-right corner
 * @param[in] y1 - y component of the lower-right corner
 * @param[in] x2 - x component of the upper-left corner
 * @param[in] y2 - y component of the upper-left corner
 * @param[in] color - Color for the border of the rectangle
 ****************************************************************************/
void DrawRectangle(float x1, float y1, float x2, float y2, const float color[])
{
    glColor3fv( color );
    glBegin( GL_LINE_LOOP );
        glVertex2f( x1, y1 );
        glVertex2f( x2, y1 );
        glVertex2f( x2, y2 );
        glVertex2f( x1, y2 );
    glEnd();
    glFlush();
}

/**************************************************************************//**
 * @author Paul Hinker & Dr. Weiss
 *
 * @par Description:
 *    Draw a filled rectangle to the screen
 *
 * @param[in] x1 - x component of the lower-right corner
 * @param[in] y1 - y component of the lower-right corner
 * @param[in] x2 - x component of the upper-left corner
 * @param[in] y2 - y component of the upper-left corner
 * @param[in] color - Color for the border of the rectangle
 ****************************************************************************/
void DrawFilledRectangle( float x1, float y1, float x2, float y2, const float color[] )
{
    glColor3fv( color );
    glBegin( GL_POLYGON );
        glVertex2f( x1, y1 );
        glVertex2f( x2, y1 );
        glVertex2f( x2, y2 );
        glVertex2f( x1, y2 );
    glEnd();
    glFlush();
}

/**************************************************************************//**
 * @author Paul Hinker & Dr. Weiss
 *
 * @par Description:
 *    Draw a text string on the screen
 *
 * @param[in] string - string to render to the screen
 * @param[in] x - x component of the starting point
 * @param[in] y - y component of the starting point
 * @param[in] color - Color for the string
 ****************************************************************************/
void DrawTextString(const char *string, int x, int y, const float color[] )
{
    glColor3fv( color );
    glRasterPos2i( x, y );
    while ( *string )
    glutBitmapCharacter( GLUT_BITMAP_8_BY_13, *string++ );
}
