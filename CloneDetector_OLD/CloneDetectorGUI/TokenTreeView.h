#pragma once

#include "TokenRefDoc.h"
#include "CloneFiles.h"


// TokenTreeView view

class TokenTreeView : public CTreeView
{
	DECLARE_DYNCREATE(TokenTreeView)

protected:
	TokenTreeView();           // protected constructor used by dynamic creation
	virtual ~TokenTreeView();

public:

	CTokenRefDoc* GetDocument() const;

	CloneFiles::TokenRefsVector_t m_tokenRefsVec;

#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

public:
    typedef std::map<HTREEITEM,Reference>  hItemRefMap_t;

	void initTreeView(const CString &                        p_searchToken,
		              const CloneFiles &                     p_cloneFiles, 
		              const CloneFiles::TokenRefsVector_t &  p_tokenRefsVec);
	 
protected:
	CloneFiles::AsmFilesMap_t m_asmFilesMap;
	hItemRefMap_t m_refMap;

	DECLARE_MESSAGE_MAP()

public:
	virtual void OnInitialUpdate();
	afx_msg void OnTvnItemChanged(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnTokenViewClose();
};


