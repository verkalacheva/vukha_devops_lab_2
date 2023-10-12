// stdafx.cpp : Quelltextdatei, die nur die Standard-Includes einbindet
//	Editor.pch ist die vorcompilierte Header-Datei
//	stdafx.obj enthält die vorcompilierte Typinformation

#include "stdafx.h"




SDL_Surface *screen;

SDL_Event event;

Uint8 *keys;

Uint32 magenta;
int cam_x=50,cam_y=3200;

int numbor=0,m_HAMMERS=5;
	BOOL	m_BOWSER_AV=0,m_RESTARTLEVEL=0;

cScene *LEVEL;
CString	m_Filename,m_PATH;
char* FileToLoad,*Levelname;
CListBox	m_TextureCtrl;

void loadGlobalSprites()
{
	LEVEL->Bonus_Textures[1] = LoadIMG("data/gfx/bonus/crackstone.png");
	LEVEL->Bonus_Textures[2] = LoadIMG("data/gfx/bonus/box.png");
	LEVEL->Bonus_Textures[3] = LoadIMG("data/gfx/bonus/box.png");
	LEVEL->Bonus_Textures[4] = LoadIMG("data/gfx/bonus/box.png");
	LEVEL->Bonus_Textures[5] = LoadIMG("data/gfx/bonus/stern.png");
	LEVEL->Bonus_Textures[6] = LoadIMG("data/gfx/bonus/pilz.png");
	LEVEL->Bonus_Textures[7] = LoadIMG("data/gfx/bonus/boxempty.png");
	LEVEL->Bonus_Textures[8] = LoadIMG("data/gfx/bonus/invis.png");
	LEVEL->Bonus_Textures[9] = LoadIMG("data/gfx/bonus/life.png");
	LEVEL->Bonus_Textures[10] = LoadIMG("data/gfx/bonus/COIN1.png");
	LEVEL->Bonus_Textures[11] = LoadIMG("data/gfx/bonus/COIN2.png");
	LEVEL->Bonus_Textures[12] = LoadIMG("data/gfx/bonus/COIN3.png");
	LEVEL->Bonus_Textures[13] = LoadIMG("data/gfx/bonus/COIN4.png");
	LEVEL->Bonus_Textures[14] = LoadIMG("data/gfx/bonus/crackstone2.png");
	LEVEL->Bonus_Textures[15] = LoadIMG("data/gfx/bonus/box.png");
	SDL_SetAlpha(LEVEL->Bonus_Textures[15] , SDL_RLEACCEL | SDL_SRCALPHA , 80);
	LEVEL->Bonus_Textures[16] = LoadIMG("data/gfx/bonus/box.png");
}
void unloadGlobalSprites()
{
	for(int i=1;i<13;i++)
		SDL_FreeSurface(LEVEL->Bonus_Textures[i]);
}
