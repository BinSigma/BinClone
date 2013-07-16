// ClonePairsAsmFrame.cpp : implementation file
//

#include "stdafx.h"
#include "CloneDetectorGUI.h"
#include "ClonePairsAsmFrame.h"
#include "ClonePairsAsmView.h"
#include "MainFrm.h"
#include "BFFileHelper.h"
#include "NewDetectDialog.h"
#include "DControllerDialog.h"
#include "CSController.h"
#include "CloneFiles.h"
#include "BFFileHelper.h"


// ClonePairsAsmFrame

IMPLEMENT_DYNCREATE(ClonePairsAsmFrame, CMDIChildWnd)

ClonePairsAsmFrame::ClonePairsAsmFrame()
:m_bInitSplitter(false),
 m_currentLine(-1),
 m_numOfClonePairs(0),
 m_xmlFile(_T("")),
 m_idFromXmlFile(0),
 m_syncScroll(true)
{
}

ClonePairsAsmFrame::~ClonePairsAsmFrame()
{
	if(m_csController)
		delete m_csController;
}


BOOL ClonePairsAsmFrame::OnCreateClient(LPCREATESTRUCT /*lpcs*/, CCreateContext* pContext)
{
	CRect cr;
	GetWindowRect( &cr );

	int x = cr.Width();
	int y = cr.Height();

	// create a splitter with 2 row, 1 columns
	if (!m_wndSplitter.CreateStatic(this, 2, 1, (WS_CHILD | WS_VISIBLE))) // | WS_BORDER | WS_CAPTION | WS_THICKFRAME))
	{
		TRACE0("Failed to CreateStaticSplitter\n");
		return FALSE;
	}

	// add the second splitter pane - which is a nested splitter with 2 columns
	if (!m_wndSplitter2.CreateStatic(
		&m_wndSplitter,     // our parent window is the first splitter
		1, 2,               // the new splitter is 1 rows, 2 columns
		WS_CHILD | WS_VISIBLE | WS_BORDER | WS_CAPTION, // | WS_THICKFRAME,   // style, WS_BORDER is needed
		m_wndSplitter.IdFromRowCol(0, 0)
			// new splitter is in the first row, 2nd column of first splitter
	   ))
	{
		TRACE0("Failed to create nested splitter\n");
		return FALSE;
	}

	
	if (!m_wndSplitter2.CreateView(0, 0, RUNTIME_CLASS(ClonePairsAsmView), CSize(x/2, (int) (y * 0.6)), pContext))
	{
		TRACE0("Failed to create first pane\n");
		return FALSE;
	}

	// add the second splitter pane - an input view in column 1
	if (!m_wndSplitter2.CreateView(0, 1,
		RUNTIME_CLASS(ClonePairsAsmView), CSize(0, 0), pContext))
	{
		TRACE0("Failed to create second pane\n");
		return FALSE;
	}

	// now add the ClonePairsList at the buttom window
	if (!m_wndSplitter.CreateView(1, 0,
		// RUNTIME_CLASS(ClonePairsAsmView), CSize(0, 0), pContext))
        //RUNTIME_CLASS(ClonePairsListView), CSize(0, 0), pContext)) // v2.0
		RUNTIME_CLASS(ClonePairsTreeView), CSize(0, 0), pContext)) // v2.1
	{
		TRACE0("Failed to create first pane\n");
		return FALSE;
	}

	//ClonePairsAsmView* pViewList = (ClonePairsAsmView*)m_wndSplitter.GetPane(1,0);
	//pViewList->initClonePairList();

	m_bInitSplitter = true;

	return TRUE;
}

void ClonePairsAsmFrame::OnSize(UINT nType, int cx, int cy) 
{
	CMDIChildWnd::OnSize(nType, cx, cy);
	
	CRect cr;
	GetWindowRect(&cr);

	if (m_bInitSplitter && nType != SIZE_MINIMIZED )
	{
		m_wndSplitter.SetRowInfo( 0, (int) (cr.Height() * 0.6), 0 );
		m_wndSplitter2.SetColumnInfo( 0, cr.Width()/2, 50);
		m_wndSplitter2.SetColumnInfo( 1, cr.Width()/2, 50);

		m_wndSplitter.RecalcLayout();
	}	
}

bool ClonePairsAsmFrame::init()
{
	bool bSearchTargetFrag = theApp.getNewCDSearchFlag();
	NewDetectDialog newDetectDlg(bSearchTargetFrag);
	newDetectDlg.DoModal();
	if( newDetectDlg.m_ok)
	{
		CBFStrHelper strHelper;
		try
		{
			//int windSize                  = strHelper.strToInt(newDetectDlg.m_windSize);
			//int stride                    = strHelper.strToInt(newDetectDlg.m_stride);
			//int maxKOpLvl                 = strHelper.strToInt(newDetectDlg.m_maxKOP);
			//double ovLap                  = strHelper.strToFloat(newDetectDlg.m_maxOVF);
			bool bfindExact               = newDetectDlg.m_bFindExactClonesChk ? true : false;
			bool bfindInExact             = newDetectDlg.m_bFindInexactClonesChk ? true : false;
			//int keyVectorsSize            = strHelper.strToInt(newDetectDlg.m_keyVectorsSize);
			double occurrenceThrs         = strHelper.strToFloat(newDetectDlg.m_occurrenceThrs);
			int regNormLvl                = newDetectDlg.m_regNormalizedLevel;
            bool bNormalizeToken          = newDetectDlg.m_bNormalizeTokenChk ? true : false;
            int inexactMTD                = newDetectDlg.m_inexactMethodLevel; 
			int dbParamId                 = newDetectDlg.m_db_param_id;
			LPCTSTR pSearchCodeFrag       = NULL;
			bool keepTmpFile              = false;
			CString dbName                = newDetectDlg.m_dbName;
			CString dbUser                = newDetectDlg.m_dbUser;
			CString dbPwd                 = newDetectDlg.m_dbPwd;
			//CString targetFilePathAndName;
			
			if( bSearchTargetFrag )
			{
				pSearchCodeFrag = newDetectDlg.m_searchCodeFragString;
				keepTmpFile = newDetectDlg.m_keepTempFileChk ? true : false; 
				/*
				if( keepTmpFile) 
					m_keepTmpFile = true;
			*/
			}
			
			CMainFrame* pFrame = (CMainFrame*) AfxGetMainWnd();
			pFrame->startProgress();

			//CSDataBase postgres benfung Z:\Security20121108b\exp\Data02\btwhidcs.asm TRUE TRUE 0.5 30
			//dbName = "CSDataBase"; dbUser = "postgres"; dbPwd = "benfung";

			CDControllerDialog cdControlDlg;
			m_targetFilePathAndName = newDetectDlg.m_targetAsmFile;
			m_csController = new CCSController( dbName, dbUser, dbPwd);
			cdControlDlg.init(m_csController, m_targetFilePathAndName,bfindExact,bfindInExact,occurrenceThrs,dbParamId,m_pClones);

			cdControlDlg.DoModal();
			pFrame->stopProgress();									   
			if( cdControlDlg.m_result == true)
			{				
				m_pClones = cdControlDlg.m_pClones;
				return TRUE;  // the only place to return true!
			}
		}
		catch(...)
		{
			AfxMessageBox(_T("Caught an unknown exeception, can't continue!"),
						          MB_ICONSTOP,0);
		}
	}

	return FALSE;

}

// Version 2.0
void ClonePairsAsmFrame::displayAsmContents(const CString & tarFile, const CString & tarContent, const CString & srcFile, const CString & srcContent)
{
	   ClonePairsAsmView* pViewA = (ClonePairsAsmView*) m_wndSplitter2.GetPane(0,0);
	   ClonePairsAsmView* pViewB = (ClonePairsAsmView*) m_wndSplitter2.GetPane(0,1);

	   pViewA->initView(tarFile,tarContent,1);
	   pViewB->initView(srcFile,srcContent,2);

}

void ClonePairsAsmFrame::displayTarAsmContents(const CString & tarFile, const CString & tarContent)
{
	   ClonePairsAsmView* pViewA = (ClonePairsAsmView*) m_wndSplitter2.GetPane(0,0);
	   pViewA->initView(tarFile,tarContent,1);
}

void ClonePairsAsmFrame::displaySrcAsmContents(const CString & srcFile, const CString & srcContent)
{
	   ClonePairsAsmView* pViewB = (ClonePairsAsmView*) m_wndSplitter2.GetPane(0,1);
	   pViewB->initView(srcFile,srcContent,2);
}


BEGIN_MESSAGE_MAP(ClonePairsAsmFrame, CMDIChildWnd)
	ON_WM_SIZE()
	ON_COMMAND(ID_VIEW_NEXT, &ClonePairsAsmFrame::OnViewNext)
	ON_COMMAND(ID_VIEW_PREVIOUS, &ClonePairsAsmFrame::OnViewPrevious)
	ON_COMMAND(ID_BUTTON_NEXT, &ClonePairsAsmFrame::OnButtonNext)
	ON_COMMAND(ID_BUTTON_PREV, &ClonePairsAsmFrame::OnButtonPrev)
	ON_COMMAND(ID_VIEW_SYNCHRONIZEDSCROLLING, &ClonePairsAsmFrame::OnViewSynchronizedscrolling)
	ON_UPDATE_COMMAND_UI(ID_VIEW_SYNCHRONIZEDSCROLLING, &ClonePairsAsmFrame::OnUpdateViewSynchronizedscrolling)
END_MESSAGE_MAP()


// ClonePairsAsmFrame message handlers
bool ClonePairsAsmFrame::fillSelectedClonePairsOnViews(int p_selLine)
{
	ClonePair clonePair;
	if( m_pCurSelCloneFile.getClonePair(p_selLine,clonePair))
	{
		Region regA = clonePair.m_regionA;
		Region regB = clonePair.m_regionB;
	
		ClonePairsAsmView* pViewA = (ClonePairsAsmView*)m_wndSplitter2.GetPane(0,0);
		SetActiveView(pViewA);
		pViewA->highLightLines(clonePair.m_regionA.m_start,clonePair.m_regionA.m_end);
		
		ClonePairsAsmView* pViewB = (ClonePairsAsmView*)m_wndSplitter2.GetPane(0,1);
		SetActiveView(pViewB);
		pViewB->highLightLines(clonePair.m_regionB.m_start,clonePair.m_regionB.m_end);

		ClonePairsAsmView* pViewList = (ClonePairsAsmView*)m_wndSplitter.GetPane(1,0);
		SetActiveView(pViewList);
		pViewList->highLightLines(p_selLine,p_selLine);
		
		return true;
	}
	return false;
}

// Version 2
bool ClonePairsAsmFrame::fillSelectedClonePairsOnViews2(int listLine, int tarStart, int tarEnd, int srcStart, int srcEnd)	                                                    
{
	ClonePairsAsmView* pViewA = NULL;
	ClonePairsAsmView* pViewB = NULL;
	ClonePairsAsmView* pViewList = NULL;
	if( tarStart >=0 && tarEnd >=0 )
	{
	    pViewA = (ClonePairsAsmView*)m_wndSplitter2.GetPane(0,0);
	    SetActiveView(pViewA);
	    pViewA->highLightLines(tarStart,tarEnd);
	}

	if( srcStart >=0 && srcEnd >=0 )
	{
	    pViewB = (ClonePairsAsmView*)m_wndSplitter2.GetPane(0,1);
	    SetActiveView(pViewB);
	    pViewB->highLightLines(srcStart,srcEnd);
	}

	if( listLine >= 0)
	{
	    pViewList = (ClonePairsAsmView*)m_wndSplitter.GetPane(1,0);
	    SetActiveView(pViewList);
		pViewList->highLightLines(listLine,listLine);
	}
	if( pViewA) SetActiveView(pViewA);
	if( pViewB) SetActiveView(pViewB);
	if( pViewList) SetActiveView(pViewList);
	return true;
}

void ClonePairsAsmFrame::ReSyncScroll()
{
	ClonePairsTreeView* pViewList = (ClonePairsTreeView*)m_wndSplitter.GetPane(1,0);
	pViewList->ReSyncScroll(this);
}

void ClonePairsAsmFrame::OnViewNext()
{
	// get the current lines to display
	if( m_numOfClonePairs <= 0)
		return;

	// version 2.1
	ClonePairsTreeView* pViewList = (ClonePairsTreeView*)m_wndSplitter.GetPane(1,0);
	pViewList->Next();


#if 0
	int tmp = m_currentLine;
	//if( (tmp < 0) || ( tmp >= (int) m_pCurSelCloneFile.getNumberOfClonePairs()-1))
	if( (tmp < 0) || ( tmp >= (int) m_numOfClonePairs-1))
	{
		tmp = 0;
	}
	else
	{
		tmp++;
	}

	if( tmp == m_currentLine)
		return;

	ClonePairsAsmView* pViewList = (ClonePairsAsmView*)m_wndSplitter.GetPane(1,0);
	pViewList->selectedLine(tmp);
	pViewList->highLightLines(tmp,tmp);
	m_currentLine = tmp;

	/*
	if( fillSelectedClonePairsOnViews(tmp))
	{
	    m_currentLine = tmp;
	}
	*/
#endif	
}


void ClonePairsAsmFrame::OnViewPrevious()
{
	if( m_numOfClonePairs <= 0)
		return;

		// version 2.1
	ClonePairsTreeView* pViewList = (ClonePairsTreeView*)m_wndSplitter.GetPane(1,0);
	pViewList->Previous();

#if 0
	int tmp = m_currentLine;
	if( tmp < 0) 
	{
		tmp = 0;
	}
	else if( tmp == 0)
	{
		tmp = m_numOfClonePairs-1;
		//tmp = m_pCurSelCloneFile.getNumberOfClonePairs()-1;
	}
	else
	{
		tmp--;
	}
	
	if( tmp == m_currentLine)
		return;

    ClonePairsAsmView* pViewList = (ClonePairsAsmView*)m_wndSplitter.GetPane(1,0);
	pViewList->selectedLine(tmp);
	pViewList->highLightLines(tmp,tmp);
	m_currentLine = tmp;

	/*
	if( fillSelectedClonePairsOnViews(tmp))
	{
	    m_currentLine = tmp;
	}
	*/
#endif

}

bool ClonePairsAsmFrame::selectedParticularLine(int p_line)
{
	if( fillSelectedClonePairsOnViews(p_line))
	{
	    m_currentLine = p_line;
		return true;
	}
	return false;
}

void ClonePairsAsmFrame::OnButtonNext()
{
	// TODO: Add your command handler code here
	OnViewNext();
}


void ClonePairsAsmFrame::OnButtonPrev()
{
	// TODO: Add your command handler code here
	OnViewPrevious();
}

void ClonePairsAsmFrame::SyncScroll(int fromView, UINT nScrollCode, int pos)
{
	ClonePairsAsmView* pView; 
	if( fromView == 2) {
		pView = (ClonePairsAsmView*)m_wndSplitter2.GetPane(0,0);
	}   
	else if( fromView == 1) {
		pView = (ClonePairsAsmView*)m_wndSplitter2.GetPane(0,1);
	}
	else {
		return;
	}
	
	pView->SyncScroll(nScrollCode,pos);
}

void ClonePairsAsmFrame::SyncVWScroll(int fromView, UINT nFlags, short zDelta, CPoint pt)
{
	ClonePairsAsmView* pView; 
	if( fromView == 2) {
		pView = (ClonePairsAsmView*)m_wndSplitter2.GetPane(0,0);
	}   
	else if( fromView == 1) {
		pView = (ClonePairsAsmView*)m_wndSplitter2.GetPane(0,1);
	}
	else {
		return;
	}
	
	pView->SyncVWScroll(nFlags,zDelta,pt);

}

void ClonePairsAsmFrame::KeySynchro(int fromView, UINT nChar)
{
	ClonePairsAsmView* pView; 
	if( fromView == 2) {
		pView = (ClonePairsAsmView*)m_wndSplitter2.GetPane(0,0);
	}   
	else if( fromView == 1) {
		pView = (ClonePairsAsmView*)m_wndSplitter2.GetPane(0,1);
	}
	else {
		return;
	}
	
	pView->KeySynchro(nChar);

}

void ClonePairsAsmFrame::KeySynchro(int fromView, UINT nChar, UINT nRepCnt, UINT nFlags)
{
	ClonePairsAsmView* pView; 
	if( fromView == 2) {
		pView = (ClonePairsAsmView*)m_wndSplitter2.GetPane(0,0);
	}   
	else if( fromView == 1) {
		pView = (ClonePairsAsmView*)m_wndSplitter2.GetPane(0,1);
	}
	else {
		return;
	}
	
	pView->KeySynchro(nChar);


}


void ClonePairsAsmFrame::OnViewSynchronizedscrolling()
{
	
	// TODO: Add your command handler code here
	
	if( m_syncScroll ) 
		m_syncScroll = false;
	else
		m_syncScroll = true;
	 
}


void ClonePairsAsmFrame::OnUpdateViewSynchronizedscrolling(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
	pCmdUI->SetCheck(m_syncScroll ? 1 : 0 );
}
