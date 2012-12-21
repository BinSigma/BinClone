// ClonePairsAsmFrame.cpp : implementation file
//

#include "stdafx.h"
#include "CloneDetectorGUI.h"
#include "ClonePairsAsmFrame.h"
#include "ClonePairsAsmView.h"
//#include "..\CloneSearcherLib\CSController.h"

#include "MainFrm.h"
#include "BFFileHelper.h"
#include "NewDetectDialog.h"
#include "DControllerDialog.h"
#include "CSController.h"
#include "CloneFiles.h"
#include "BFFileHelper.h"
//#include "CloneFiles.h"
//#include "..\..\CloneSearcher\CloneSearcherLib\CSController.h"


// ClonePairsAsmFrame

IMPLEMENT_DYNCREATE(ClonePairsAsmFrame, CMDIChildWnd)

ClonePairsAsmFrame::ClonePairsAsmFrame()
:m_bInitSplitter(false),
 m_currentLine(-1),
 m_numOfClonePairs(0),
 m_xmlFile(_T("")),
 m_idFromXmlFile(0)
{
}

ClonePairsAsmFrame::~ClonePairsAsmFrame()
{
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
        RUNTIME_CLASS(ClonePairsListView), CSize(0, 0), pContext)) // v2.0
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
			int windSize                  = strHelper.strToInt(newDetectDlg.m_windSize);
			int stride                    = strHelper.strToInt(newDetectDlg.m_stride);
			int maxKOpLvl                 = strHelper.strToInt(newDetectDlg.m_maxKOP);
			double ovLap                  = strHelper.strToFloat(newDetectDlg.m_maxOVF);
			bool bfindExact               = newDetectDlg.m_bFindExactClonesChk ? true : false;
			bool bfindInExact             = newDetectDlg.m_bFindInexactClonesChk ? true : false;
			int keyVectorsSize            = strHelper.strToInt(newDetectDlg.m_keyVectorsSize);
			double occurrenceThrs         = strHelper.strToFloat(newDetectDlg.m_occurrenceThrs);
			int regNormLvl                = newDetectDlg.m_regNormalizedLevel;
            bool bNormalizeToken          = newDetectDlg.m_bNormalizeTokenChk ? true : false;
            int inexactMTD                = newDetectDlg.m_inexactMethodLevel; 
			int dbParamId                 = strHelper.strToInt(newDetectDlg.m_db_param_id);
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
			m_targetFilePathAndName = newDetectDlg.m_targetAsmFile; // "Z:\\Security20121108b\\exp\\Data02\\btwhidcs.asm";
			m_csController = new CCSController( dbName, dbUser, dbPwd);
			cdControlDlg.init(m_csController, m_targetFilePathAndName,bfindExact,bfindInExact,occurrenceThrs,dbParamId,m_pClones);

			cdControlDlg.DoModal();
			pFrame->stopProgress();									   
			if( cdControlDlg.m_result == true)
			{
				//m_xmlFilePathAndName = newDetectDlg.m_targetAsmFile;
                //theApp.setCurrXMLFile(m_xmlFilePathAndName);
				//m_tmpTargetFragAsmFile = cdControlDlg.m_tmpAsmFilePath;
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


void ClonePairsAsmFrame::init(const CloneFile & p_clonefile, const CString & p_xmlFile, int p_id) 
{
	   m_pCurSelCloneFile = p_clonefile;
	   m_xmlFile = p_xmlFile;
	   m_idFromXmlFile = p_id;
       
	   CString dbName, dbUser, dbPwd, targetAssemblyFilePath;
	   CCSParams params;
       CCSController theControllerObj(dbName, dbUser, dbPwd);
            
	   /* OLD
       CCDController theControllerObj(_T(""),_T(""),-1,-1,-1,-1,false,false,CD_NORM_REG_ROOT,true,-1,-1.0, CD_INEXACT_METHOD_RANDOM);  
	   */

	   CString fileA, fileB, contentA, contentB, lineStr, lineText;
	   p_clonefile.getFiles(fileA,fileB);

       // Get the content of file A
	   if (!theControllerObj.openRawAssemblyFile(fileA))
	   {
		   CString error;
		   error.Format(_T("%s is not found!"),fileA);
		   AfxMessageBox(error, MB_ICONSTOP,0);
		   PostMessage(WM_CLOSE);
		   return;  // error
	   }
       int lineIdx = 0;
	   while (theControllerObj.getRawAssemblyFileLineStr(lineStr)) {
		   lineText.Format(_T("%5d: %s\n"), lineIdx++, lineStr);
		   contentA += lineText;
	   }
       theControllerObj.closeRawAssemblyFile();

       // Get the content of file B
	   if (!theControllerObj.openRawAssemblyFile(fileB))
	   {
		   CString error;
		   error.Format(_T("%s is not found!"),fileB);
		   AfxMessageBox(error, MB_ICONSTOP,0);
		   theControllerObj.closeRawAssemblyFile();
		   PostMessage(WM_CLOSE);
		   return;  // error
	   }

	   lineIdx = 0;
	   while (theControllerObj.getRawAssemblyFileLineStr(lineStr)) {
		   lineText.Format(_T("%5d: %s\n"), lineIdx++, lineStr);
		   contentB += lineText;
	   }
       theControllerObj.closeRawAssemblyFile();

	   // call the views to fill the contents
	   ClonePairsAsmView* pViewA = (ClonePairsAsmView*) m_wndSplitter2.GetPane(0,0);
	   ClonePairsAsmView* pViewB = (ClonePairsAsmView*) m_wndSplitter2.GetPane(0,1);

	   pViewA->initView(fileA,contentA);
	   pViewB->initView(fileB,contentB);

	   ClonePairsAsmView* pViewList = (ClonePairsAsmView*)m_wndSplitter.GetPane(1,0);
	   pViewList->initCloneFile(m_pCurSelCloneFile);
}

// Version 2.0
void ClonePairsAsmFrame::displayAsmContents(const CString & tarFile, const CString & tarContent, const CString & srcFile, const CString & srcContent)
{
	   ClonePairsAsmView* pViewA = (ClonePairsAsmView*) m_wndSplitter2.GetPane(0,0);
	   ClonePairsAsmView* pViewB = (ClonePairsAsmView*) m_wndSplitter2.GetPane(0,1);

	   pViewA->initView(tarFile,tarContent);
	   pViewB->initView(srcFile,srcContent);

}

void ClonePairsAsmFrame::displayTarAsmContents(const CString & tarFile, const CString & tarContent)
{
	   ClonePairsAsmView* pViewA = (ClonePairsAsmView*) m_wndSplitter2.GetPane(0,0);
	   pViewA->initView(tarFile,tarContent);
}

void ClonePairsAsmFrame::displaySrcAsmContents(const CString & srcFile, const CString & srcContent)
{
	   ClonePairsAsmView* pViewB = (ClonePairsAsmView*) m_wndSplitter2.GetPane(0,1);
	   pViewB->initView(srcFile,srcContent);
}


BEGIN_MESSAGE_MAP(ClonePairsAsmFrame, CMDIChildWnd)
	ON_WM_SIZE()
	ON_COMMAND(ID_VIEW_NEXT, &ClonePairsAsmFrame::OnViewNext)
	ON_COMMAND(ID_VIEW_PREVIOUS, &ClonePairsAsmFrame::OnViewPrevious)
	ON_COMMAND(ID_BUTTON_NEXT, &ClonePairsAsmFrame::OnButtonNext)
	ON_COMMAND(ID_BUTTON_PREV, &ClonePairsAsmFrame::OnButtonPrev)
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
	if( tarStart >=0 && tarEnd >=0 )
	{
	    ClonePairsAsmView* pViewA = (ClonePairsAsmView*)m_wndSplitter2.GetPane(0,0);
	    SetActiveView(pViewA);
	    pViewA->highLightLines(tarStart,tarEnd);
	}

	if( srcStart >=0 && srcEnd >=0 )
	{
	    ClonePairsAsmView* pViewB = (ClonePairsAsmView*)m_wndSplitter2.GetPane(0,1);
	    SetActiveView(pViewB);
	    pViewB->highLightLines(srcStart,srcEnd);
	}

	if( listLine >= 0)
	{
	    ClonePairsAsmView* pViewList = (ClonePairsAsmView*)m_wndSplitter.GetPane(1,0);
	    SetActiveView(pViewList);
		pViewList->highLightLines(listLine,listLine);
	}
	return true;
}


void ClonePairsAsmFrame::OnViewNext()
{
	// get the current lines to display
	if( m_numOfClonePairs <= 0)
		return;

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
	
}


void ClonePairsAsmFrame::OnViewPrevious()
{
	if( m_numOfClonePairs <= 0)
		return;

	// TODO: Add your command handler code here
		// get the current lines to display
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

/*
BOOL ClonePairsAsmFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Add your specialized code here and/or call the base class

	//return CMDIChildWnd::PreCreateWindow(cs);

	if( !CMDIChildWnd::PreCreateWindow(cs) )
		return FALSE;

	//cs.style =  WS_CHILD | WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_MAXIMIZE;

	cs.style = WS_CHILD | WS_VISIBLE | WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_MAXIMIZE;

//FWS_ADDTOTITLE

	return TRUE;
}
*/

