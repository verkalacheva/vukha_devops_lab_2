

#ifndef __functions_H__
#define __functions_H__


void PutPixel(SDL_Surface *pSurf, int x, int y, Uint32 color);
bool pointCollision(SDL_Rect *r1, int x, int y);
bool collision(SDL_Rect *r1, SDL_Rect *r2);
SDL_Surface *LoadIMG(const char *filename);

//  0 = LOADING &&   1 = SAVING
char* loadFile(int type);

//Shorts a Path to the form : "data/..."
char* shortPath(char* bla);
#endif