
// CloneDetectorGUIView.cpp : implementation of the CCloneDetectorGUIView class
//

#include "stdafx.h"
// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
// and search filter handlers and allows sharing of document code with that project.
#ifndef SHARED_HANDLERS
#include "CloneDetectorGUI.h"
#endif

#include "CloneDetectorGUIDoc.h"
#include "CloneDetectorGUIView.h"
#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CCloneDetectorGUIView

IMPLEMENT_DYNCREATE(CCloneDetectorGUIView, CView)

BEGIN_MESSAGE_MAP(CCloneDetectorGUIView, CView)
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CCloneDetectorGUIView::OnFilePrintPreview)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_CONTEXTMENU()
	ON_WM_RBUTTONUP()
	ON_NOTIFY(NM_DBLCLK, ListID, OnLButtonDblClk)
	ON_COMMAND(ID_TOOLS_SEARCHTOKEN, &CCloneDetectorGUIView::OnToolsSearchtoken)
	ON_COMMAND(ID_SEARCH_BUTTON, &CCloneDetectorGUIView::OnToolsSearchtoken)
END_MESSAGE_MAP()

// CCloneDetectorGUIView construction/destruction

CCloneDetectorGUIView::CCloneDetectorGUIView()
{
	// TODO: add construction code here

}

CCloneDetectorGUIView::~CCloneDetectorGUIView()
{
}

BOOL CCloneDetectorGUIView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);

}

// CCloneDetectorGUIView drawing

void CCloneDetectorGUIView::OnDraw(CDC* /*pDC*/)
{
	CCloneDetectorGUIDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: add draw code for native data here
}

int CCloneDetectorGUIView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// Create the style
	DWORD dwStyle = WS_CHILD | WS_VISIBLE | WS_TABSTOP | LVS_REPORT | LVS_SHOWSELALWAYS | LBS_NOTIFY;

	// Create the list control.  Don't worry about specifying
	// correct coordinates.  That will be handled in OnSize()
	BOOL bResult = m_ListCtrl.Create( LVS_REPORT | LVS_SHOWSELALWAYS | LBS_NOTIFY, CRect(0,0,400,200), this, ListID );

	//BOOL bResult = m_ListCtrl.Create(dwStyle, CRect(0,0,0,0), this, ListID); 

	m_ListCtrl.SetExtendedStyle(LVS_EX_FLATSB  | LVS_EX_HEADERDRAGDROP | LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT );		

	CFont fArial;
	fArial.CreatePointFont (100,_T("System"));
	m_ListCtrl.SetFont(&fArial);

	return 0;
	//return (bResult ? 0 : -1);
}

// Override OnSize to resize the control to match the view
void CCloneDetectorGUIView::OnSize(UINT nType, int cx, int cy) 
{
	CView::OnSize(nType, cx, cy);
	
	
	if (::IsWindow(m_ListCtrl.m_hWnd))
		m_ListCtrl.MoveWindow(0, 0, cx, cy, TRUE);
    

	CRect r;
	GetClientRect(r);
	m_ListCtrl.SetColumnWidth (1,r.right /4);
	m_ListCtrl.SetColumnWidth (2,r.right /4);
	m_ListCtrl.SetColumnWidth (3,r.right /4);
	m_ListCtrl.SetColumnWidth (4,r.right /4);
	m_ListCtrl.SetWindowPos (&wndTop,0,0,r.right,r.bottom ,SWP_SHOWWINDOW);
	
	CCloneDetectorGUIDoc* pDoc = GetDocument();
	CString xmlfile = pDoc->getXmlFilePathAndName();
	GetParent()->SetWindowText(xmlfile);
    CMainFrame *pFrame=(CMainFrame*)AfxGetMainWnd();
    pFrame->SetAppName(_T("")); 
    pFrame->OnUpdateFrameTitle(TRUE);
}

void CCloneDetectorGUIView::OnInitialUpdate() 
{
	CView::OnInitialUpdate();

	CCloneDetectorGUIDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;
	
	CString xmlfile = pDoc->getXmlFilePathAndName();

	AfxGetMainWnd()->SetWindowText(xmlfile);
	CMainFrame *pFrame=(CMainFrame*)AfxGetMainWnd(); 
	pFrame->SetWindowText(xmlfile);
	pFrame->SetAppName((LPCTSTR)xmlfile);
    pFrame->OnUpdateFrameTitle(TRUE);
	
	CString error;
	if( m_CloneFiles.init(xmlfile,error) == true)
	{
		//m_ListCtrl.Init(&m_CloneFiles);


// ********************************************************
{
	
	BOOL bResult = FALSE; //(false);
	
	CloneFiles::CloneFileVector_t clonefilesvec;
	m_CloneFiles.getCloneFileVector(clonefilesvec);
	int numOfClonePairFiles = clonefilesvec.size(); 

	// Insert the columns 
	int iCol = 0;
	int iColNum;
	//First column - ID
	iColNum = m_ListCtrl.InsertColumn(iCol, _T("ID"), LVCFMT_LEFT, 50);
	ASSERT(iCol == iColNum);
	iCol++;  // col 1
	// Second - Number of clone pairs
	iColNum = m_ListCtrl.InsertColumn(iCol, _T("Number Of Clone Pairs"), LVCFMT_CENTER, 250);
	ASSERT(iCol == iColNum);
	iCol++;  // col 2
	// Third - Number of File A
    iColNum = m_ListCtrl.InsertColumn(iCol, _T("File A"), LVCFMT_LEFT, 320);
	ASSERT(iCol == iColNum);
	iCol++;  // col 3
	// Third - Number of File B
    iColNum = m_ListCtrl.InsertColumn(iCol, _T("File B"), LVCFMT_LEFT, 320);
	ASSERT(iCol == iColNum);
	/*
	// now insert rows
	int iRow=0;
	while(iRow < numOfClonePairFiles )
	{
		CloneFile clonefile = clonefilesvec[iRow];
		int numOfClonePairs = clonefile.getNumberOfClonePairs();
		CString fileA;
		CString fileB;
		clonefile.getFilesWithoutPath(fileA,fileB);

		if( m_ListCtrl.m_toolTipTexts.find(fileA) == m_ListCtrl.m_toolTipTexts.end())
		{
			CString tipsA(clonefile.getFileA());
			m_ListCtrl.m_toolTipTexts[fileA] = tipsA;
		}
		if( m_ListCtrl.m_toolTipTexts.find(fileB) == m_ListCtrl.m_toolTipTexts.end())
		{
			CString tipsB(clonefile.getFileA());
			m_ListCtrl.m_toolTipTexts[fileB] = tipsB;
		}		

		CString idItem;
		idItem.Format(_T("%d"),iRow+1);
		CString numOfClonePairsItem;
		numOfClonePairsItem.Format(_T("%d"),numOfClonePairs);

		int iRowNum;
		iRowNum = m_ListCtrl.InsertItem(iRow, idItem);
		ASSERT(iRow == iRowNum);
		//BOOL bResult = SetItemText(iRow, 1, numOfClonePairsItem );
		bResult = m_ListCtrl.SetItemText(iRow, 1, numOfClonePairsItem );
		bResult = m_ListCtrl.SetItemText(iRow, 2, fileA );
		bResult = m_ListCtrl.SetItemText(iRow, 3, fileB );

		++iRow;
	}

	if( bResult == TRUE)
	{
		m_ListCtrl.m_init = true;
	}
*/

}

// ********************************************************
	}
	else
	{
		GetParentFrame()->PostMessage(WM_CLOSE);
	}
	
}

// CCloneDetectorGUIView printing


void CCloneDetectorGUIView::OnFilePrintPreview()
{
#ifndef SHARED_HANDLERS
	AFXPrintPreview(this);
#endif
}

BOOL CCloneDetectorGUIView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CCloneDetectorGUIView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CCloneDetectorGUIView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}

void CCloneDetectorGUIView::OnRButtonUp(UINT /* nFlags */, CPoint point)
{
	ClientToScreen(&point);
	OnContextMenu(this, point);
}

void CCloneDetectorGUIView::OnContextMenu(CWnd* /* pWnd */, CPoint point)
{
#ifndef SHARED_HANDLERS
	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
#endif
}


// CCloneDetectorGUIView diagnostics

#ifdef _DEBUG
void CCloneDetectorGUIView::AssertValid() const
{
	CView::AssertValid();
}

void CCloneDetectorGUIView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CCloneDetectorGUIDoc* CCloneDetectorGUIView::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CCloneDetectorGUIDoc)));
	return (CCloneDetectorGUIDoc*)m_pDocument;
}
#endif //_DEBUG

void CCloneDetectorGUIView::OnLButtonDblClk(NMHDR* pNMHDR, LRESULT* pResult)
{
	NMLISTVIEW* p = (NMLISTVIEW*) pNMHDR;
	
	CMainFrame* pFrame = (CMainFrame*) AfxGetMainWnd(); 
	CloneFile clonefile;
	m_CloneFiles.getCloneFile(p->iItem, clonefile); 
	pFrame->launchClonePairsAsmWind(clonefile,p->iItem+1);

}


// CCloneDetectorGUIView message handlers


void CCloneDetectorGUIView::OnToolsSearchtoken()
{
	// TODO: Add your command handler code here
	m_searchTokenDlg.DoModal();
	if( m_searchTokenDlg.m_ok)
	{
		CString searchStr = m_searchTokenDlg.m_searchStr;
	    CMainFrame* pFrame = (CMainFrame*) AfxGetMainWnd(); 
	    pFrame->launchTokenRefWind(m_CloneFiles,searchStr);
	}
}

