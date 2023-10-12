// castle_dlg.cpp: Implementierungsdatei
//

#include "stdafx.h"
#include "Editor.h"
#include "castle_dlg.h"

#include "stdafx.h"
#include "Editor.h"
#include "LoadDLG.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld castle_dlg 


castle_dlg::castle_dlg(CWnd* pParent /*=NULL*/)
	: CDialog(castle_dlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(castle_dlg)
	m_TYPEA = 0;
	//}}AFX_DATA_INIT
}


void castle_dlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(castle_dlg)
	DDX_CBIndex(pDX, IDC_COMBATYPE, m_TYPEA);
	
	//}}AFX_DATA_MAP
	DDX_CBIndex(pDX, IDC_startpointscomba, LEVEL->CASTLE.startpoint);
}


BEGIN_MESSAGE_MAP(castle_dlg, CDialog)
	//{{AFX_MSG_MAP(castle_dlg)
	ON_BN_CLICKED(IDC_REMOVE, OnRemove)
	ON_BN_CLICKED(IDC_PAINT, OnPaint)
	ON_BN_CLICKED(IDC_nextlevel, Onnextlevel)
	ON_BN_CLICKED(IDC_showLevel, OnshowLevel)
	ON_BN_CLICKED(IDC_RELOAD, OnReload)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Behandlungsroutinen für Nachrichten castle_dlg 

void castle_dlg::OnRemove() 
{
	LEVEL->CASTLE.available = 0;
}

void castle_dlg::OnPaint() 
{
	UpdateData(1);
	LEVEL->CASTLE.big = m_TYPEA;
	LEVEL->CASTLE.available = 1;
	paintmode = 20;
}

void castle_dlg::Onnextlevel() 
{
	m_PATH = "data/levels/";
	LoadDLG LW;
	LW.DoModal();
	if(FileToLoad[0]=='-')
		return;
	delete[](LEVEL->CASTLE.level);
	LEVEL->CASTLE.level = new char[100];
	strcpy(LEVEL->CASTLE.level,FileToLoad);
}

void castle_dlg::OnshowLevel() 
{
		if(!LEVEL->CASTLE.level)
	{
		MessageBox("NO LEVEL DEFINED","ERROR",0);	
		return;
	}
	MessageBox(LEVEL->CASTLE.level,LEVEL->CASTLE.level,0);	
}

void castle_dlg::OnReload() 
{
	UpdateData(0);
	
}
