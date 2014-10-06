//******************************************************************************//
// Copyright 2014 Concordia University											//
//																				//		
// Licensed under the Apache License, Version 2.0 (the "License");				//
// you may not use this file except in compliance with the License.				//
// You may obtain a copy of the License at										//
//																				//
//    http://www.apache.org/licenses/LICENSE-2.0								//
//																				//
// Unless required by applicable law or agreed to in writing, software			//
// distributed under the License is distributed on an "AS IS" BASIS,			//
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.		//
// See the License for the specific language governing permissions and			//
// limitations under the License.												//
//******************************************************************************//

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
