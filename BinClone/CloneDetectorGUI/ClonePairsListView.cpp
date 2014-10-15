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

// ClonePairsListView.cpp : implementation file
//

#include "stdafx.h"
#include "CloneDetectorGUI.h"
#include "ClonePairsListView.h"


// ClonePairsListView

IMPLEMENT_DYNCREATE(ClonePairsListView, CListView)

ClonePairsListView::ClonePairsListView()
{

}

ClonePairsListView::~ClonePairsListView()
{
}

BEGIN_MESSAGE_MAP(ClonePairsListView, CListView)
	//ON_WM_CREATE()
END_MESSAGE_MAP()

int ClonePairsListView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// Create the style
	DWORD dwStyle = WS_CHILD | WS_VISIBLE | WS_TABSTOP | LVS_REPORT | LVS_SHOWSELALWAYS | LBS_NOTIFY;
	//DWORD dwStyle = WS_CHILD | WS_VISIBLE | LVS_REPORT | LVS_SHOWSELALWAYS | LBS_NOTIFY;
	// Create the list control.  Don't worry about specifying
	// correct coordinates.  That will be handled in OnSize()
	BOOL bResult = m_ListCtrl.Create(dwStyle, CRect(0,0,0,0), this, ListID); 
	//BOOL bResult = m_ListCtrl.Create(dwStyle, CRect(0,0,400,200), this, ListID); 

	m_ListCtrl.SetExtendedStyle(LVS_EX_FLATSB  | LVS_EX_HEADERDRAGDROP | LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT );		

	CFont fArial;
	fArial.CreatePointFont (100,_T("System"));
	m_ListCtrl.SetFont(&fArial);

	return (bResult ? 0 : -1);
}//OnCreate

// ClonePairsListView diagnostics

#ifdef _DEBUG
void ClonePairsListView::AssertValid() const
{
	CListView::AssertValid();
}

#ifndef _WIN32_WCE
void ClonePairsListView::Dump(CDumpContext& dc) const
{
	CListView::Dump(dc);
}
#endif
#endif //_DEBUG


// ClonePairsListView message handlers


void ClonePairsListView::OnInitialUpdate()
{
	CListView::OnInitialUpdate();

	CFont fArial;
	fArial.CreatePointFont (100,_T("System"));
	GetListCtrl().SetFont(&fArial);

	// TODO: Add your specialized code here and/or call the base class
	GetListCtrl().SetExtendedStyle(WS_CHILD | WS_VISIBLE | WS_TABSTOP | LVS_REPORT | LVS_SHOWSELALWAYS | LBS_NOTIFY | LVS_EX_FLATSB  | LVS_EX_HEADERDRAGDROP | LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT );

	GetListCtrl().InsertColumn(0, _T("This is the column"), LVCFMT_LEFT,1200);
	
	
}


BOOL ClonePairsListView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Add your specialized code here and/or call the base class

	if( !CListView::PreCreateWindow(cs))
		return FALSE;

	return TRUE;
}


void ClonePairsListView::initView( CloneFile & p_cloneFile)
{
	//m_ListCtrl.InitCloneFile(p_cloneFile);
	GetListCtrl().EnableWindow(true);
	
	GetListCtrl().InsertColumn(0, _T("This is the column"), LVCFMT_LEFT,1200);
	int numOfClonePairs = p_cloneFile.getNumberOfClonePairs();
	int iRow = 0;
	while(iRow < numOfClonePairs )
	{
		ClonePair pair;
		if( p_cloneFile.getClonePair(iRow,pair))
		{
			CString tmp;
			tmp.Format(_T("clone_pair regionB_end=\"%d\" regionB_start=\"%d\" regionA_end=\"%d\" regionA_start=\"%d\" "),
				          pair.m_regionB.m_end,
						  pair.m_regionB.m_start,
						  pair.m_regionA.m_end,
						pair.m_regionA.m_start);

			int iRowNum = GetListCtrl().InsertItem(iRow, tmp); 
			ASSERT(iRow == iRowNum);
			++iRow;
		}
		else
		{
			break;
		}
	}
	
}

void ClonePairsListView::OnSize(UINT nType, int cx, int cy) 
{
	CView::OnSize(nType, cx, cy);

}//OnSize
