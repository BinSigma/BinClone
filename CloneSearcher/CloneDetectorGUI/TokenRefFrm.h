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

// TokenRefFrm.h : interface of the CTokenRefFrm class
//
#pragma once

#include "CloneFiles.h"
#include "CSClone.h"
#include "CSController.h"

#define SEARCH_TOKEN_STRING     0x001
#define SEARCH_TOKEN_CONSTANT   0x010
#define SEARCH_TOKEN_IMPORT     0x100


class CTokenRefFrm : public CMDIChildWndEx
{
	DECLARE_DYNCREATE(CTokenRefFrm)
public:
	CTokenRefFrm();

// Attributes
public:
	CloneFiles::TokenRefsVector_t m_tokenRefsVec;

// Operations
public:
	bool init(const CString & p_searchToken, const CString & p_dbName, const CString & p_dbUser, const CString & dbPwd, int p_searchFlag);
	void init(const CString & p_searchToken, const CloneFiles & p_cloneFiles) ;
	void loadAsmFile(CString & p_filename, CString & p_content);
	void selectAsmFileAndLine(int fileId, int p_line);
	CString getSearchTokenStr() {return m_searchToken; }

// Overrides
public:
	//virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	afx_msg void OnSize(UINT nType, int cx, int cy);

// Implementation
public:
	virtual ~CTokenRefFrm();

	CCSController * m_csController;

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

// Generated message map functions
protected:

	CString                   m_searchToken;
	CloneFiles::AsmFilesMap_t m_asmFilesMap;
	CSplitterWnd              m_wndSplitter;
	bool                      m_bInitSplitter;
	std::map<int,CString>     m_contentMap;

	DECLARE_MESSAGE_MAP()
	virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);
public:
	afx_msg void OnClose();
};
