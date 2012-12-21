
// TokenRefFrm.h : interface of the CTokenRefFrm class
//
#pragma once

#include "CloneFiles.h"

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
};
