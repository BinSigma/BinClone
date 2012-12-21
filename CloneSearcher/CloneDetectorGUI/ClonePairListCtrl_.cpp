// ClonePairListCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "CloneDetectorGUI.h"
#include "ClonePairListCtrl.h"
#include "CloneFiles.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CClonePairListCtrl

CClonePairListCtrl::CClonePairListCtrl()
{
	//m_colRow1 = RGB(220,220,220);
	//m_colRow2 = RGB(247,247,247);
	m_init = false;
	m_colRow1 = RGB(240,247,249);
	m_colRow2 = RGB(229,232,239);
}

CClonePairListCtrl::~CClonePairListCtrl()
{
	//AfxMessageBox(_T("CClonePairListCtrl destructor is called."));
}


BEGIN_MESSAGE_MAP(CClonePairListCtrl, CListCtrl)
	//{{AFX_MSG_MAP(CClonePairListCtrl)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
	ON_WM_ERASEBKGND()
	ON_NOTIFY_REFLECT(NM_CUSTOMDRAW, OnCustomDraw)
	ON_NOTIFY_EX(TTN_NEEDTEXTA, 0, OnToolNeedText)
    ON_NOTIFY_EX(TTN_NEEDTEXTW, 0, OnToolNeedText)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CClonePairListCtrl message handlers

BOOL CClonePairListCtrl::Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID) 
{
	BOOL retval = CListCtrl::Create(dwStyle, rect, pParentWnd, nID);

	// You can also set up any image lists and modify the style (e.g. set
	// extended styles) here

	return (retval);
}



// Call this function from CMyListView::OnInitialUpdate() or
// a dialog's OnInitDialog() function
BOOL CClonePairListCtrl::Init(CloneFiles * p_CloneFiles)
{
	BOOL bResult = FALSE; //(false);
	if( p_CloneFiles == NULL)
		return bResult;

	CloneFiles::CloneFileVector_t clonefilesvec;
	p_CloneFiles->getCloneFileVector(clonefilesvec);
	int numOfClonePairFiles = clonefilesvec.size(); 

	// Insert the columns 
	int iCol = 0;
	int iColNum;
	//First column - ID
	iColNum = this->InsertColumn(iCol, _T("ID"), LVCFMT_LEFT, 50);
	ASSERT(iCol == iColNum);
	iCol++;  // col 1
	// Second - Number of clone pairs
	iColNum = this->InsertColumn(iCol, _T("Number Of Clone Pairs"), LVCFMT_CENTER, 250);
	ASSERT(iCol == iColNum);
	iCol++;  // col 2
	// Third - Number of File A
    iColNum = this->InsertColumn(iCol, _T("File A"), LVCFMT_LEFT, 320);
	ASSERT(iCol == iColNum);
	iCol++;  // col 3
	// Third - Number of File B
    iColNum = this->InsertColumn(iCol, _T("File B"), LVCFMT_LEFT, 320);
	ASSERT(iCol == iColNum);
	
	// now insert rows
	int iRow=0;
	while(iRow < numOfClonePairFiles )
	{
		CloneFile clonefile = clonefilesvec[iRow];
		int numOfClonePairs = clonefile.getNumberOfClonePairs();
		CString fileA;
		CString fileB;
		clonefile.getFilesWithoutPath(fileA,fileB);

		if( m_toolTipTexts.find(fileA) == m_toolTipTexts.end())
		{
			CString tipsA(clonefile.getFileA());
			m_toolTipTexts[fileA] = tipsA;
		}
		if( m_toolTipTexts.find(fileB) == m_toolTipTexts.end())
		{
			CString tipsB(clonefile.getFileA());
			m_toolTipTexts[fileB] = tipsB;
		}		

		CString idItem;
		idItem.Format(_T("%d"),iRow+1);
		CString numOfClonePairsItem;
		numOfClonePairsItem.Format(_T("%d"),numOfClonePairs);

		int iRowNum;
		iRowNum = this->InsertItem(iRow, idItem);
		ASSERT(iRow == iRowNum);
		//BOOL bResult = SetItemText(iRow, 1, numOfClonePairsItem );
		bResult = SetItemText(iRow, 1, numOfClonePairsItem );
		bResult = SetItemText(iRow, 2, fileA );
		bResult = SetItemText(iRow, 3, fileB );

		++iRow;
	}

	if( bResult == TRUE)
	{
		m_init = true;
	}
	return(bResult);
}//Init

BOOL CClonePairListCtrl::InitCloneFile(CloneFile & p_cloneFile)
{
	this->InsertColumn(0, _T("This is the column"), LVCFMT_LEFT,1200);
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

			int iRowNum = this->InsertItem(iRow, tmp); 
			ASSERT(iRow == iRowNum);
			++iRow;
		}
		else
		{
			break;
		}
	}
	return TRUE;
}

void CClonePairListCtrl::OnCustomDraw(NMHDR* pNMHDR, LRESULT* pResult)
{

   *pResult = 0;
  LPNMLVCUSTOMDRAW  lplvcd = (LPNMLVCUSTOMDRAW)pNMHDR;
  int iRow = lplvcd->nmcd.dwItemSpec;

  switch(lplvcd->nmcd.dwDrawStage)
  {
    case CDDS_PREPAINT :
    {
      *pResult = CDRF_NOTIFYITEMDRAW;
      return;
    }

    // Modify item text and or background
    case CDDS_ITEMPREPAINT:
    {
      lplvcd->clrText = RGB(0,0,0);
      // If you want the sub items the same as the item,
      //*pResult = CDRF_NEWFONT;
      *pResult = CDRF_NOTIFYSUBITEMDRAW;
      return;
    }

	
    // Modify sub item text and/or background
    case CDDS_SUBITEM | CDDS_PREPAINT | CDDS_ITEM:
    {
		
        if(iRow %2){
         lplvcd->clrTextBk = m_colRow2;
        }
        else{
          lplvcd->clrTextBk = m_colRow1;
        }
		

        *pResult = CDRF_DODEFAULT;
        return;
    }
	
  }

}

BOOL CClonePairListCtrl::OnEraseBkgnd(CDC* pDC) 
{
	// TODO: Add your message handler code here and/or call default

  CRect rect;
  CClonePairListCtrl::GetClientRect(rect);


  POINT mypoint;  
  
  CBrush brush0(m_colRow1);
  CBrush brush1(m_colRow2);


 
 int chunk_height=GetCountPerPage();
 pDC->FillRect(&rect,&brush1); // BEN

 /*
 int top = 0;
 int bottom = 0;
 GetItemPosition(0,&mypoint);
 top = mypoint.y;
 GetItemPosition(1,&mypoint);
 bottom = mypoint.y;
 int height = bottom - top;
 */
 for (int i=0;i<=chunk_height;i++)
 {
		
	GetItemPosition(i,&mypoint); // Ben
	rect.top=mypoint.y; // origin top; //mypoint.y ; // ben
	GetItemPosition(i+1,&mypoint); // ben
	rect.bottom = mypoint.y; // ben orign: =bottom; // ben =mypoint.y;
	pDC->FillRect(&rect,i %2 ? &brush1 : &brush0);

	//top = bottom;
	//bottom +=height;

 }

  brush0.DeleteObject();
  brush1.DeleteObject();

  return FALSE;
}


void CClonePairListCtrl::PreSubclassWindow()
{
	// TODO: Add your specialized code here and/or call the base class

	CListCtrl::PreSubclassWindow();

	/*
	 // Disable the CToolTipCtrl of CListCtrl so it won't disturb the CWnd tooltip
    GetToolTips()->Activate(FALSE);

    // Activate the standard CWnd tooltip functionality
    VERIFY( EnableToolTips(TRUE) );
*/
}


INT_PTR CClonePairListCtrl::OnToolHitTest(CPoint point, TOOLINFO* pTI) const
{
	// TODO: Add your specialized code here and/or call the base class

	return CListCtrl::OnToolHitTest(point, pTI);
}

BOOL CClonePairListCtrl::OnToolNeedText(UINT id, NMHDR* pNMHDR, LRESULT* pResult)
{
    CPoint pt(GetMessagePos());
    ScreenToClient(&pt);

    int nRow, nCol;
    CellHitTest(pt, nRow, nCol);

    LPTSTR tooltip = GetToolTipText(nRow, nCol);
    if( tooltip == NULL)
	{ 
		return FALSE;
	}
	/*
	if (tooltip.IsEmpty())
        return FALSE;
		*/
    // Non-unicode applications can receive requests for tooltip-text in unicode
    TOOLTIPTEXTA* pTTTA = (TOOLTIPTEXTA*)pNMHDR;
    TOOLTIPTEXTW* pTTTW = (TOOLTIPTEXTW*)pNMHDR;
#ifndef _UNICODE
    if (pNMHDR->code == TTN_NEEDTEXTA)
        lstrcpyn(pTTTA->szText, static_cast<LPCTSTR>(tooltip), sizeof(pTTTA->szText));
    else
        _mbstowcsz(pTTTW->szText, static_cast<LPCTSTR>(tooltip), 
				sizeof(pTTTW->szText)/sizeof(WCHAR));
#else
	pTTTA->lpszText = (LPSTR) tooltip;
	/*
	int  sizeOfString = (tooltip.GetLength() + 1);
	pTTTA->lpszText = (LPSTR) malloc(sizeOfString*sizeof(wchar_t));// new TCHAR[ sizeOfString ];
    wcscpy_s((wchar_t *)pTTTA->lpszText, sizeOfString, tooltip);
	*/
	/*
	pTTTA->lpszText = (LPSTR) malloc(tooltip.GetLength());
	pTTTA->lpszText 
	
    if (pNMHDR->code == TTN_NEEDTEXTA)
        _wcstombsz(pTTTA->szText, static_cast<LPCTSTR>(tooltip), sizeof(pTTTA->szText));
    else
        lstrcpyn(pTTTW->szText, static_cast<LPCTSTR>(tooltip), 
				sizeof(pTTTW->szText)/sizeof(WCHAR));
		*/		
#endif
    // If wanting to display a tooltip which is longer than 80 characters,
    // one must allocate the needed text-buffer instead of using szText,
    // and point the TOOLTIPTEXT::lpszText to this text-buffer.
    // When doing this, one is required to release this text-buffer again
	//free( pTTTA->lpszText);
    return TRUE;
}

void CClonePairListCtrl::CellHitTest(const CPoint& pt, int& nRow, int& nCol) const
{
	nRow = -1;
	nCol = -1;

	LVHITTESTINFO lvhti = {0};
	lvhti.pt = pt;
	nRow = ListView_SubItemHitTest(m_hWnd, &lvhti);	// SubItemHitTest is non-const
	nCol = lvhti.iSubItem;
	if (!(lvhti.flags & LVHT_ONITEMLABEL))
		nRow = -1;
}

LPTSTR CClonePairListCtrl::GetToolTipText(int nRow, int nCol)
{
	if (nRow!=-1 && nCol!=-1)
	{
		CString text(GetItemText(nRow, nCol));	// Cell-ToolTip
		std::map<CString,CString>::iterator itr = m_toolTipTexts.find(text);
		if( itr != m_toolTipTexts.end())
		{
			return itr->second.GetBuffer();
		}
		else
		{
			return _T("");
		}
	}
	else
	{
		return _T("");
	}
}
