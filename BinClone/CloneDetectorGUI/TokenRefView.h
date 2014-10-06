
// TokenRefView.h : interface of the CTokenRefView class
//

#pragma once

#include "TokenRefDoc.h"
#include "ClonePairsAsmView.h"

class CTokenRefView : public ClonePairsAsmView //public CRichEditView //CView
{
protected: // create from serialization only
	CTokenRefView();
	DECLARE_DYNCREATE(CTokenRefView)

// Attributes
public:
	CTokenRefDoc* GetDocument() const;

// Operations
public:
	void loadFile(CString & p_filename, CString & p_content, int p_highlightedLine);

// Overrides
public:
	//virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	//virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	//virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	//virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

// Implementation
public:
	virtual ~CTokenRefView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	afx_msg void OnFilePrintPreview();
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	//afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	DECLARE_MESSAGE_MAP()
public:
	virtual void OnInitialUpdate();
	//void initTreeView(const CloneFiles::TokenRefsVector_t & p_tokenRefsVec) const;
};

#ifndef _DEBUG  // debug version in TokenRefView.cpp
inline CTokenRefDoc* CTokenRefView::GetDocument() const
   { return reinterpret_cast<CTokenRefDoc*>(m_pDocument); }
#endif

