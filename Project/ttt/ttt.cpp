// for reference, using the implementation at http://www.angelfire.com/linux/nexusone/projects.html
// as my base game. many, many thanks!!!

#include <GL/glut.h>
#include <cstdio>
#include <iostream>
#include <cstdlib>
#include <cmath>
#include <vector>
#include <algorithm>
#include <set>

using namespace std;

GLfloat LightAmbient[]= { 0.5f, 0.5f, 0.5f, 1.0f };
GLfloat LightDiffuse[]= { 0.5f, 0.5f, 0.5f, 1.0f };
GLfloat LightPosition[]= { 5.0f, 25.0f, 5.0f, 1.0f };
GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };


// init

const int row[8][3] = {{0, 1, 2}, {3, 4, 5}, {6, 7, 8}, {0, 3, 6}, {1, 4, 7}, {2, 5, 8}, {0, 4, 8}, {2, 4, 6}};
const int game_map[9][2] = {{-6,6},{0,6},{6,6},{-6,0},{0,0},{6,0},{-6,-6},{0,-6},{6,-6}};

int base_cornerMoves[4] = {0, 2, 6, 8};
int base_sideMoves[4] = {1, 3, 5, 7};
vector< int > corner_moves ( base_cornerMoves, base_cornerMoves + sizeof( base_cornerMoves) / sizeof( int ) );
vector< int > side_moves ( base_sideMoves, base_sideMoves + sizeof( base_sideMoves ) / sizeof( int ) );

int base_row_moves[8] = {0, 1, 2, 3, 4, 5, 6, 7};
int base_game_map_moves[9] = {0, 1, 2, 3, 4, 5, 6, 7, 8};
vector< int > row_moves ( base_row_moves, base_row_moves + sizeof( base_row_moves ) / sizeof ( int ) );
vector< int > game_map_moves ( base_game_map_moves, base_game_map_moves + sizeof( base_game_map_moves ) / sizeof( int ) );

int base_moves4[4] = {0, 1, 2, 3};
vector< int > moves4 ( base_moves4, base_moves4 + sizeof( base_moves4 ) / sizeof( int ) );


int game[9];

// states
int winner = 0;
int start = 0;
int pause = 0;
int credits = 0;
int winningline = 0;
int menu = 1;

// players
int computer = -1;
int player = 1;

// line
int cross;

// difficulty
int level = 2;

// moves
vector<int> gamemove(9);


set<int> forkset;
set<int> winset;


// determine the state of the game

int gameCondition(int board[9])
{
	int condition = 0;
	for ( int i = 0; i < 8; i++ )
	{
		condition = board[row[ row_moves[i] ][0]] + board[row[ row_moves[i] ][1]] + board[row[ row_moves[i] ][2]];
		if ( abs(condition) == 3 )
		{
			cross = row_moves[i];
			if ( condition == (player*3) )
			{
				return player;
			}
			else 
			{
				return computer;
			}
		}
		condition = 0;
	}

	condition = 0;
	for ( int i = 0; i < 9; i++ )
	{
		condition += abs( board[i] );
	}
	if ( condition == 9 )
	{
		return 2;
	}
	return 0;
}


// determine the move number ( 1-9 )

int getGameMove(  )
{
	return gamemove.size();
}


//
//
// GAME LOGIC BEGIN
//
//

// MEDIUM AI IMPLEMENTATION

int countForkOpportunities( int board[9], int side )
{
	forkset.clear();
	set<int> tempset;
	int forks = 0;
	int tempsetsize = 0;
	for ( int i = 0; i < 9; i++ )
	{
		if ( board[i] == 0 )
		{
			board[i] = side;
			for ( int j = 0; j < 9; j++ )
			{
				if ( board[j] == 0 )
				{
					board[j] = side;
					if ( abs( gameCondition(board) ) == 1 )
					{
						tempset.insert(cross);
					}
					board[j] = 0;
				}
			}
			tempsetsize = (int)tempset.size();
			if ( tempsetsize >= 2 )
			{	
				forkset.insert(tempset.begin(), tempset.end());
			}
			board[i] = 0;
			tempset.clear();
		}
	}
	forks = (int)forkset.size();
	if ( forks > 1 )
	{
		return forks;
	}
	return 0;
}

int gameAIWin(int board[9])
{
	int condition = 0;
	for ( int i = 0; i < 8; i++ )
	{
		condition = board[row[ row_moves[i] ][0]] + board[row[ row_moves[i] ][1]] + board[row[ row_moves[i] ][2]];
		if ( condition == (computer+computer) )
		{
			for ( int j = 0; j < 3; j++ )
			{
				if ( board[ row[ row_moves[i] ][j] ] == 0 )
				{
					game[ row[ row_moves[i] ][j] ] = computer;
					return row[ row_moves[i] ][j];
				}
			}
		}
	}
	return -1;
}

int gameAIBlock(int board[9])
{
	int condition = 0;
	for ( int i = 0; i < 8; i++ )
	{
		condition = board[row[ row_moves[i] ][0]] + board[row[ row_moves[i] ][1]] + board[row[ row_moves[i] ][2]];
		if ( condition == (player+player) )
		{
			for ( int j = 0; j < 3; j++ )
			{
				if ( board[row[ row_moves[i] ][j]] == 0 )
				{
					game[ row[ row_moves[i] ][j] ] = computer;
					return row[ row_moves[i] ][j];
				}
			}
		}
	}
	return -1;
}

int gameAICenter(int board[9])
{
	int n = getGameMove();
	if ( n == 0 )
	{
		for ( int i = 0; i < 4; i++ )
		{
			if ( board[ corner_moves[i] ] == 0 )
			{
				game[ corner_moves[i] ] = computer;
				return corner_moves[i];
			}
		}
	}
	if ( board[4] == 0 )
	{
		game[4] = computer;
		return 4;
	}
	return -1;
}

int gameAIOppositeCorner(int board[9])
{
	if ( board[0] == player )
	{
		if ( board[8] == 0 )
		{
			game[8] = computer;
			return 8;
		}
	}
	if ( board[8] == player )
	{
		if ( board[0] == 0 )
		{
			game[0] = computer;
			return 0;
		}
	}
	if ( board[2] == player )
	{
		if ( board[6] == 0 )
		{
			game[6] = computer;
			return 6;
		}
	}
	if ( board[6] == player )
	{
		if ( board[2] == 0 )
		{
			game[2] = computer;
			return 2;
		}
	}
	return -1;
}

int gameAIEmptyCorner(int board[9])
{
	for ( int i = 0; i < 4; i++ )
	{
		if ( board[ corner_moves[i] ] == 0 )
		{
			game[ corner_moves[i] ] = computer;
			return corner_moves[i];
		}
	}
	return -1;
}

int gameAIEmptySide(int board[9])
{
	for ( int i = 0; i < 4; i++ )
	{
		if ( board[ side_moves[i] ] == 0 )
		{
			game[ side_moves[i] ] = computer;
			return side_moves[i];
		}
	}
	return -1;
}

int gameAIFork(int board[9])
{
	int forkAIWin = countForkOpportunities( board, computer );
	set<int>::iterator forkit = forkset.begin();
	if ( forkAIWin == 2 )
	{
		int x0 = *forkit++;
		int x1 = *forkit;
		for ( int i = 0; i < 3; i++ )
		{
			for ( int j = 0; j < 3; j++ )
			{
				if ( row[x0][i] == row[x1][j] )
				{
					game[ row[x0][i] ] = computer;
					return row[x0][i];
				}
			}
		}
	}
	return -1;
}

int gameAIBlockFork(int board[9])
{
	int forkpos = -1;
	int forkAIWin = countForkOpportunities( board, player );
	set<int>::iterator forkit = forkset.begin();
	if ( forkAIWin == 4 )
	{
		return gameAIEmptySide(board);
	}
	else if ( forkAIWin == 3 )
	{
		set<int> testset;
		set<int>::iterator tsit;
		int same;
		for ( set<int>::iterator it = forkset.begin(); it != forkset.end(); it++ )
		{
			tsit = testset.begin();
			if ( testset.insert( tsit, *it ) == it )
				same = *it;
		}
		game[same] = computer;
		return same;		

	}
	else if ( forkAIWin == 2 )
	{
		int x0 = *forkit++;
		int x1 = *forkit;
		for ( int i = 0; i < 3; i++ )
		{
			for ( int j = 0; j < 3; j++ )
			{
				if ( row[x0][i] == row[x1][j] )
				{
					forkpos = row[x0][i];
				}
			}
		}
		int condition = 0;
		set<int> winset;
		for ( int i = 0; i < 8; i++ )
		{
			condition = abs( board[ row[i][0] ] ) + abs( board[ row[i][1] ] ) + abs( board[ row[i][2] ] );
			if ( condition == 1 )
			{
				if ( board[row[i][0]] == computer || board[row[i][1]] == computer || board[row[i][2]] == computer)
				{
					winset.insert( i );
				}
			}
		}
		set<int>::iterator it = winset.begin();
		if ( board[ row[*it][0] ] == 0 && row[*it][0] == forkpos )
		{
			game[ row[*it][0] ] = computer;
			return row[*it][0];
		}
		else if ( board[ row[*it][1] ] == 0 && row[*it][1] == forkpos )
		{
			game[ row[*it][1] ] = computer;
			return row[*it][1];
		}
		else if ( board[ row[*it][2] ] == 0 && row[*it][2] == forkpos )
		{
			game[ row[*it][2] ] = computer;
			return row[*it][2];
		}
	}
	return -1;
}

int gameMEDIUMAIMove(int board[9])
{
	int won = gameAIWin(board);
	if ( won != -1 )
	{
		return won;
	}
	int blocked = gameAIBlock(board);
	if ( blocked != -1 )
	{
		return blocked;
	}
	int forked = gameAIFork(board);
	if ( forked != -1 )
	{
		return forked;
	}
	int blockedfork = gameAIBlockFork(board);
	if ( blockedfork != -1 )
	{
		return blockedfork;
	}
	int playedcenter = gameAICenter(board);
	if ( playedcenter != -1 )
	{
		return playedcenter;
	}
	int playedoppositecorner = gameAIOppositeCorner(board);
	if ( playedoppositecorner != -1 )
	{
		return playedoppositecorner;
	}
	int playedemptycorner = gameAIEmptyCorner(board);
	if ( playedemptycorner != -1 )
	{
		return playedemptycorner;
	}
	int playedemptyside = gameAIEmptySide(board);
	if ( playedemptyside )
	{
		return playedemptyside;
	}
	for ( int i = 0; i < 9; i++ )
	{
		if ( game[i] == 0 )
		{
			game[i] = computer;
			return i;
		}
	}
	return -1;
}


// HARD AI IMPLEMENTATION

int getDistanceFromFirst()
{
	if ( gamemove[0] == 0 )
	{
		switch (gamemove[1])
		{
			case 1: case 3:
				return 1;
			case 4:
				return 2;
			case 2: case 6:
				return 3;
			case 5: case 7:
				return 4;
			case 8:
				return 5;
		}
	}
	if ( gamemove[0] == 2 )
	{
		switch (gamemove[1])
		{
			case 1: case 5:
				return 1;
			case 4:
				return 2;
			case 0: case 8:
				return 3;
			case 3: case 7:
				return 4;
			case 6:
				return 5;
		}
	}
	if ( gamemove[0] == 6 )
	{
		switch (gamemove[1])
		{
			case 3: case 7:
				return 1;
			case 4:	
				return 2;
			case 0: case 8:
				return 3;
			case 1: case 5:
				return 4;
			case 2:
				return 5;
		}
	}
	if ( gamemove[0] == 8 )
	{
		switch (gamemove[1])
		{
			case 5: case 7:
				return 1;
			case 4:
				return 2;
			case 2: case 6:
				return 3;
			case 1: case 3:
				return 4;
			case 0:
				return 5;
		}
	}
	return -1;
}

int gameAIPlayBeside()
{
	if ( gamemove[0] == 0 )
	{
		if ( game[1] == 0 && game[2] == 0 )
		{
			game[1] = computer;
			return 1;
		}
		else if ( game[3] == 0 && game[6] == 0 )
		{
			game[3] = computer;
			return 3;
		}
	}
	if ( gamemove[0] == 2 )
	{
		if ( game[1] == 0 && game[0] == 0 )
		{
			game[1] = computer;
			return 1;
		}
		else if ( game[5] == 0 && game[8] == 0 )
		{
			game[5] = computer;
			return 5;
		}
	}
	if ( gamemove[0] == 6 )
	{
		if ( game[3] == 0 && game[0] == 0 )
		{
			game[3] = computer;
			return 3;
		}
		else if ( game[7] == 0 && game[8] == 0 )
		{
			game[7] = computer;
			return 7;
		}
	}
	if ( gamemove[0] == 8 )
	{
		if ( game[5] == 0 && game[2] == 0 )
		{
			game[5] = computer;
			return 5;
		}
		else if ( game[7] == 0 && game[6] == 0 )
		{
			game[7] = computer;
			return 7;
		}
	}
}

int gameAIPlayACorner(int board[9])
{
	if ( board[0] == computer )
	{
		for ( int i = 0; i < 9; i++ )
		{
			if ( game_map_moves[i] == 2 )
			{
				game[2] = computer;
				return 2;
			}
			else if ( game_map_moves[i] == 6 )
			{
				game[6] = computer;
				return 6;
			}
		}
	}
	if ( board[2] == computer )
	{
		for ( int i = 0; i < 9; i++ )
		{
			if ( game_map_moves[i] == 0 )
			{
				game[0] = computer;
				return 0;
			}
			else if ( game_map_moves[i] == 8 )
			{
				game[8] = computer;
				return 8;
			}
		}
	}
	if ( board[8] == computer )
	{
		for ( int i = 0; i < 9; i++ )
		{
			if ( game_map_moves[i] == 2 )
			{
				game[2] = computer;
				return 2;
			}
			else if ( game_map_moves[i] == 6 )
			{
				game[6] = computer;
				return 6;
			}
		}
	}
	if ( board[6] == computer )
	{
		for ( int i = 0; i < 9; i++ )
		{
			if ( game_map_moves[i] == 2 )
			{
				game[2] = computer;
				return 2;
			}
			else if ( game_map_moves[i] == 8 )
			{
				game[8] = computer;
				return 8;
			}
		}
	}
	return -1;
}

int gameHARDAIMove(int board[9])
{
	int n = getGameMove();

	int won = gameAIWin(board);
	if ( won != -1 )
	{
		return won;
	}

	if ( gamemove.empty() )
	{
		game[0] = computer;
		return 0;
	}

	if ( (n % 2) == 0 )
	{
		int d = getDistanceFromFirst();
		if ( n == 2 )
		{
			switch ( d )
			{
				case 1: case 2:	case 3:
					return gameAIPlayBeside();
				case 4:
					return gameAICenter(board);
				case 5:
					return gameAIPlayACorner(board);
			}
		}
		int blocked = gameAIBlock(board);
		if ( blocked != -1 )
		{
			return blocked;
		}
		int forked = gameAIFork(board);
		if ( forked != -1 )
		{
			return forked;
		}
	}

	if ( n % 2 == 1 )
	{
		if ( n == 1 )
		{
			if ( gamemove[0] == 1 || gamemove[0] == 3 || gamemove[0] == 5 || gamemove[0] == 7 )
			{
				game[4] = computer;
				return 4;
			}
		}
		return gameMEDIUMAIMove(board);
	}
	for ( int i = 0; i < 9; i++ )
	{
		if ( game[i] == 0 )
		{
			game[i] = computer;
			return i;
		}
	}
	return -1;
}

int gameEASYAIMove( int board[9] )
{
	int won = gameAIWin(board);
	if ( won != -1 )
	{
		return won;
	}
	int blocked = gameAIBlock(board);
	if ( blocked != -1 )
	{
		return blocked;
	}
		int playedcenter = gameAICenter(board);
	if ( playedcenter != -1 )
	{
		return playedcenter;
	}
	int playedemptycorner = gameAIEmptyCorner(board);
	if ( playedemptycorner != -1 )
	{
		return playedemptycorner;
	}
	int playedemptyside = gameAIEmptySide(board);
	if ( playedemptyside )
	{
		return playedemptyside;
	}
}



//
//
// GAME LOGIC END
//
//



//
//
// GAME DISPLAY BEGIN
//
//

int mouse_x, mouse_y, Win_x, Win_y, object_select;
GLUquadricObj *Cylinder;


void drawBlack()
{
	glColor3f( 0.0, 0.0, 0.0);
}

void drawWhite()
{
	glColor3f( 1, 1, 1 );
}

void clearScreenBlack()
{
	glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void printText( float x, float y, string s )
{
	glRasterPos2f(x, y);
	int n = s.length();
	for ( int i = 0; i < n; i++ )
	{
		glutBitmapCharacter( GLUT_BITMAP_9_BY_15, s[i] );
	}
}

// optimized line circle algorithm from http://slabode.exofire.net/circle_draw.shtml
void drawLineCircle( float cx, float cy, float r, int num_segments ) 
{ 
	float theta = 2 * 3.1415926 / float(num_segments); 
	float tangetial_factor = tanf(theta);
	float radial_factor = cosf(theta);
	
	float x = r;
	float y = 0; 
    
	glBegin(GL_LINE_LOOP); 
	for(int ii = 0; ii < num_segments; ii++) 
	{ 
		glVertex2f(x + cx, y + cy);
		float tx = -y; 
		float ty = x; 
		x += tx * tangetial_factor; 
		y += ty * tangetial_factor; 
		x *= radial_factor; 
		y *= radial_factor; 
	} 
	glEnd(); 
}

void drawCircle( float x, float y, float z )
{
	drawWhite();
	for ( float i = 1.5; i < 2.5; i+=0.001)
	{
		drawLineCircle(x,y,i,500);
	}
}

void drawLine(float x1, float y1, float x2, float y2, float w = 0.75)
{

    float angle = atan2(y2 - y1, x2 - x1);
    float t2sina1 = w / 2 * sin(angle);
    float t2cosa1 = w / 2 * cos(angle);
    float t2sina2 = w / 2 * sin(angle);
    float t2cosa2 = w / 2 * cos(angle);

    glBegin(GL_TRIANGLES);
    glVertex2f(x1 + t2sina1, y1 - t2cosa1);
    glVertex2f(x2 + t2sina2, y2 - t2cosa2);
    glVertex2f(x2 - t2sina2, y2 + t2cosa2);
    glVertex2f(x2 - t2sina2, y2 + t2cosa2);
    glVertex2f(x1 - t2sina1, y1 + t2cosa1);
    glVertex2f(x1 + t2sina1, y1 - t2cosa1);
    glEnd();
}

void drawCross( float x, float y, float z)
{
	drawWhite();
	glPushMatrix();
	glTranslatef(x, y, z);
	glPushMatrix();
	glRotatef(0, 1, 0, 0);
	glRotatef(90, 0, 1, 0);
	glRotatef(315, 1, 0, 0);
	glTranslatef( 0, 0, -3);
	gluCylinder( Cylinder, 0.5, 0.5, 6, 16, 16);
	glPopMatrix();

	glPushMatrix();
	glRotatef(0, 1, 0, 0);
	glRotatef(90, 0, 1, 0);
	glRotatef(45, 1, 0, 0);
	glTranslatef( 0, 0, -3);
	gluCylinder( Cylinder, 0.5, 0.5, 6, 16, 16);
	glPopMatrix();
	glPopMatrix();
}

void drawGrid()
{
	drawWhite();
	for( int ix = 0; ix < 4; ix++)
	{
		glPushMatrix();
		glBegin(GL_LINES);
		glVertex2i(-9 , -9 + ix * 6);
		glVertex2i(9 , -9 + ix * 6 );
		glEnd();
		glPopMatrix();
	}
	for( int iy = 0; iy < 4; iy++ )
	{
		glPushMatrix();
		glBegin(GL_LINES);
		glVertex2i(-9 + iy * 6, 9 );
		glVertex2i(-9 + iy * 6, -9 );
		glEnd();
		glPopMatrix();
	}
}

void drawBox ( int object )
{
	float w = 0.1;
	glColor3f( 1.0, 0, 0 );

	if ( object == 0 )
	{
		drawLine( -3, 3, -3, 9, w );
		drawLine( -3, 3, -9, 3, w );
		drawLine( -9, 3, -9, 9, w );
		drawLine( -3, 9, -9, 9, w );
	}
	if ( object == 2 )
	{
		drawLine( 3, 3, 3, 9, w );
		drawLine( 3, 3, 9, 3, w );
		drawLine( 9, 3, 9, 9, w );
		drawLine( 3, 9, 9, 9, w );
	}
	if ( object == 6 )
	{
		drawLine( -3, -3, -3, -9, w );
		drawLine( -3, -3, -9, -3, w );
		drawLine( -9, -3, -9, -9, w );
		drawLine( -3, -9, -9, -9, w );
	}
	if ( object == 8 )
	{
		drawLine( 3, -3, 3, -9, w );
		drawLine( 3, -3, 9, -3, w );
		drawLine( 9, -3, 9, -9, w );
		drawLine( 3, -9, 9, -9, w );
	}
	if ( object == 7 )
	{
		drawLine( -3, -3, 3, -3, w );
		drawLine( -3, -3, -3, -9, w );
		drawLine( -3, -9, 3, -9, w );
		drawLine( 3, -3, 3, -9, w );
	}
}

//
//
// GAME DISPLAY END
//
//


//
//
// GAME BASE BEGIN
//
//

// init

void gameSetup()
{
	glClearColor (0.0, 0.0, 0.0, 0.0);
	glShadeModel (GL_SMOOTH); 
	glDisable(GL_DEPTH_TEST);
	glLightfv(GL_LIGHT1 ,GL_AMBIENT, LightAmbient);
	glLightfv(GL_LIGHT1 ,GL_DIFFUSE, LightDiffuse);
	glLightfv(GL_LIGHT1 ,GL_POSITION, LightPosition);
	glDisable(GL_LIGHTING);
	glEnable(GL_LIGHT1);

	start = 0;
	winner = 0;

	Cylinder = gluNewQuadric();
	gluQuadricDrawStyle( Cylinder, GLU_FILL );
	gluQuadricNormals( Cylinder, GLU_SMOOTH );
	gluQuadricOrientation( Cylinder, GLU_OUTSIDE );
}

// resize screen

void reshape (int w, int h)
{
   Win_x = w;
   Win_y = h;
   glViewport (0, 0, (GLsizei) w, (GLsizei) h);
   glMatrixMode (GL_PROJECTION);
   glLoadIdentity ();
}

// display game and grid

void display()
{
	clearScreenBlack();
	glMatrixMode (GL_PROJECTION);  // Tell opengl that we are doing project matrix work
	glLoadIdentity();  // Clear the matrix
	glOrtho(-9.0, 9.0, -9.0, 9.0, 0.0, 30.0);  // Setup an Ortho view
	glMatrixMode(GL_MODELVIEW);  // Tell opengl that we are doing model matrix work. (drawing)
	glLoadIdentity(); // Clear the model matrix

	glDisable(GL_COLOR_MATERIAL);
	glDisable(GL_LIGHTING);
	
	if ( start == 1 && pause == 0 && winningline == 0 && credits == 0 && menu == 0 )
	{
		clearScreenBlack();
		drawWhite();
		drawGrid();

		gluLookAt( 0, 0, 20, 0, 0, 0, 0, 1, 0);
	
		for ( int i = 0; i < 9; i++ )
		{
			if ( game[i] == 1 )
			{
				drawCircle( game_map[i][0], game_map[i][1], -1 );
			}
			if ( game[i] == -1 )
			{
				drawCross( game_map[i][0], game_map[i][1], -1 );
			}
		}
	}
	

	if ( start == 0 && pause == 0 && winningline == 1 && credits == 0 && menu == 0)
	{
		clearScreenBlack();
		drawWhite();
		drawGrid();

		gluLookAt( 0, 0, 20, 0, 0, 0, 0, 1, 0);
	
		for ( int i = 0; i < 9; i++ )
		{
			if ( game[i] == 1 )
			{
				drawCircle( game_map[i][0], game_map[i][1], -1 );
			}
			if ( game[i] == -1 )
			{
				drawCross( game_map[i][0], game_map[i][1], -1 );
			}
		}

		
		if ( winner == computer )
		{
			glColor3f(1.0, 0.0, 0.0);
		}
		else if ( winner == player )
		{
			glColor3f(0, 0, 1);
		}
		drawLine( game_map[ row[cross][0] ][0], game_map[ row[cross][0] ][1], game_map[ row[cross][2] ][0], game_map[ row[cross][2] ][1], 0.8 );

	}


	if ( start == 0 && pause == 0 && winningline == 0 && credits == 1 && menu == 0)
	{
		clearScreenBlack();
		if ( winner == player ) 
		{
			glColor3f(0, 0.0, 1.0);
			printText( -0.5, 1, "You win");
		}
		else if ( winner == computer )
		{
			glColor3f(1.0, 0.0, 0.0);
			printText( -1.2, 1, "Computer win");
		}
		else if ( winner == 2 ) 
		{
			glColor3f(1, 1.0, 0.0);
			printText( -0.2, 1, "Tie");
		}
	}


	if ( start == 0 && pause == 0 && winningline == 0 && credits == 0 && menu == 1 )
	{
		clearScreenBlack();
		
		drawWhite();

		printText( -0.5, 0, "Play");


		drawCross( game_map[0][0], game_map[0][1], -1 );
		drawCircle( game_map[2][0], game_map[2][1], -1 );

		drawWhite();

		if ( player == -1 )
		{
			drawBox(0);
		}
		else if ( player == 1 )
		{
			drawBox(2);
		}

		if ( level == 3 )
		{
			drawBox(8);
		}
		else if ( level == 2 )
		{
			drawBox(7);
		}
		else if ( level == 1 )
		{
			drawBox(6);
		}

		drawWhite();

		drawLine( -6, -5, -6, -7, 0.25 );

		drawLine( -0.5, -5, -0.5, -7, 0.25 );
		drawLine( 0.5, -5, 0.5, -7, 0.25 );

		drawLine( 5, -5, 5, -7, 0.25 );
		drawLine( 6, -5, 6, -7, 0.25 );
		drawLine( 7, -5, 7, -7, 0.25 );


	}

	glutSwapBuffers();
}

// take keyboard input (not implemented except for quit)

void keyboard( unsigned char key, int x, int y )
{
	switch( key )
	{
		case 27:
			exit(0);
			break;
		case 'p':
		case 'P':
			if ( start == 1 )
			{
				if ( pause == 0 )
				{
					start = 0;
					pause = 1;
				}
				else if ( pause == 1 )
				{
					start = 1;
					pause = 0;
				}
				cout << "Pause" << endl;
			}
		default:
			break;
	}
}

// game clear

void startSetup()
{
	gamemove.clear();

	random_shuffle(game_map_moves.begin(), game_map_moves.end() );
	random_shuffle(row_moves.begin(), row_moves.end() );
	random_shuffle(corner_moves.begin(), corner_moves.end() );
	random_shuffle(side_moves.begin(), side_moves.end() );
	random_shuffle(moves4.begin(), moves4.end() );
	
	for ( int i = 0; i < 9; i++ )
	{
		game[i] = 0;
	}

	start = 1;
	menu = 0;
}

// take mouse input

void mouse( int button, int state, int x, int y )
{
	mouse_x =  (18 * (float) ((float)x/(float)Win_x))/6;
	mouse_y =  (18 * (float) ((float)y/(float)Win_y))/6;

	object_select = mouse_x + mouse_y * 3;


	if (  start == 1 && pause == 0 && winningline == 0 && credits == 0 && menu == 0 )
	{
		if ((button == GLUT_LEFT_BUTTON) && (state == GLUT_DOWN))
		{
			if ( game[ object_select ] == 0 )
			{
				gamemove.push_back(object_select);
				game[ object_select ] = player;
				winner = gameCondition( game );
				if ( winner == player )
				{
					start = 0;
					winningline = 1;
					return;
				}
				if ( level == 3 )
				{
					gamemove.push_back(gameHARDAIMove(game));
				}
				else if ( level == 2 )
				{
					gamemove.push_back(gameMEDIUMAIMove(game));
				}
				else if ( level == 1 )
				{
					gamemove.push_back(gameEASYAIMove(game));
				}
				winner = gameCondition( game );
				if ( winner == computer )
				{
					start = 0;
					winningline = 1;
					return;
				}
				else if ( winner == 2 )
				{
					start = 0;
					credits = 1;
					return;
				}
			}
		}
	}
	

	if (  start == 0 && pause == 0 && winningline == 1 && credits == 0 && menu == 0)
	{
		if ((button == GLUT_LEFT_BUTTON) && (state == GLUT_DOWN))
		{
			winningline = 0;
			credits = 1;
			return;
		}
	}


	if (  start == 0 && pause == 0 && winningline == 0 && credits == 1 && menu == 0 )
	{
		if ((button == GLUT_LEFT_BUTTON) && (state == GLUT_DOWN))
		{
			credits = 0;
			menu = 1;
			return;
		}
	}


	if (  start == 0 && pause == 0 && winningline == 0 && credits == 0 && menu == 1)
	{
		if ((button == GLUT_LEFT_BUTTON) && (state == GLUT_DOWN))
		{
			if ( object_select == 4 )
			{
				if ( player == 1 )
				{
					startSetup();
					if ( level == 3 )
					{
						gamemove.push_back(gameHARDAIMove(game));
					}
					else if ( level == 2 )
					{
						gamemove.push_back(gameMEDIUMAIMove(game));
					}
					else if ( level == 1 )
					{
						gamemove.push_back(gameEASYAIMove(game));
					}
					return;
				}
				else if ( player == -1 )
				{
					startSetup();
					return;
				}
			}

			if ( object_select == 2 )
			{
				player = 1;
				computer = -1;
				return;
			}

			if ( object_select == 0 )
			{
				player = -1;
				computer = 1;
				return;
			}

			if ( object_select == 8 )
			{
				level = 3;
				return;
			}
			
			if ( object_select == 7 )
			{
				level = 2;
				return;
			}

			if ( object_select == 6 )
			{
				level = 1;
				return;
			}
		}
	}
}

// main 

int main(int argc, char** argv)
{
   glutInit(&argc, argv);
   glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
   glutInitWindowSize (500, 500);
   glutInitWindowPosition (10, 10);
   glutCreateWindow (argv[0]);
   glutSetWindowTitle("TIC TAC TOE");
   
   
   gameSetup();
   glutDisplayFunc(display);
   glutReshapeFunc(reshape);
   glutKeyboardFunc(keyboard);
   glutMouseFunc(mouse);
   glutMainLoop();
  
   return 0;
}


//
//
// GAME BASE END
//
//