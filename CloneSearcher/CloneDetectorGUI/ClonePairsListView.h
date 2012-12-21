#pragma once

#include "CloneFiles.h"
#include "ClonePairListCtrl.h"


// ClonePairsListView view

class ClonePairsListView : public CListView
{
	DECLARE_DYNCREATE(ClonePairsListView)

protected:
	ClonePairsListView();           // protected constructor used by dynamic creation
	virtual ~ClonePairsListView();

public:
	enum ChildrenIDs { HeaderID = 1, ListID = 2 };

#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif
	void initView( CloneFile & p_cloneFile);

protected:
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	CClonePairListCtrl m_ListCtrl;

	DECLARE_MESSAGE_MAP()
public:
	virtual void OnInitialUpdate();
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
};


