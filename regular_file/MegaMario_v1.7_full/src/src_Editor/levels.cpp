#include "stdafx.h"

int i;


cScene::cScene()
{
	this->parts = NULL;

	BGCOLOR = 0;

	SCREEN_RECT.w = WIDTH;
	SCREEN_RECT.h = HEIGHT;
	SCREEN_RECT.x = 0;
	SCREEN_RECT.y = 0;

	TEXTUREcount = 1;
	Enemy_Textures[GOOMBA]     = LoadIMG("data/gfx/characters/goomba1.png");
	Enemy_Textures[TURTLE]     = LoadIMG("data/gfx/characters/turtle/turtler1.png");
	Enemy_Textures[SPINY]      = LoadIMG("data/gfx/characters/spiny1.png");
	Enemy_Textures[FIRECIRCLE1]= LoadIMG("data/gfx/editor/firecircle1");
	Enemy_Textures[FIRECIRCLE2]= LoadIMG("data/gfx/editor/firecircle2");
	Enemy_Textures[FIRECIRCLE3]= LoadIMG("data/gfx/editor/firecircle3");
	Enemy_Textures[FIRE]       = LoadIMG("data/gfx/editor/fire");
	Enemy_Textures[CANNON]     = LoadIMG("data/gfx/characters/cannon.PNG");
	Enemy_Textures[BLUME]      = LoadIMG("data/gfx/characters/pirnaha1.PNG");
	Enemy_Textures[BOWSER]     = LoadIMG("data/gfx/editor/bowser");
	Enemy_Textures[TURTLEFLY]  = LoadIMG("data/gfx/characters/turtle/flyl1.png");
	Enemy_Textures[BRETT]     = LoadIMG("data/gfx/editor/brett");

	Enemy_Textures[0]= LoadIMG("data/gfx/characters/big/player1r.png");
	Enemy_Textures[19]= LoadIMG("data/gfx/editor/exit");

	CASTLE.big_s = LoadIMG("data/gfx/tiles/bigcastle.PNG");
	CASTLE.small_s = LoadIMG("data/gfx/tiles/smallcastle.PNG");
	
	COINcount = 10;
	for(i=0;i<MAX_X;i++)
		for(int n=0;n<MAX_Y;n++)
		{
			TILE[i][n] = 0;
			BONUS[i][n] = 0;
			ENEMY[i][n] = 0;
			COIN[i][n] = 0;
		}

	for(i=0;i<MAX_TEXTURES;i++)
		Textures[i] = 0;
	
	Textures[MAX_TEXTURES-1] = LoadIMG("data/gfx/editor/mover");
	Textures[MAX_TEXTURES-2] = LoadIMG("data/gfx/background/bush1.png");
	Textures[MAX_TEXTURES-3] = LoadIMG("data/gfx/background/bush2.png");
	Textures[MAX_TEXTURES-4] = LoadIMG("data/gfx/background/bush3.png");
	Textures[MAX_TEXTURES-5] = LoadIMG("data/gfx/background/bush4.png");
	Textures[MAX_TEXTURES-6] = LoadIMG("data/gfx/background/bush5.png");
	Textures[MAX_TEXTURES-7] = LoadIMG("data/gfx/background/bush6.png");
	Textures[MAX_TEXTURES-8] = LoadIMG("data/gfx/background/cloud1.png");
	Textures[MAX_TEXTURES-9] = LoadIMG("data/gfx/background/cloud2.png");
}

void cScene::loadLevel(char* scenename)
{
	CASTLE.available = 0;
	CASTLE.x = 0;
	CASTLE.y = 0;
	CASTLE.big = 0;
	CASTLE.level = new char[2];
	CASTLE.startpoint = 0;

	m_TextureCtrl.ResetContent();
	tileread = 0;
	bonusread = 0;
	TEXTUREcount = 1;
	parts = 0;	
	MUSIC = 0;
	BACK = 0;

	for(i=0;i<5;i++)
	{
		STARTPOINTS[i].x=0;
		STARTPOINTS[i].y =0;
		STARTPOINTS[i].type =0;

		EXITPOINTS[i].x =0;
		EXITPOINTS[i].y =0;		
		EXITPOINTS[i].level = new char[5];		
		strcpy(EXITPOINTS[i].level,"NONE");
		EXITPOINTS[i].startpoint =0;
	}

	ifstream ifs(scenename, ios::in);

	char *contents = new char[670];

	for (; ifs.getline(contents, 670);)
		if ( contents[0] != '#' && strlen(contents) > 2)
		{		
			getParts(contents);
			interpreteSceneLine();
			
		}
	ifs.close();
	delete[](contents);
}

void cScene::interpreteSceneLine()
{
	if (!strcmp (parts[0], "TEXTURE"))
	{
		Textures[TEXTUREcount] = LoadIMG(parts[1]);
		m_TextureCtrl.AddString(parts[1]);
		m_TextureCtrl.SetCurSel(TEXTUREcount-1);
		TEXTUREcount++;	
	}
	else if (!strcmp (parts[0], "::TILES->"))
	{
		if (!strcmp (parts[1], "BEGIN"))
			tileread = 1;
		else
			tileread = 0;
	}
	else if (!strcmp (parts[0], "::BONUS->"))
	{
		if (!strcmp (parts[1], "BEGIN"))
			bonusread = 1;
		else
			bonusread = 0;
	}
	else if (!strcmp (parts[0], "::COIN->"))
	{
		if (!strcmp (parts[1], "BEGIN"))
			coinread = 1;
		else
			coinread = 0;
	}
	else if (tileread)
	{
		TILE[atoi(parts[0])][atoi(parts[1])] = atoi(parts[2]);
	}
	else if (bonusread)
	{
		BONUS[atoi(parts[0])][atoi(parts[1])] = atoi(parts[2]);
	}
	else if (coinread)
	{
		COIN[atoi(parts[0])][atoi(parts[1])] = 1;
	}
	else if (!strcmp (parts[0], "BGCOLOR"))
		BGCOLOR = atoi(parts[1]);
	else if (!strcmp (parts[0], "EXIT"))
	{
		EXITPOINTS[atoi(parts[1])].x = atoi(parts[2]);
		EXITPOINTS[atoi(parts[1])].y = atoi(parts[3]);
		EXITPOINTS[atoi(parts[1])].startpoint = atoi(parts[5]);
		EXITPOINTS[atoi(parts[1])].type = atoi(parts[6]);
		EXITPOINTS[atoi(parts[1])].level = new char[100];
		strcpy(EXITPOINTS[atoi(parts[1])].level,parts[4]);
	}
	else if (!strcmp (parts[0], "START"))
	{
		STARTPOINTS[atoi(parts[1])].x = atoi(parts[2]);
		STARTPOINTS[atoi(parts[1])].y = atoi(parts[3]);
		STARTPOINTS[atoi(parts[1])].type = atoi(parts[4]);
	}
	else if (!strcmp (parts[0], "GOOMBA"))
	{
		ENEMY[atoi(parts[1])][atoi(parts[2])] = GOOMBA;
	}
	else if (!strcmp (parts[0], "TURTLE"))
	{
		ENEMY[atoi(parts[1])][atoi(parts[2])] = TURTLE;
	}
	else if (!strcmp (parts[0], "TURTLEFLY"))
	{
		ENEMY[atoi(parts[1])][atoi(parts[2])] = TURTLEFLY;
	}
	else if (!strcmp (parts[0], "SPINY"))
	{
		ENEMY[atoi(parts[1])][atoi(parts[2])] = SPINY;
	}
	else if (!strcmp (parts[0], "FIRECIRCLE"))
	{
		if(atoi(parts[3])<=6)
			ENEMY[atoi(parts[1])][atoi(parts[2])] = FIRECIRCLE1;
		else if(atoi(parts[3])<=10)
			ENEMY[atoi(parts[1])][atoi(parts[2])] = FIRECIRCLE2;
		else
			ENEMY[atoi(parts[1])][atoi(parts[2])] = FIRECIRCLE3;
	}
	else if (!strcmp (parts[0], "FIRE"))
	{
		ENEMY[atoi(parts[1])][atoi(parts[2])] = FIRE;
	}
	else if (!strcmp (parts[0], "KILLERBLUME"))
	{
		ENEMY[atoi(parts[1])][atoi(parts[2])] = BLUME;
	}
	else if (!strcmp (parts[0], "CANNON"))
	{
		ENEMY[atoi(parts[1])][atoi(parts[2])] = CANNON;
	}
	else if (!strcmp (parts[0], "BRETT"))
	{
		ENEMY[atoi(parts[1])/40][atoi(parts[2])/40] = BRETT;
	}
	else if (!strcmp (parts[0], "CASTLE"))
	{
		CASTLE.x = atoi(parts[1]);
		CASTLE.y = atoi(parts[2]);
		CASTLE.big = atoi(parts[3]);
		CASTLE.level = new char[100];
		strcpy(CASTLE.level,parts[4]);
		CASTLE.startpoint = atoi(parts[5]);
		CASTLE.available = 1;
	}
	else if (!strcmp (parts[0], "BOWSER"))
	{
		_BOWSER.x = atoi(parts[1]);
		_BOWSER.y = atoi(parts[2]);
		m_BOWSER_AV = 1;
		m_HAMMERS = atoi(parts[3]);
	}
	else if (!strcmp (parts[0], "RESTART"))
	{
		m_RESTARTLEVEL = 1;
	}
	else if (!strcmp (parts[0], "MUSIC"))
	{
		MUSIC = new char[100];
		strcpy(MUSIC,parts[1]);
	}
	else if (!strcmp (parts[0], "BACKGROUND"))
	{
		BACK = new char[100];
		strcpy(BACK,parts[1]);
	}
}

void cScene::getParts(char *Text)
{
	
	delete[] parts;

	char *str = Text;
	partcount = 0;
	int len = 0, i = 0;

	
	str += strspn(str," ");
	
	while (*str)
	{
		partcount++;
		str += strcspn(str," ");
		str += strspn(str," ");
	}
	
	str = Text;
	
	parts = new char*[partcount];

	str += strspn(str," ");
	
	while (*str)
	{
		int len = (int)(strcspn(str," "));
		parts[i] = new char[len + 2];
		memcpy(parts[i],str,len);
		parts[i][len] = 0;
		str += len + strspn(str+len," ");
		i++; 
	}
}

void cScene::unloadLevel()
{
	for(i=0;i<TEXTUREcount;i++)
		SDL_FreeSurface(Textures[i]);
	
	TEXTUREcount = 1;
	
	for(i=0;i<MAX_X;i++)
		for(int n=0;n<MAX_Y;n++)
		{
			ENEMY[i][n] = 0;
			TILE[i][n] = 0;
			BONUS[i][n] = 0;
			COIN[i][n] = 0;
		}
	CASTLE.available = 0;
	m_BOWSER_AV = 0;
	
}


SDL_Rect tmp_rect;
int posx,posy;
void cScene::drawLevel()
{
	tmp_rect.y = -cam_y%600+500;
	tmp_rect.x = -cam_x%800+700;
	SDL_UpperBlit(Textures[MAX_TEXTURES-1],0,screen,&tmp_rect);

	posx = cam_x/40;
	posy = cam_y/40;
	COINcount+=0.1f;
	if(COINcount>13)
		COINcount=10;
	for(i = posx;i<posx+25;i++)
	{
		for(int n = posy;n<posy+21;n++)
		{
			if(COIN[i][n])
			{
				tmp_rect.x = (i*40) - cam_x;
				tmp_rect.y = (n*40) - cam_y;
				SDL_UpperBlit(Bonus_Textures[(int)COINcount],0,screen,&tmp_rect);

			}
			if(TILE[i][n])
			{
				tmp_rect.x = (i*40) - cam_x;
				tmp_rect.y = (n*40) - cam_y;
				SDL_UpperBlit(Textures[TILE[i][n]],0,screen,&tmp_rect);
			}
			else if(BONUS[i][n])
			{
				tmp_rect.x = (i*40) - cam_x;
				tmp_rect.y = (n*40) - cam_y;
				SDL_UpperBlit(Bonus_Textures[BONUS[i][n]],0,screen,&tmp_rect);
			}
			if(ENEMY[i][n])
			{
				tmp_rect.x = (i*40) - cam_x;
				tmp_rect.y = (n*40) - cam_y;
				SDL_UpperBlit(Enemy_Textures[ENEMY[i][n]],0,screen,&tmp_rect);

			}
	
		}	
	}
	if(m_BOWSER_AV)
	{
		tmp_rect.x = _BOWSER.x*40 - cam_x;
		tmp_rect.y = _BOWSER.y*40 - cam_y;
		SDL_UpperBlit(Enemy_Textures[BOWSER],0,screen,&tmp_rect);
	}
	if(CASTLE.available)
	{
		tmp_rect.x = CASTLE.x - cam_x;
		tmp_rect.y = CASTLE.y - cam_y;
		if(CASTLE.big)
			SDL_UpperBlit(CASTLE.big_s,0,screen,&tmp_rect);
		else 
			SDL_UpperBlit(CASTLE.small_s,0,screen,&tmp_rect);
	}
	for(i=0;i<5;i++)
	{
		char* c = new char[2];
		sprintf(c,"%d",i);

		tmp_rect.x = STARTPOINTS[i].x - cam_x;
		tmp_rect.y = STARTPOINTS[i].y - cam_y;
		SDL_UpperBlit(Enemy_Textures[0],0,screen,&tmp_rect);
		StringColor(screen,tmp_rect.x,tmp_rect.y,c,1440);

		tmp_rect.x = EXITPOINTS[i].x - cam_x;
		tmp_rect.y = EXITPOINTS[i].y - cam_y;
		SDL_UpperBlit(Enemy_Textures[19],0,screen,&tmp_rect);
		StringColor(screen,tmp_rect.x+22,tmp_rect.y,c,1450);
		delete[] (c);
	}

}





void cScene::saveLevel()
{
	ofstream out(Levelname);
	
	out<<"# Level for Mega Mario"<<endl<<endl;

	CString PATH_FILE = _T("");

	for(i=0;i<m_TextureCtrl.GetCount();i++)
	{
		m_TextureCtrl.GetText(i,PATH_FILE);
		out<<"TEXTURE "<<PATH_FILE<<endl;
	}
	
	out<<"::TILES-> BEGIN"<<endl;
	for(i=0;i<MAX_X;i++)
		for(int n=0;n<MAX_Y;n++)
		{
			if(TILE[i][n])
				out<<i<<" "<<n<<" "<<TILE[i][n]<<endl;
		
		}
	out<<"::TILES-> END"<<endl;
	out<<"::BONUS-> BEGIN"<<endl;
	for(i=0;i<MAX_X;i++)
		for(int n=0;n<MAX_Y;n++)
		{
			if(BONUS[i][n])
				out<<i<<" "<<n<<" "<<BONUS[i][n]<<endl;
		
		}
	out<<"::BONUS-> END"<<endl;
	out<<"::COIN-> BEGIN"<<endl;
	for(i=0;i<MAX_X;i++)
		for(int n=0;n<MAX_Y;n++)
		{
			if(COIN[i][n])
				out<<i<<" "<<n<<endl;
		
		}
	out<<"::COIN-> END"<<endl;

	for(i=0;i<MAX_X;i++)
		for(int n=0;n<MAX_Y;n++)
		{
			switch(ENEMY[i][n])
			{
				case GOOMBA:
					out<<"GOOMBA "<<i<<" "<<n<<endl;
				break;
				case TURTLE:
					out<<"TURTLE "<<i<<" "<<n<<endl;
				break;
				case SPINY:
					out<<"SPINY "<<i<<" "<<n<<endl;
				break;
				case FIRE:
					out<<"FIRE "<<i<<" "<<n<<endl;
				break;
				case FIRECIRCLE1:
					out<<"FIRECIRCLE "<<i<<" "<<n<<" "<<6<<endl;
				break;
				case FIRECIRCLE2:
					out<<"FIRECIRCLE "<<i<<" "<<n<<" "<<8<<endl;
				break;
				case FIRECIRCLE3:
					out<<"FIRECIRCLE "<<i<<" "<<n<<" "<<16<<endl;
				break;
				case CANNON:
					out<<"CANNON "<<i<<" "<<n<<endl;
				break;
				case BLUME:
					out<<"KILLERBLUME "<<i<<" "<<n<<endl;
				break;
				case TURTLEFLY:
					out<<"TURTLEFLY "<<i<<" "<<n<<endl;
				break;
				case BRETT:
					out<<"BRETT "<<i*40<<" "<<n*40<<endl;
				break;

			}		
		}	
	if(m_RESTARTLEVEL)
		out<<"RESTART "<<Levelname<<endl;

	for(i=0;i<5;i++)
	{
		out<<"START "<<i<<" "<<STARTPOINTS[i].x<<" "<<STARTPOINTS[i].y<<" "<<STARTPOINTS[i].type<<endl;
		out<<"EXIT "<<i<<" "<<EXITPOINTS[i].x<<" "<<EXITPOINTS[i].y<<" "<<EXITPOINTS[i].level<<" "<<EXITPOINTS[i].startpoint<<" "<<EXITPOINTS[i].type<<endl;
	}
	out<<"BGCOLOR "<<BGCOLOR<<endl;
	out<<endl;

	if(m_BOWSER_AV)
	{
		out<<"BOWSER "<<_BOWSER.x<<" "<<_BOWSER.y<<" "<<m_HAMMERS<<endl;
	}
	if(CASTLE.available)
	{
		out<<"CASTLE "<<CASTLE.x<<" "<<CASTLE.y<<" "<<CASTLE.big<<" "<<CASTLE.level<<" "<<CASTLE.startpoint<<endl;;
	}

	if(MUSIC)
		out<<"MUSIC "<<MUSIC<<endl;
	else
		out<<"#If you want Background music:\n#MUSIC megamusic.mid"<<endl;

	if(BACK)
		out<<"BACKGROUND "<<BACK<<endl;
	else
		out<<"#if you want a big static background:\n#BACKGROUND data/gfx/background/VH.jpg\n"<<endl;
	

}



