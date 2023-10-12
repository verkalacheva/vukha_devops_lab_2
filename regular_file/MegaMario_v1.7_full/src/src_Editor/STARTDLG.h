#if !defined(AFX_STARTDLG_H__F229C447_C2D1_4D34_9F38_DE87633364A6__INCLUDED_)
#define AFX_STARTDLG_H__F229C447_C2D1_4D34_9F38_DE87633364A6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// STARTDLG.h : Header-Datei
//

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld STARTDLG 

class STARTDLG : public CDialog
{
// Konstruktion
public:
	STARTDLG(CWnd* pParent = NULL);   // Standardkonstruktor

// Dialogfelddaten
	//{{AFX_DATA(STARTDLG)
	enum { IDD = IDD_STARTDLG };
	int		m_numborstat;
	int		m_typeo;
	int		m_EXITnumbro;
	int		m_startpoinleve;
	int		m_ANIMA;
	//}}AFX_DATA


// Überschreibungen
	// Vom Klassen-Assistenten generierte virtuelle Funktionsüberschreibungen
	//{{AFX_VIRTUAL(STARTDLG)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung
	//}}AFX_VIRTUAL

// Implementierung
protected:

	// Generierte Nachrichtenzuordnungsfunktionen
	//{{AFX_MSG(STARTDLG)
	afx_msg void OnEditchangestartpointscomba();
	afx_msg void OnPaint();
	afx_msg void Onupdate();
	afx_msg void OnupdateEXIT();
	afx_msg void OnPaintexit();
	afx_msg void OnCelvelname();
	afx_msg void OnshowLevel();
	afx_msg void OnCastelation();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // AFX_STARTDLG_H__F229C447_C2D1_4D34_9F38_DE87633364A6__INCLUDED_
