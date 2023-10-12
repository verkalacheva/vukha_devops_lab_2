#if !defined(AFX_LOADDLG_H__06D6C171_9BB5_451D_A9D4_6822622BD7CD__INCLUDED_)
#define AFX_LOADDLG_H__06D6C171_9BB5_451D_A9D4_6822622BD7CD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// LoadDLG.h : Header-Datei
//

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld LoadDLG 

class LoadDLG : public CDialog
{
// Konstruktion
public:
	LoadDLG(CWnd* pParent = NULL);   // Standardkonstruktor


// Dialogfelddaten
	//{{AFX_DATA(LoadDLG)
	enum { IDD = IDD_Load };
		//}}AFX_DATA


// Überschreibungen
	// Vom Klassen-Assistenten generierte virtuelle Funktionsüberschreibungen
	//{{AFX_VIRTUAL(LoadDLG)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung
	//}}AFX_VIRTUAL

// Implementierung
protected:

	// Generierte Nachrichtenzuordnungsfunktionen
	//{{AFX_MSG(LoadDLG)
	afx_msg void OnChangeFilename();
	afx_msg void OnCancel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // AFX_LOADDLG_H__06D6C171_9BB5_451D_A9D4_6822622BD7CD__INCLUDED_
