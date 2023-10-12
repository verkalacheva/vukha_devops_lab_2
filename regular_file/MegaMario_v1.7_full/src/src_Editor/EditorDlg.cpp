// EditorDlg.cpp : Implementierungsdatei
//

#include "stdafx.h"
#include "Editor.h"
#include "EditorDlg.h"
#include "LoadDLG.h"
#include "STARTDLG.h"
//#include <AFXWIN.H>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define PM_TILE 0
#define PM_PILZ 1
#define PM_LIFE 2
#define PM_STAR 3
#define PM_CRACK 4
#define PM_COIN 5
#define PM_CRACK2 6
#define PM_INVLIFE 7
#define PM_QCOIN 8
#define PM_GOOMBA 9
#define PM_TURTLE 10
#define PM_PLAYER 11
#define PM_EXIT 12
#define PM_SPINY 13
#define PM_FIRE 14
#define PM_FIREC1 15
#define PM_FIREC2 16
#define PM_FIREC3 17
#define PM_BLUME 18
#define PM_CANNON 19
#define PM_CASTLE 20
#define PM_BOWSER 21
#define PM_TURTLEFLY 22
#define PM_BRETT 23

int paintmode;
int mouse_x,mouse_y;

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg-Dialogfeld für Anwendungsbefehl "Info"

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialogfelddaten
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// Vom Klassenassistenten generierte Überladungen virtueller Funktionen
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung
	//}}AFX_VIRTUAL

// Implementierung
protected:
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// Keine Nachrichten-Handler
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEditorDlg Dialogfeld

CEditorDlg::CEditorDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CEditorDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CEditorDlg)
	m_HAMMERS = -1;
	m_MASSPAINT = FALSE;
	//}}AFX_DATA_INIT
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CEditorDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEditorDlg)
	DDX_Control(pDX, IDC_Textures, m_TextureCtrl);
	DDX_CBIndex(pDX, IDC_HAMMERCOMBO, m_HAMMERS);
	DDX_Check(pDX, IDC_RESTARTLEVEL, m_RESTARTLEVEL);
	DDX_Check(pDX, IDC_MASSES, m_MASSPAINT);
	//}}AFX_DATA_MAP
	
}

BEGIN_MESSAGE_MAP(CEditorDlg, CDialog)
	//{{AFX_MSG_MAP(CEditorDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDOK, OnEXIIT)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_LoadLevel, OnLoadLevel)
	ON_BN_CLICKED(IDC_Addtexture, OnAddtexture)
	ON_BN_CLICKED(IDC_paintmode_STATICTILE, OnpaintmodeSTATICTILE)
	ON_BN_CLICKED(IDC_SaveLevel, OnSaveLevel)
	ON_BN_CLICKED(IDC_paintmode_PILZ, OnpaintmodePILZ)
	ON_BN_CLICKED(IDC_paintmode_LIFE, OnpaintmodeLIFE)
	ON_BN_CLICKED(IDC_paintmode_STAR, OnpaintmodeSTAR)
	ON_BN_CLICKED(IDC_paintmode_4, Onpaintmode4)
	ON_BN_CLICKED(IDC_paintmode_coin, Onpaintmodecoin)
	ON_BN_CLICKED(IDC_newLevel, OnnewLevel)
	ON_BN_CLICKED(IDC_paintmode_invlife, Onpaintmodeinvlife)
	ON_BN_CLICKED(IDC_paintmode_CRACKBOX2, OnpaintmodeCRACKBOX2)
	ON_BN_CLICKED(IDC_Q_BOX_COIN, OnQBoxCoin)
	ON_BN_CLICKED(IDC_paintmode_GOOMBA, OnpaintmodeGOOMBA)
	ON_BN_CLICKED(IDC_paintmode_TURTLE, OnpaintmodeTURTLE)
	ON_BN_CLICKED(IDC_paintmode_PLAYER, OnpaintmodePLAYER)
	ON_BN_CLICKED(IDC_paintmode_SPINY, OnpaintmodeSPINY)
	ON_BN_CLICKED(IDC_paintmode_FIRE, OnpaintmodeFIRE)
	ON_BN_CLICKED(IDC_paintmode_FIRE2, OnpaintmodeFIRE2)
	ON_BN_CLICKED(IDC_paintmode_FIRE3, OnpaintmodeFIRE3)
	ON_BN_CLICKED(IDC_paintmode_FIRE4, OnpaintmodeFIRE4)
	ON_BN_CLICKED(IDC_paintmode_BLUME, OnpaintmodeBLUME)
	ON_BN_CLICKED(IDC_paintmode_CANNON, OnpaintmodeCANNON)
	ON_BN_CLICKED(IDC_BOWSER, OnBowser)
	ON_BN_CLICKED(IDC_paintmode_TURTLEFLY3, OnpaintmodeTURTLEFLY3)
	ON_BN_CLICKED(IDC_INFO, OnInfo)
	ON_BN_CLICKED(IDC_paintmode_BRETT, OnpaintmodeBRETT)
	ON_BN_CLICKED(IDC_MASSES, OnMasses)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEditorDlg Nachrichten-Handler

BOOL CEditorDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Hinzufügen des Menübefehls "Info..." zum Systemmenü.

	// IDM_ABOUTBOX muss sich im Bereich der Systembefehle befinden.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{	
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	initSDL();
	SetIcon(m_hIcon, TRUE);			// Großes Symbol verwenden
	SetIcon(m_hIcon, FALSE);		// Kleines Symbol verwenden
	
	// ZU ERLEDIGEN: Hier zusätzliche Initialisierung einfügen
	
	return TRUE;  // Geben Sie TRUE zurück, außer ein Steuerelement soll den Fokus erhalten
}

void CEditorDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// Wollen Sie Ihrem Dialogfeld eine Schaltfläche "Minimieren" hinzufügen, benötigen Sie 
//  den nachstehenden Code, um das Symbol zu zeichnen. Für MFC-Anwendungen, die das 
//  Dokument/Ansicht-Modell verwenden, wird dies automatisch für Sie erledigt.

void CEditorDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // Gerätekontext für Zeichnen

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Symbol in Client-Rechteck zentrieren
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Symbol zeichnen
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

HCURSOR CEditorDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}




SDL_Surface *CUBE;
void CEditorDlg::initSDL()
{
	SDL_Init(SDL_INIT_VIDEO); 
	
    atexit(SDL_Quit);
    screen = SDL_SetVideoMode(WIDTH, HEIGHT, 32, SDL_RLEACCEL | SDL_SRCALPHA);

	SDL_ShowCursor(SDL_DISABLE);
	magenta = SDL_MapRGB(screen->format, 255, 0, 255);
	SDL_WM_SetCaption("Mega Mario Editor", NULL);

	CUBE = LoadIMG("data/gfx/editor/box");
	SDL_SetAlpha(CUBE,SDL_RLEACCEL | SDL_SRCALPHA,160);
	LEVEL = new cScene();
	loadGlobalSprites();
	LEVEL->loadLevel("data/levels/default");
	MessageBox("First : Press \"New Level\" or load a level , before editing!!!","ATTENTION",0);
	FileToLoad = new char[100];
	SetTimer(0,25,NULL);

}

bool down,down2;int oldmouse_x,oldmouse_y;
void CEditorDlg::gameEvent()
{

	while(SDL_PollEvent(&event))
	{
		switch(event.type)
		{
		case SDL_QUIT:
				OnEXIIT();
			break;
			
			case SDL_KEYDOWN:
			if(event.key.keysym.sym == SDLK_ESCAPE)
				OnEXIIT();
			break;
		}
	
	}	
	SDL_GetMouseState (&mouse_x, &mouse_y);

	if( SDL_GetMouseState(NULL, NULL) & 2 )
		{
			if(!down2)
			{
				oldmouse_x = mouse_x;
				oldmouse_y = mouse_y;
				down2=1;
			}
			cam_x += oldmouse_x-mouse_x;
			cam_y += oldmouse_y-mouse_y;
			oldmouse_x = mouse_x;
			oldmouse_y = mouse_y;			
		}else
	{
		down2=0;
		if( SDL_GetMouseState(NULL, NULL) & 1 )
		{			
			if(paintmode==PM_TILE)
			{
				if(m_MASSPAINT)
				{
					for(int i = 0;i<20;i++)
					{
						if((mouse_y+cam_y)/40+i < 100)
							LEVEL->TILE[(mouse_x+cam_x)/40][(mouse_y+cam_y)/40+i] = m_TextureCtrl.GetCurSel()+1;
					}
				}
				else
					LEVEL->TILE[(mouse_x+cam_x)/40][(mouse_y+cam_y)/40] = m_TextureCtrl.GetCurSel()+1;
			}
			else if(paintmode==PM_CRACK)
			{
				LEVEL->BONUS[(mouse_x+cam_x)/40][(mouse_y+cam_y)/40] = 1;
			}
			else if(paintmode==PM_CRACK2)
			{
				LEVEL->BONUS[(mouse_x+cam_x)/40][(mouse_y+cam_y)/40] = 14;
			}
			else if(paintmode==PM_INVLIFE)
			{
				LEVEL->BONUS[(mouse_x+cam_x)/40][(mouse_y+cam_y)/40] = 15;
			}
			else if(paintmode==PM_PILZ)
			{
				LEVEL->BONUS[(mouse_x+cam_x)/40][(mouse_y+cam_y)/40] = 2;
			}
			else if(paintmode==PM_LIFE)
			{
				LEVEL->BONUS[(mouse_x+cam_x)/40][(mouse_y+cam_y)/40] = 4;
			}
			else if(paintmode==PM_STAR)
			{
				LEVEL->BONUS[(mouse_x+cam_x)/40][(mouse_y+cam_y)/40] = 3;
			}
			else if(paintmode==PM_QCOIN)
			{
				LEVEL->BONUS[(mouse_x+cam_x)/40][(mouse_y+cam_y)/40] = 16;
			}
			else if(paintmode==PM_PLAYER)
			{
				LEVEL->STARTPOINTS[numbor].x = (mouse_x+cam_x);
				LEVEL->STARTPOINTS[numbor].y = (mouse_y+cam_y);
			}		
			else if(paintmode==PM_EXIT)
			{
				LEVEL->EXITPOINTS[numbor].x = (mouse_x+cam_x);
				LEVEL->EXITPOINTS[numbor].y = (mouse_y+cam_y);
			}
			else if(paintmode==PM_CASTLE)
			{
				LEVEL->CASTLE.x = (mouse_x+cam_x);
				LEVEL->CASTLE.y = (mouse_y+cam_y);
			}
			else if(paintmode==PM_COIN)
			{
				LEVEL->COIN[(mouse_x+cam_x)/40][(mouse_y+cam_y)/40] = 1;
			}
			else if(paintmode==PM_GOOMBA)
				LEVEL->ENEMY[(mouse_x+cam_x)/40][(mouse_y+cam_y)/40] = GOOMBA;
			else if(paintmode==PM_TURTLE)
				LEVEL->ENEMY[(mouse_x+cam_x)/40][(mouse_y+cam_y)/40-1] = TURTLE;
			else if(paintmode==PM_SPINY)
				LEVEL->ENEMY[(mouse_x+cam_x)/40][(mouse_y+cam_y)/40] = SPINY;
			else if(paintmode==PM_FIRE)
				LEVEL->ENEMY[(mouse_x+cam_x)/40][(mouse_y+cam_y)/40-1] = FIRE;
			else if(paintmode==PM_FIREC1)
				LEVEL->ENEMY[(mouse_x+cam_x)/40][(mouse_y+cam_y)/40] = FIRECIRCLE1;
			else if(paintmode==PM_FIREC2)
				LEVEL->ENEMY[(mouse_x+cam_x)/40][(mouse_y+cam_y)/40] = FIRECIRCLE2;
			else if(paintmode==PM_FIREC3)
				LEVEL->ENEMY[(mouse_x+cam_x)/40][(mouse_y+cam_y)/40] = FIRECIRCLE3;
			else if(paintmode==PM_BLUME)
				LEVEL->ENEMY[(mouse_x+cam_x)/40][(mouse_y+cam_y)/40] = BLUME;
			else if(paintmode==PM_CANNON)
				LEVEL->ENEMY[(mouse_x+cam_x)/40][(mouse_y+cam_y)/40] = CANNON;
			else if(paintmode==PM_TURTLEFLY)
				LEVEL->ENEMY[(mouse_x+cam_x)/40][(mouse_y+cam_y)/40] = TURTLEFLY;
			else if(paintmode==PM_BRETT)
				LEVEL->ENEMY[(mouse_x+cam_x)/40][(mouse_y+cam_y)/40] = BRETT;
			else if(paintmode==PM_BOWSER)
			{
				LEVEL->_BOWSER.x = (mouse_x+cam_x)/40;
				LEVEL->_BOWSER.y = (mouse_y+cam_y)/40;
			}

			
		}
		else 
		{
			down=0;

			if( SDL_GetMouseState(NULL, NULL) & 4 )
			{
				LEVEL->TILE[(mouse_x+cam_x)/40][(mouse_y+cam_y)/40] = 0;
				LEVEL->BONUS[(mouse_x+cam_x)/40][(mouse_y+cam_y)/40] = 0;
				LEVEL->COIN[(mouse_x+cam_x)/40][(mouse_y+cam_y)/40] = 0;
				LEVEL->ENEMY[(mouse_x+cam_x)/40][(mouse_y+cam_y)/40] = 0;
			}
			
		}
	}

	SDL_FillRect(screen,0,LEVEL->BGCOLOR);

	keys = SDL_GetKeyState(NULL);


	#define camspeed 40
	if(keys[SDLK_LEFT])
		cam_x-=camspeed;
	if(keys[SDLK_RIGHT])
		cam_x+=camspeed;
	if(keys[SDLK_UP])
		cam_y-=camspeed;
	if(keys[SDLK_DOWN])
		cam_y+=camspeed;
	if(cam_x<40)
		cam_x = 40;
	else
		if(cam_x>15800-WIDTH)
			cam_x = 15800-WIDTH;
	if(cam_y<40)
		cam_y = 40;
	else
		if(cam_y>4000-HEIGHT)
			cam_y = 4000-HEIGHT;


	LEVEL->drawLevel();
 			
	
	tmp_rect.x = ((int)((mouse_x+cam_x)/40))*40 -cam_x;
	tmp_rect.y = ((int)((mouse_y+cam_y)/40))*40 -cam_y;
	if(paintmode==PM_TILE)
		SDL_UpperBlit(LEVEL->Textures[m_TextureCtrl.GetCurSel()+1],0,screen,&tmp_rect);
	else
		SDL_UpperBlit(CUBE,0,screen,&tmp_rect);
	SDL_Flip(screen);
}


void CEditorDlg::OnEXIIT() 
{
	if(MessageBox("Really wanna EXIT ???","```??? EXIT ???´´´",MB_YESNO | MB_ICONQUESTION)==7)
		return;

	unloadGlobalSprites();
	exit(1);	
}

void CEditorDlg::OnTimer(UINT nIDEvent) 
{
	gameEvent();
	CDialog::OnTimer(nIDEvent);
}

void CEditorDlg::OnLoadLevel() 
{
	m_PATH = "data/levels/";
	LoadDLG LW;
	LW.DoModal();
	if(FileToLoad[0]=='-')
		return;
	
	Levelname = new char[100];
	strcpy(Levelname,FileToLoad);
	LEVEL->unloadLevel();
	LEVEL->loadLevel(FileToLoad);

	//"data/levels/test.txt"
	UpdateData(0);
}

void CEditorDlg::OnAddtexture() 
{

	m_PATH = "data/gfx/";
	LoadDLG LW;
	LW.DoModal();
	
	
	if(FileToLoad[0]=='-')
		return;
	LEVEL->Textures[LEVEL->TEXTUREcount] = LoadIMG(FileToLoad);
	m_TextureCtrl.AddString(FileToLoad);
	LEVEL->TEXTUREcount++;
}

void CEditorDlg::OnpaintmodeSTATICTILE() 
{
	paintmode = PM_TILE;	
}

void CEditorDlg::OnSaveLevel() 
{
	if(MessageBox("Are sure to overwrite the old level  ??? ","```??? OVERWRITE ???´´´",MB_YESNO | MB_ICONQUESTION)==7)
		return;
	UpdateData(1);
	LEVEL->saveLevel();
}

void CEditorDlg::OnpaintmodePILZ() 
{
	paintmode = PM_PILZ;	
}

void CEditorDlg::OnpaintmodeLIFE() 
{
	paintmode = PM_LIFE;
}

void CEditorDlg::OnpaintmodeSTAR() 
{
	paintmode = PM_STAR;	
}

void CEditorDlg::Onpaintmode4() 
{
	paintmode = PM_CRACK;
}

void CEditorDlg::Onpaintmodecoin() 
{
	paintmode = PM_COIN;	
}

void CEditorDlg::OnnewLevel() 
{
	if(MessageBox("Sure create a new level? ","```??? NEW LEVEL ???´´´",MB_YESNO | MB_ICONQUESTION)==7)
		return;
	LEVEL->unloadLevel();	
	LEVEL->loadLevel("data/levels/default");
	
	m_PATH = "data/levels/";
	LoadDLG LW;
	LW.DoModal();
	if(FileToLoad[0]=='-')
	{
		MessageBox("Enter a Levelname!","```!!! LEVEL-NAME !!!´´´",MB_OK | MB_ICONEXCLAMATION);
		m_PATH = "data/levels/";
		LoadDLG LW;
		LW.DoModal();
	}
	Levelname = new char[100];
	strcpy(Levelname,FileToLoad);	


}

void CEditorDlg::Onpaintmodeinvlife() 
{
	paintmode =PM_INVLIFE;
}

void CEditorDlg::OnpaintmodeCRACKBOX2() 
{
paintmode = PM_CRACK2;	
}

void CEditorDlg::OnQBoxCoin() 
{
	paintmode = PM_QCOIN;	
}

void CEditorDlg::OnpaintmodeGOOMBA() 
{
	paintmode = PM_GOOMBA;	
}

void CEditorDlg::OnpaintmodeTURTLE() 
{
	paintmode = PM_TURTLE;	
}

void CEditorDlg::OnpaintmodePLAYER() 
{
	STARTDLG sta;
	sta.DoModal();
}

void CEditorDlg::OnpaintmodeSPINY() 
{
paintmode = PM_SPINY;	
	
}

void CEditorDlg::OnpaintmodeFIRE() 
{
	paintmode = PM_FIREC1;	
}

void CEditorDlg::OnpaintmodeFIRE2() 
{
paintmode = PM_FIREC2;	
}

void CEditorDlg::OnpaintmodeFIRE3() 
{
paintmode = PM_FIREC3;
}

void CEditorDlg::OnpaintmodeFIRE4() 
{
	paintmode = PM_FIRE;
}

void CEditorDlg::OnpaintmodeBLUME() 
{
		paintmode = PM_BLUME;
}

void CEditorDlg::OnpaintmodeCANNON() 
{
		paintmode = PM_CANNON;	
}

void CEditorDlg::OnBowser() 
{	
	m_BOWSER_AV = !m_BOWSER_AV;
	paintmode = PM_BOWSER;
}



void CEditorDlg::OnpaintmodeTURTLEFLY3() 
{
	paintmode = PM_TURTLEFLY;	
}

void CEditorDlg::OnInfo() 
{
	MessageBox(" If 'Restarting-Level' is enabled, you will \n begin this level at startpount '0' , when you die, \n else you'll jump to the last 'Restarting-Level' loaded.\n\n\nEnable this for i.e. castles, new worlds, main levels.","INFO ABOUT 'Restarting-Level'",0);
}

void CEditorDlg::OnpaintmodeBRETT() 
{

		paintmode = PM_BRETT;	
}

void CEditorDlg::OnMasses() 
{
	UpdateData(1);	
}
