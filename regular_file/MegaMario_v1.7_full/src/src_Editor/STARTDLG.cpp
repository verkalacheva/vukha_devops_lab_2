// STARTDLG.cpp: Implementierungsdatei
//

#include "stdafx.h"
#include "Editor.h"
#include "STARTDLG.h"
#include "LoadDLG.h"
#include "castle_dlg.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld STARTDLG 


STARTDLG::STARTDLG(CWnd* pParent /*=NULL*/)
	: CDialog(STARTDLG::IDD, pParent)
{
	//{{AFX_DATA_INIT(STARTDLG)
	m_numborstat = -1;
	m_typeo = -1;
	m_EXITnumbro = -1;
	m_startpoinleve = -1;
	m_ANIMA = -1;
	//}}AFX_DATA_INIT
}


void STARTDLG::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(STARTDLG)
	DDX_CBIndex(pDX, IDC_startpointscomba, m_numborstat);
	DDX_CBIndex(pDX, IDC_COMBOSTART2, m_typeo);
	DDX_CBIndex(pDX, IDC_sEXITpointscomba2, m_EXITnumbro);
	DDX_CBIndex(pDX, IDC_COMBOEXITSTARPOITM, m_startpoinleve);
	DDX_CBIndex(pDX, IDC_ANIM, m_ANIMA);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(STARTDLG, CDialog)
	//{{AFX_MSG_MAP(STARTDLG)
	ON_CBN_EDITCHANGE(IDC_startpointscomba, OnEditchangestartpointscomba)
	ON_BN_CLICKED(IDC_PAINT, OnPaint)
	ON_BN_CLICKED(IDC_update, Onupdate)
	ON_BN_CLICKED(IDC_updateEXIT, OnupdateEXIT)
	ON_BN_CLICKED(IDC_PAINTEXIT, OnPaintexit)
	ON_BN_CLICKED(IDCELVELNAME, OnCelvelname)
	ON_BN_CLICKED(IDC_showLevel, OnshowLevel)
	ON_BN_CLICKED(IDC_CASTELATION, OnCastelation)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Behandlungsroutinen für Nachrichten STARTDLG 

void STARTDLG::OnEditchangestartpointscomba() 
{
	UpdateData(1);
	numbor = m_numborstat;
	m_typeo = LEVEL->STARTPOINTS[numbor].type;
	UpdateData(0);
}

void STARTDLG::OnPaint() 
{
	UpdateData(1);
	numbor = m_numborstat;
	LEVEL->STARTPOINTS[numbor].type = m_typeo;
	paintmode = 11;	
}

void STARTDLG::Onupdate() 
{
	UpdateData(1);
	numbor = m_numborstat;
	paintmode = 11;	
	m_typeo = LEVEL->STARTPOINTS[numbor].type;
	UpdateData(0);
}

void STARTDLG::OnupdateEXIT() 
{
	UpdateData(1);
	numbor = m_EXITnumbro;
	m_startpoinleve = LEVEL->EXITPOINTS[numbor].startpoint;
	m_ANIMA = LEVEL->EXITPOINTS[numbor].type;
	paintmode = 12;	
	UpdateData(0);
}

void STARTDLG::OnPaintexit() 
{
	UpdateData(1);
	numbor = m_EXITnumbro;
	LEVEL->EXITPOINTS[numbor].startpoint = m_startpoinleve;
	LEVEL->EXITPOINTS[numbor].type = m_ANIMA ;

	paintmode = 12;
}

void STARTDLG::OnCelvelname() 
{
	OnPaintexit();
	m_PATH = "data/levels/";
	LoadDLG LW;
	LW.DoModal();
	if(FileToLoad[0]=='-')
		return;
	LEVEL->EXITPOINTS[numbor].level = new char[100];
	strcpy(LEVEL->EXITPOINTS[numbor].level,FileToLoad);

}

void STARTDLG::OnshowLevel() 
{
	OnPaintexit();

	if(!LEVEL->EXITPOINTS[numbor].level)
	{
		MessageBox("NO LEVEL DEFINED","ERROR",0);	
		return;
	}
	MessageBox(LEVEL->EXITPOINTS[numbor].level,LEVEL->EXITPOINTS[numbor].level,0);	
}

void STARTDLG::OnCastelation() 
{
	castle_dlg cas;
	cas.DoModal();
}
