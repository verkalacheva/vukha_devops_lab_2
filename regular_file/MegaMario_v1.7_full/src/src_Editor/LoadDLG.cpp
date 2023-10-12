// LoadDLG.cpp: Implementierungsdatei
//

#include "stdafx.h"
#include "Editor.h"
#include "LoadDLG.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld LoadDLG 


LoadDLG::LoadDLG(CWnd* pParent /*=NULL*/)
	: CDialog(LoadDLG::IDD, pParent)
{
	//{{AFX_DATA_INIT(LoadDLG)
	m_Filename = _T("");
	//}}AFX_DATA_INIT
}


void LoadDLG::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(LoadDLG)
	DDX_Text(pDX, IDC_FILENAME, m_Filename);
	DDX_Text(pDX, IDC_PATH, m_PATH);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(LoadDLG, CDialog)
	//{{AFX_MSG_MAP(LoadDLG)
	ON_EN_CHANGE(IDC_FILENAME, OnChangeFilename)
	ON_BN_CLICKED(IDC_CANCEL, OnCancel)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Behandlungsroutinen für Nachrichten LoadDLG 

void LoadDLG::OnChangeFilename() 
{
	UpdateData(1);
	
	sprintf(FileToLoad,"%s%s",static_cast<LPCTSTR>(m_PATH),static_cast<LPCTSTR>(m_Filename));	
}

void LoadDLG::OnCancel() 
{
	FileToLoad[0] = '-';
	this->OnOK();
}
