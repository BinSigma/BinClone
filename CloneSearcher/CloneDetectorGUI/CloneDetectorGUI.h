
// CloneDetectorGUI.h : main header file for the CloneDetectorGUI application
//
#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"       // main symbols


// CCloneDetectorGUIApp:
// See CloneDetectorGUI.cpp for the implementation of this class
//
class CCmdLineInfo : public CCommandLineInfo
{
public:
    CCmdLineInfo(void);
    virtual void ParseParam(const TCHAR* pszParam, BOOL bFlag, BOOL bLast);
    bool m_bBoolParam;
	bool m_fragmentTmpFile;
    bool m_bHelpParam;
    //CString m_strFileParam;
};
 
class CCloneDetectorGUIApp : public CWinAppEx
{
public:
	CCloneDetectorGUIApp();


// Overrides
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

// Implementation
	UINT  m_nAppLook;
	BOOL  m_bHiColorIcons;

	virtual void PreLoadState();
	virtual void LoadCustomState();
	virtual void SaveCustomState();

	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()

public:
	void setCurrXMLFile(CString & p_xmlFile);
	
	bool getNewCDSearchFlag() 
	{
		return m_newCDSearchFlag;
	}

	const CString & getInitalFragStr() { return m_initFragStr;}
	
	const CString & getCurrXMLFile();

protected:
	CString m_currXMLFile;
    bool    m_newCDSearchFlag;
	CString m_initFragStr;
	void readFragFromFile(const CString &infile, CString &fragStr);

public:
	CMultiDocTemplate * m_pDocTemplateClonePairsAsm;
	CMultiDocTemplate * m_pDocTemplateTokenRef;

	afx_msg void OnFileNewSearchCodeFrag();
	afx_msg void OnFileNewClonePairsDetect();
};

extern CCloneDetectorGUIApp theApp;
