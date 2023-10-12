// EditorDlg.h : Header-Datei
//

#if !defined(AFX_EDITORDLG_H__9B7E72C8_E6B4_4FFC_89DF_75F5F3013AAD__INCLUDED_)
#define AFX_EDITORDLG_H__9B7E72C8_E6B4_4FFC_89DF_75F5F3013AAD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CEditorDlg Dialogfeld

class CEditorDlg : public CDialog
{
// Konstruktion
public:
	CEditorDlg(CWnd* pParent = NULL);	// Standard-Konstruktor

	void initSDL();
	void gameEvent();

// Dialogfelddaten
	//{{AFX_DATA(CEditorDlg)
	enum { IDD = IDD_EDITOR_DIALOG };
	BOOL	m_MASSPAINT;
	//}}AFX_DATA

	// Vom Klassenassistenten generierte Überladungen virtueller Funktionen
	//{{AFX_VIRTUAL(CEditorDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV-Unterstützung
	//}}AFX_VIRTUAL

// Implementierung
protected:
	HICON m_hIcon;

	// Generierte Message-Map-Funktionen
	//{{AFX_MSG(CEditorDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnEXIIT();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnLoadLevel();
	afx_msg void OnAddtexture();
	afx_msg void OnpaintmodeSTATICTILE();
	afx_msg void OnSaveLevel();
	afx_msg void OnpaintmodePILZ();
	afx_msg void OnpaintmodeLIFE();
	afx_msg void OnpaintmodeSTAR();
	afx_msg void Onpaintmode4();
	afx_msg void Onpaintmodecoin();
	afx_msg void OnnewLevel();
	afx_msg void Onpaintmodeinvlife();
	afx_msg void OnpaintmodeCRACKBOX2();
	afx_msg void OnQBoxCoin();
	afx_msg void OnpaintmodeGOOMBA();
	afx_msg void OnpaintmodeTURTLE();
	afx_msg void OnpaintmodePLAYER();
	afx_msg void OnpaintmodeSPINY();
	afx_msg void OnpaintmodeFIRE();
	afx_msg void OnpaintmodeFIRE2();
	afx_msg void OnpaintmodeFIRE3();
	afx_msg void OnpaintmodeFIRE4();
	afx_msg void OnpaintmodeBLUME();
	afx_msg void OnpaintmodeCANNON();
	afx_msg void OnpaintmodeBOWSER();
	afx_msg void OnBowser();
	afx_msg void OnpaintmodeTURTLEFLY();
	afx_msg void OnpaintmodeTURTLEFLY3();
	afx_msg void OnInfo();
	afx_msg void OnpaintmodeBRETT();
	afx_msg void OnMasses();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // !defined(AFX_EDITORDLG_H__9B7E72C8_E6B4_4FFC_89DF_75F5F3013AAD__INCLUDED_)
