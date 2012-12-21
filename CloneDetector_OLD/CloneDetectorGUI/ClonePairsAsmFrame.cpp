// ClonePairsAsmFrame.cpp : implementation file
//

#include "stdafx.h"
#include "CloneDetectorGUI.h"
#include "ClonePairsAsmFrame.h"
#include "ClonePairsAsmView.h"
#include "..\..\CloneDetector\CloneDetectorLib\CDController.h"


// ClonePairsAsmFrame

IMPLEMENT_DYNCREATE(ClonePairsAsmFrame, CMDIChildWnd)

ClonePairsAsmFrame::ClonePairsAsmFrame()
:m_bInitSplitter(false),
 m_currentLine(-1),
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

	// create a splitter with 1 row, 2 columns
	if (!m_wndSplitter.CreateStatic(this, 2, 1 ))//,WS_CHILD | WS_VISIBLE | WS_BORDER | WS_CAPTION | WS_THICKFRAME))
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
		RUNTIME_CLASS(ClonePairsAsmView), CSize(0, 0), pContext))
	{
		TRACE0("Failed to create first pane\n");
		return FALSE;
	}

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

void ClonePairsAsmFrame::init(const CloneFile & p_clonefile, const CString & p_xmlFile, int p_id) 
{
	   m_pCurSelCloneFile = p_clonefile;
	   m_xmlFile = p_xmlFile;
	   m_idFromXmlFile = p_id;
       
       CCDController theControllerObj(_T(""),_T(""),-1,-1,-1,-1,false,false,CD_NORM_REG_ROOT,true,-1,-1.0, CD_INEXACT_METHOD_RANDOM);  
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

void ClonePairsAsmFrame::OnViewNext()
{
	// get the current lines to display
	int tmp = m_currentLine;
	if( (tmp < 0) || ( tmp >= (int) m_pCurSelCloneFile.getNumberOfClonePairs()-1))
	{
		tmp = 0;
	}
	else
	{
		tmp++;
	}

	if( fillSelectedClonePairsOnViews(tmp))
	{
	    m_currentLine = tmp;
	}
	
}


void ClonePairsAsmFrame::OnViewPrevious()
{
	// TODO: Add your command handler code here
		// get the current lines to display
	int tmp = m_currentLine;
	if( tmp < 0) 
	{
		tmp = 0;
	}
	else if( tmp == 0)
	{
		tmp = m_pCurSelCloneFile.getNumberOfClonePairs()-1;
	}
	else
	{
		tmp--;
	}
	
	if( fillSelectedClonePairsOnViews(tmp))
	{
	    m_currentLine = tmp;
	}


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
