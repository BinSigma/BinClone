
// TokenRefFrm.cpp : implementation of the CTokenRefFrm class
//

#include "stdafx.h"
#include "TokenRefFrm.h"
//#include "TokenRefView.h"
#include "TokenTreeView.h"
#include "ClonePairsAsmView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CTokenRefFrm

IMPLEMENT_DYNCREATE(CTokenRefFrm, CMDIChildWndEx)

BEGIN_MESSAGE_MAP(CTokenRefFrm, CMDIChildWndEx)
	ON_WM_SIZE()
END_MESSAGE_MAP()

// CTokenRefFrm construction/destruction

CTokenRefFrm::CTokenRefFrm()
	: m_bInitSplitter(false)
{
	// TODO: add member initialization code here
}

CTokenRefFrm::~CTokenRefFrm()
{
}


// CTokenRefFrm diagnostics

#ifdef _DEBUG
void CTokenRefFrm::AssertValid() const
{
	CMDIChildWndEx::AssertValid();
}

void CTokenRefFrm::Dump(CDumpContext& dc) const
{
	CMDIChildWndEx::Dump(dc);
}
#endif //_DEBUG

// CTokenRefFrm message handlers


BOOL CTokenRefFrm::OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext)
{
	CRect cr;
	GetWindowRect( &cr );

	int x = cr.Width();
	int y = cr.Height();

	// create a splitter with 1 row, 2 columns
	if (!m_wndSplitter.CreateStatic(this, 1, 2))
	{
		TRACE0("Failed to CreateStaticSplitter\n");
		return FALSE;
	}

	if (!m_wndSplitter.CreateView(0, 0,
		RUNTIME_CLASS(TokenTreeView), CSize((int)(x*0.2), y), pContext))
	{
		TRACE0("Failed to create first pane\n");
		return FALSE;
	}

	if (!m_wndSplitter.CreateView(0, 1,
		RUNTIME_CLASS(ClonePairsAsmView), CSize(0,0), pContext))
	{
		TRACE0("Failed to create second pane\n");
		return FALSE;
	}

	m_bInitSplitter = true;

	return true; 
}

void CTokenRefFrm::OnSize(UINT nType, int cx, int cy) 
{
	CMDIChildWnd::OnSize(nType, cx, cy);	
	CRect cr;
	GetWindowRect(&cr);
	if (m_bInitSplitter && nType != SIZE_MINIMIZED )
	{
		m_wndSplitter.SetColumnInfo(0, (int)(cr.Width()*0.2),0);
		m_wndSplitter.RecalcLayout();
	}
	
}

void CTokenRefFrm::init(const CString & p_searchToken, const CloneFiles & p_cloneFiles) 
{
	if (m_bInitSplitter)
	{
		m_searchToken = p_searchToken;
		TokenTreeView* pTokenView = (TokenTreeView*) m_wndSplitter.GetPane(0,0);
		if( pTokenView != NULL)
		{
			pTokenView->initTreeView(p_searchToken, p_cloneFiles, m_tokenRefsVec);
		}

		// initialize the map
		p_cloneFiles.getAsmFilesMap(m_asmFilesMap);
	}
}

void CTokenRefFrm::selectAsmFileAndLine(int p_fileId, int p_line)
{
	ClonePairsAsmView* pRefView = (ClonePairsAsmView*)m_wndSplitter.GetPane(0,1);
	CloneFiles::AsmFilesMap_t::iterator filename_itr = m_asmFilesMap.find(p_fileId);
	if( filename_itr == m_asmFilesMap.end())
	{
		// error
		return;
	}

	CString filename(filename_itr->second);

	SetActiveView(pRefView);
	if( pRefView->getFileId() != p_fileId)
	{
		// check if the file has been already loaded before
		std::map<int,CString>::iterator content_itr = m_contentMap.find(p_fileId);
		if( content_itr != m_contentMap.end())
		{
			// we have loaded the content of the file already, update the content to the view and 
			// highlight the line
			pRefView->initView(filename, content_itr->second,p_fileId);
		}
		else
		{
			// the file hasn't been loaded before, load the content
			CString content(_T(""));
			loadAsmFile(filename,content);
			m_contentMap[p_fileId]=content;
			pRefView->initView(filename, content,p_fileId);
		}
	}

	pRefView->highLightLines(p_line, p_line);
	SetWindowText(filename); 
    
}

void CTokenRefFrm::loadAsmFile(CString & p_filename, CString & p_content)
{
	p_content = _T("");
    CCDController theControllerObj(_T(""),_T(""),-1,-1,-1,-1,false,false,CD_NORM_REG_ROOT,true,-1,-1.0, CD_INEXACT_METHOD_RANDOM);  
	CString lineStr, lineText;

	// Get the content of file
	if (!theControllerObj.openRawAssemblyFile(p_filename))		   
		return;  // error	  
       
	int lineIdx = 0;
	while (theControllerObj.getRawAssemblyFileLineStr(lineStr)) 
	{
		lineText.Format(_T("%5d: %s\n"), lineIdx++, lineStr);
		p_content += lineText;
	}
	
	theControllerObj.closeRawAssemblyFile();
}