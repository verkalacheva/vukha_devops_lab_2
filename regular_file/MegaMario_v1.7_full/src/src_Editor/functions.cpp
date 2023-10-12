#include "StdAfx.h"

void PutPixel(SDL_Surface *pSurf, int x, int y, Uint32 color)
{
	int bpp;
	Uint8 *p;

	if (x < 0 || x >= pSurf->w || // Prüfe, ob das Pixel innerhalb der Surface liegt
	    y < 0 || y >= pSurf->h)
		return;

	bpp = pSurf->format->BytesPerPixel; // Berechne den Zielpointer
	p = (Uint8 *)pSurf->pixels + y*pSurf->pitch + x*bpp;

	switch(bpp) // Setze den Farbwert an der Stelle
	{
	case 1:
		*p = color;
		break;

	case 2:
		*(Uint16 *)p = color;
		break;

	case 4:
		*(Uint32 *)p = color;
		break;

	case 3:
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
		p[0] = (color >> 16) & 0xff;
		p[1] = (color >> 8) & 0xff;
		p[2] = color & 0xff;
#else
		p[0] = color & 0xff;
		p[1] = (color >> 8) & 0xff;
		p[2] = (color >> 16) & 0xff;
#endif
		break;
	}
}



SDL_Surface *LoadIMG(const char *filename)
{
	SDL_Surface *image,*temp=0;


	temp = IMG_Load(filename);
    if(temp == NULL)
    {        
		MessageBox(0,filename,"Es fehlt :",0);
		temp = IMG_Load("data/gfx/editor/error");
		if(temp == NULL)
			return 0;
    }

	image = SDL_DisplayFormat(temp);
	SDL_FreeSurface(temp);
	SDL_SetColorKey(image, SDL_SRCCOLORKEY | SDL_RLEACCEL, magenta);
	return image;
}


bool collision(SDL_Rect *r1, SDL_Rect *r2)
{
	if (r1->x + r1->w <= r2->x)
		return false;
	if (r1->x >= r2->x + r2->w)
		return false;
	if (r1->y + r1->h <= r2->y)
		return false;
	if (r1->y >= r2->y + r2->h)
		return false;
	return true;
}
bool pointCollision(SDL_Rect *r1, int x, int y)
{
	if(x<r1->x)
	return false;
	if(x>r1->x+r1->w)
	return false;
	if(y<r1->y)
	return false;
	if(y>r1->y+r1->h)
	return false;
	
	return true;
}





OPENFILENAME musicFile;
char FileName [256];
//  0 = LOADING &&   1 = SAVING
char* loadFile(int type)

{	
	if(type == 0)
	{
		musicFile.lStructSize       = sizeof(OPENFILENAME);
		musicFile.lpstrFilter       = "Level data(*.rok)\0*.rok\0All Data(*.*)\0*.*\0\0"; 
		musicFile.lpstrCustomFilter = NULL;
		musicFile.nFilterIndex      = 1;
		musicFile.lpstrFile         = NULL;
		musicFile.lpstrFile         = FileName;
		musicFile.nMaxFile          = sizeof(FileName);
		musicFile.lpstrFileTitle    = NULL;
		musicFile.lpstrInitialDir   = NULL;
		musicFile.lpstrTitle        = "Select your text file to open !";  
		musicFile.Flags             = OFN_HIDEREADONLY | OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST;
		musicFile.lpstrDefExt       = "*.rok";  
		*FileName = '\0';
	}
	else if(type == 1)
	{
		musicFile.lStructSize       = sizeof(OPENFILENAME);
		musicFile.lpstrFilter       = "Level data(*.rok)\0*.rok\0All Data(*.*)\0*.*\0\0"; 
		musicFile.lpstrCustomFilter = NULL;
		musicFile.nFilterIndex      = 1;
		musicFile.lpstrFile         = NULL;
		musicFile.lpstrFile         = FileName;
		musicFile.nMaxFile          = sizeof(FileName);
		musicFile.lpstrFileTitle    = NULL;
		musicFile.lpstrInitialDir   = NULL;
		musicFile.lpstrTitle        = "Select your text file to open !";  
		musicFile.Flags             = OFN_HIDEREADONLY | OFN_PATHMUSTEXIST;
		musicFile.lpstrDefExt       = "*.rok";  
		*FileName = '\0';
	}
	else return "-";
		if (!GetOpenFileName(&musicFile))
		{
			return "-"; 
		}
		return FileName; 
}

//Shorts a Path to the form : "data/..."
char* shortPath(char* bla)
{	
	if(bla[0]=='-')
		return "-";
	
	int i;
	for(i=0;i<(signed)strlen(bla);i++)
	{
		if(  bla[i] == 'd')
			if(bla[i+1] == 'a')
				if(bla[i+2] == 't')
					if(bla[i+3] == 'a')
						break;
	}
	
	char* newPath = new char[strlen(bla)-i];
	for(int n=0;n<(signed)strlen(bla)-i+1;n++)
	{
		newPath[n] = bla[i+n];
		if(newPath[n] == 92)
			newPath[n] = '/';
	}
	return newPath;
}