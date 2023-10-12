#if !defined(AFX_CASTLE_DLG_H__8F17977F_C70D_4B6F_8F00_6A891B8AAFEC__INCLUDED_)
#define AFX_CASTLE_DLG_H__8F17977F_C70D_4B6F_8F00_6A891B8AAFEC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// castle_dlg.h : Header-Datei
//

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld castle_dlg 

class castle_dlg : public CDialog
{
// Konstruktion
public:
	castle_dlg(CWnd* pParent = NULL);   // Standardkonstruktor

// Dialogfelddaten
	//{{AFX_DATA(castle_dlg)
	enum { IDD = IDD_CASTLESETTINGS };
	int		m_TYPEA;
	//}}AFX_DATA


// Überschreibungen
	// Vom Klassen-Assistenten generierte virtuelle Funktionsüberschreibungen
	//{{AFX_VIRTUAL(castle_dlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung
	//}}AFX_VIRTUAL

// Implementierung
protected:

	// Generierte Nachrichtenzuordnungsfunktionen
	//{{AFX_MSG(castle_dlg)
	afx_msg void OnRemove();
	afx_msg void OnPaint();
	afx_msg void Onnextlevel();
	afx_msg void OnshowLevel();
	afx_msg void OnReload();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // AFX_CASTLE_DLG_H__8F17977F_C70D_4B6F_8F00_6A891B8AAFEC__INCLUDED_
