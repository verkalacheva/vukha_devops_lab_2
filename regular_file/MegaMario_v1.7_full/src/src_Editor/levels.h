
#ifndef __LEVELS_H__
#define __LEVELS_H__


class cScene
{
   public:
	cScene();
	/*LOADINGS::*/
	void loadLevel(char* scenename);
	void getParts(char *Text);
	void interpreteSceneLine();

	void saveLevel();

	/*LOADINGS::*/
	/*EXCECUTATION::*/
	void drawLevel();

	void unloadLevel();

	void changeLevel();
	void restartLevel();


	/*EXCECUTATION::*/

	int partcount;
	char **parts;
	ifstream ifs;
	bool tileread;
	bool bonusread;
	bool coinread;
	float COINcount;

	int BGCOLOR;

	//!COUNTER
	int TEXTUREcount;
	//COUNTER!

	#define MAX_X 400
	#define MAX_Y 100
	int TILE[MAX_X][MAX_Y];


	int ENEMY[MAX_X][MAX_Y];

	bool COIN[MAX_X][MAX_Y];
	
	int BONUS[MAX_X][MAX_Y];

	#define MAX_TEXTURES 100
	#define MAX_BONUS_TEXTURES 20
	SDL_Surface* Textures[MAX_TEXTURES];
	SDL_Surface* Bonus_Textures[MAX_BONUS_TEXTURES];
	SDL_Surface* Enemy_Textures[MAX_BONUS_TEXTURES];

		struct START_POINT
	{
		int x,y;
		int type;
	}STARTPOINTS[5],_BOWSER;
	struct EXIT_POINT
	{
		int x,y;
		char* level;
		int type;
		int startpoint;
	}EXITPOINTS[5];
	struct CASTELSTR
	{
		bool available;
		int x,y;
		bool big;
		char* level;
		int startpoint;
		SDL_Surface* small_s;
		SDL_Surface* big_s;
	}CASTLE;

	char* MUSIC,*BACK;
	
	SDL_Rect SCREEN_RECT;
};

#endif
