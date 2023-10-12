// stdafx.h : Include-Datei für Standard-System-Include-Dateien,
//  oder projektspezifische Include-Dateien, die häufig benutzt, aber
//      in unregelmäßigen Abständen geändert werden.
//

#if !defined(AFX_STDAFX_H__F5A4D914_5CF0_45F7_BA76_2033350DFDBA__INCLUDED_)
#define AFX_STDAFX_H__F5A4D914_5CF0_45F7_BA76_2033350DFDBA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define VC_EXTRALEAN		// Selten verwendete Teile der Windows-Header nicht einbinden

#include <afxwin.h>         // MFC-Kern- und -Standardkomponenten
#include <afxext.h>         // MFC-Erweiterungen
#include <afxdisp.h>        // MFC Automatisierungsklassen
#include <afxdtctl.h>		// MFC-Unterstützung für allgemeine Steuerelemente von Internet Explorer 4
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC-Unterstützung für gängige Windows-Steuerelemente
#endif // _AFX_NO_AFXCMN_SUPPORT


#include <SDL.h>
#include <SDL/SDL_image.h>
#include <fstream.h>

#include "functions.h"
#include "levels.h"
#include "Gfx.h"
//#include <stdlib.h>
//#include <stdio.h>





void loadGlobalSprites();
void unloadGlobalSprites();

extern SDL_Surface *screen;

extern SDL_Event event;

extern Uint8 *keys;
extern double frame_multi;


extern bool NoFrameCheck;
extern SDL_Rect tmp_rect;
extern int  cam_x,cam_y;

extern Uint32 magenta;
extern 	BOOL	m_BOWSER_AV,m_RESTARTLEVEL;
extern int numbor,m_HAMMERS;
extern int paintmode;
extern cScene *LEVEL;

#define WIDTH 900	
#define HEIGHT 700
#define ER MessageBox(0,"HUBA-BUBBA","TODO",0);
#define ERO MessageBox("HUBA-BUBBA","TODO",0);

#define GOOMBA 1
#define TURTLE 2
#define SPINY 3
#define FIRECIRCLE1 4
#define FIRECIRCLE2 5
#define FIRECIRCLE3 6
#define FIRE 7
#define CANNON 8
#define BLUME 9
#define BOWSER 10
#define TURTLEFLY 11
#define BRETT 12

extern CString	m_Filename,m_PATH;
extern char* FileToLoad,*Levelname;
extern CListBox	m_TextureCtrl;

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // !defined(AFX_STDAFX_H__F5A4D914_5CF0_45F7_BA76_2033350DFDBA__INCLUDED_)
