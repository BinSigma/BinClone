
// CloneDetectorGUIView.h : interface of the CCloneDetectorGUIView class
//

#pragma once

#include "CloneFiles.h"
#include "ClonePairListCtrl.h"
#include "CloneDetectorGUIDoc.h"
#include "SearchTokenDlg.h"
#include "MainFrm.h"

class CCloneDetectorGUIView : public CView
{
protected: // create from serialization only
	CCloneDetectorGUIView();
	DECLARE_DYNCREATE(CCloneDetectorGUIView)

// Attributes
public:
	CCloneDetectorGUIDoc* GetDocument() const;

// Operations
public:
	friend class PropertyDialog;

	enum ChildrenIDs { HeaderID = 1, ListID = 2 };

// Overrides
public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void OnInitialUpdate();

protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

// Implementation
public:
	virtual ~CCloneDetectorGUIView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	afx_msg void OnFilePrintPreview();
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnLButtonDblClk(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	DECLARE_MESSAGE_MAP()

protected:
	CloneFiles         m_CloneFiles;
	CClonePairListCtrl m_ListCtrl;
	SearchTokenDlg     m_searchTokenDlg;

public:
	afx_msg void OnToolsSearchtoken();

};

#ifndef _DEBUG  // debug version in CloneDetectorGUIView.cpp
inline CCloneDetectorGUIDoc* CCloneDetectorGUIView::GetDocument() const
   { return reinterpret_cast<CCloneDetectorGUIDoc*>(m_pDocument); }
#endif

