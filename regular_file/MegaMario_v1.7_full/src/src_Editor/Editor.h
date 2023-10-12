// Editor.h : Haupt-Header-Datei für die Anwendung EDITOR
//

#if !defined(AFX_EDITOR_H__B1E4A364_8300_4835_92D6_D8B56DF28A2B__INCLUDED_)
#define AFX_EDITOR_H__B1E4A364_8300_4835_92D6_D8B56DF28A2B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// Hauptsymbole

/////////////////////////////////////////////////////////////////////////////
// CEditorApp:
// Siehe Editor.cpp für die Implementierung dieser Klasse
//

class CEditorApp : public CWinApp
{
public:
	CEditorApp();

// Überladungen
	// Vom Klassenassistenten generierte Überladungen virtueller Funktionen
	//{{AFX_VIRTUAL(CEditorApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementierung

	//{{AFX_MSG(CEditorApp)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // !defined(AFX_EDITOR_H__B1E4A364_8300_4835_92D6_D8B56DF28A2B__INCLUDED_)
