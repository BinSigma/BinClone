
// MainFrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "CloneDetectorGUI.h"
#include "CloneDetectorGUIView.h"
#include "ClonePairsAsmFrame.h"
#include "ClonePairsAsmDoc.h"
#include "TokenRefDoc.h"
#include "TokenRefFrm.h"
#include "afxdialogex.h"


#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define ID_PROGRESS_TIMER	1
#define PROGRESS_MAX		100

// CMainFrame

IMPLEMENT_DYNAMIC(CMainFrame, CMDIFrameWndEx)

const int  iMaxUserToolbars = 10;
const UINT uiFirstUserToolBarId = AFX_IDW_CONTROLBAR_FIRST + 40;
const UINT uiLastUserToolBarId = uiFirstUserToolBarId + iMaxUserToolbars - 1;

BEGIN_MESSAGE_MAP(CMainFrame, CMDIFrameWndEx)
	ON_WM_CREATE()
	ON_COMMAND(ID_WINDOW_MANAGER, &CMainFrame::OnWindowManager)
	ON_COMMAND(ID_VIEW_CUSTOMIZE, &CMainFrame::OnViewCustomize)
	ON_REGISTERED_MESSAGE(AFX_WM_CREATETOOLBAR, &CMainFrame::OnToolbarCreateNew)
	ON_COMMAND_RANGE(ID_VIEW_APPLOOK_WIN_2000, ID_VIEW_APPLOOK_WINDOWS_7, &CMainFrame::OnApplicationLook)
	ON_UPDATE_COMMAND_UI_RANGE(ID_VIEW_APPLOOK_WIN_2000, ID_VIEW_APPLOOK_WINDOWS_7, &CMainFrame::OnUpdateApplicationLook)
	ON_COMMAND(ID_FILE_PROPERTIES, &CMainFrame::OnFileProperties)
	ON_WM_TIMER()
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

// CMainFrame construction/destruction

CMainFrame::CMainFrame()
	:m_pCurSelCloneFile(_T(""),_T(""))
{
	// TODO: add member initialization code here
	theApp.m_nAppLook = theApp.GetInt(_T("ApplicationLook"), ID_VIEW_APPLOOK_VS_2008);
	m_bInProgress = FALSE;
	m_nProgressCurr = 0;
}

CMainFrame::~CMainFrame()
{
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CMDIFrameWndEx::OnCreate(lpCreateStruct) == -1)
		return -1;

	BOOL bNameValid;
	// set the visual manager and style based on persisted value
	OnApplicationLook(theApp.m_nAppLook);

	if (!m_wndMenuBar.Create(this))
	{
		TRACE0("Failed to create menubar\n");
		return -1;      // fail to create
	}

	m_wndMenuBar.SetPaneStyle(m_wndMenuBar.GetPaneStyle() | CBRS_SIZE_DYNAMIC | CBRS_TOOLTIPS | CBRS_FLYBY);

	// prevent the menu bar from taking the focus on activation
	CMFCPopupMenu::SetForceMenuFocus(FALSE);

	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP | CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndToolBar.LoadToolBar(theApp.m_bHiColorIcons ? IDR_MAINFRAME_256 : IDR_MAINFRAME))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}

	CString strToolBarName;
	bNameValid = strToolBarName.LoadString(IDS_TOOLBAR_STANDARD);
	ASSERT(bNameValid);
	m_wndToolBar.SetWindowText(strToolBarName);

	CString strCustomize;
	bNameValid = strCustomize.LoadString(IDS_TOOLBAR_CUSTOMIZE);
	ASSERT(bNameValid);
	m_wndToolBar.EnableCustomizeButton(TRUE, ID_VIEW_CUSTOMIZE, strCustomize);

	// Allow user-defined toolbars operations:
	InitUserToolbars(NULL, uiFirstUserToolBarId, uiLastUserToolBarId);

	if (!m_wndStatusBar.Create(this))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}
	m_wndStatusBar.SetIndicators(indicators, sizeof(indicators)/sizeof(UINT));

	// TODO: Delete these five lines if you don't want the toolbar and menubar to be dockable
	m_wndMenuBar.EnableDocking(CBRS_ALIGN_ANY);
	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
	DockPane(&m_wndMenuBar);
	DockPane(&m_wndToolBar);


	// enable Visual Studio 2005 style docking window behavior
	CDockingManager::SetDockingMode(DT_SMART);
	// enable Visual Studio 2005 style docking window auto-hide behavior
	EnableAutoHidePanes(CBRS_ALIGN_ANY);

	// Enable enhanced windows management dialog
	EnableWindowsDialog(ID_WINDOW_MANAGER, ID_WINDOW_MANAGER, TRUE);

	// Enable toolbar and docking window menu replacement
	EnablePaneMenu(TRUE, ID_VIEW_CUSTOMIZE, strCustomize, ID_VIEW_TOOLBAR);

	// enable quick (Alt+drag) toolbar customization
	CMFCToolBar::EnableQuickCustomization();

	if (CMFCToolBar::GetUserImages() == NULL)
	{
		// load user-defined toolbar images
		if (m_UserImages.Load(_T(".\\UserImages.bmp")))
		{
			CMFCToolBar::SetUserImages(&m_UserImages);
		}
	}

	/* leave the following here for future needs
	// enable menu personalization (most-recently used commands)
	// TODO: define your own basic commands, ensuring that each pulldown menu has at least one basic command.
	CList<UINT, UINT> lstBasicCommands;

	lstBasicCommands.AddTail(ID_FILE_NEW);
	lstBasicCommands.AddTail(ID_FILE_OPEN);
	//lstBasicCommands.AddTail(ID_FILE_SAVE);
	//lstBasicCommands.AddTail(ID_FILE_PRINT);
	lstBasicCommands.AddTail(ID_APP_EXIT);
	//lstBasicCommands.AddTail(ID_EDIT_CUT);
	//lstBasicCommands.AddTail(ID_EDIT_PASTE);
	//lstBasicCommands.AddTail(ID_EDIT_UNDO);
	lstBasicCommands.AddTail(ID_APP_ABOUT);
	lstBasicCommands.AddTail(ID_VIEW_STATUS_BAR);
	lstBasicCommands.AddTail(ID_VIEW_TOOLBAR);
	lstBasicCommands.AddTail(ID_VIEW_APPLOOK_OFF_2003);
	lstBasicCommands.AddTail(ID_VIEW_APPLOOK_VS_2005);
	lstBasicCommands.AddTail(ID_VIEW_APPLOOK_OFF_2007_BLUE);
	lstBasicCommands.AddTail(ID_VIEW_APPLOOK_OFF_2007_SILVER);
	lstBasicCommands.AddTail(ID_VIEW_APPLOOK_OFF_2007_BLACK);
	lstBasicCommands.AddTail(ID_VIEW_APPLOOK_OFF_2007_AQUA);
	lstBasicCommands.AddTail(ID_VIEW_APPLOOK_WINDOWS_7);

	CMFCToolBar::SetBasicCommands(lstBasicCommands);
	*/
	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CMDIFrameWndEx::PreCreateWindow(cs) )
		return FALSE;
	// TODO: Modify the Window class or styles here by modifying

	return TRUE;
}

// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CMDIFrameWndEx::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CMDIFrameWndEx::Dump(dc);
}
#endif //_DEBUG


// CMainFrame message handlers

void CMainFrame::OnWindowManager()
{
	ShowWindowsDialog();
}

void CMainFrame::OnViewCustomize()
{
	CMFCToolBarsCustomizeDialog* pDlgCust = new CMFCToolBarsCustomizeDialog(this, TRUE /* scan menus */);
	pDlgCust->EnableUserDefinedToolbars();
	pDlgCust->Create();
}

LRESULT CMainFrame::OnToolbarCreateNew(WPARAM wp,LPARAM lp)
{
	LRESULT lres = CMDIFrameWndEx::OnToolbarCreateNew(wp,lp);
	if (lres == 0)
	{
		return 0;
	}

	CMFCToolBar* pUserToolbar = (CMFCToolBar*)lres;
	ASSERT_VALID(pUserToolbar);

	BOOL bNameValid;
	CString strCustomize;
	bNameValid = strCustomize.LoadString(IDS_TOOLBAR_CUSTOMIZE);
	ASSERT(bNameValid);

	pUserToolbar->EnableCustomizeButton(TRUE, ID_VIEW_CUSTOMIZE, strCustomize);
	return lres;
}

void CMainFrame::OnApplicationLook(UINT id)
{
	CWaitCursor wait;

	theApp.m_nAppLook = id;

	switch (theApp.m_nAppLook)
	{
	case ID_VIEW_APPLOOK_WIN_2000:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManager));
		break;

	case ID_VIEW_APPLOOK_OFF_XP:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerOfficeXP));
		break;

	case ID_VIEW_APPLOOK_WIN_XP:
		CMFCVisualManagerWindows::m_b3DTabsXPTheme = TRUE;
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows));
		break;

	case ID_VIEW_APPLOOK_OFF_2003:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerOffice2003));
		CDockingManager::SetDockingMode(DT_SMART);
		break;

	case ID_VIEW_APPLOOK_VS_2005:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerVS2005));
		CDockingManager::SetDockingMode(DT_SMART);
		break;

	case ID_VIEW_APPLOOK_VS_2008:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerVS2008));
		CDockingManager::SetDockingMode(DT_SMART);
		break;

	case ID_VIEW_APPLOOK_WINDOWS_7:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows7));
		CDockingManager::SetDockingMode(DT_SMART);
		break;

	default:
		switch (theApp.m_nAppLook)
		{
		case ID_VIEW_APPLOOK_OFF_2007_BLUE:
			CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_LunaBlue);
			break;

		case ID_VIEW_APPLOOK_OFF_2007_BLACK:
			CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_ObsidianBlack);
			break;

		case ID_VIEW_APPLOOK_OFF_2007_SILVER:
			CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_Silver);
			break;

		case ID_VIEW_APPLOOK_OFF_2007_AQUA:
			CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_Aqua);
			break;
		}

		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerOffice2007));
		CDockingManager::SetDockingMode(DT_SMART);
	}

	RedrawWindow(NULL, NULL, RDW_ALLCHILDREN | RDW_INVALIDATE | RDW_UPDATENOW | RDW_FRAME | RDW_ERASE);

	theApp.WriteInt(_T("ApplicationLook"), theApp.m_nAppLook);
}

void CMainFrame::OnUpdateApplicationLook(CCmdUI* pCmdUI)
{
	pCmdUI->SetRadio(theApp.m_nAppLook == pCmdUI->m_nID);
}

BOOL CMainFrame::LoadFrame(UINT nIDResource, DWORD dwDefaultStyle, CWnd* pParentWnd, CCreateContext* pContext) 
{
	// base class does the real work

	if (!CMDIFrameWndEx::LoadFrame(nIDResource, dwDefaultStyle, pParentWnd, pContext))
	{
		return FALSE;
	}

	// enable customization button for all user toolbars
	BOOL bNameValid;
	CString strCustomize;
	bNameValid = strCustomize.LoadString(IDS_TOOLBAR_CUSTOMIZE);
	ASSERT(bNameValid);

	for (int i = 0; i < iMaxUserToolbars; i ++)
	{
		CMFCToolBar* pUserToolbar = GetUserToolBarByIndex(i);
		if (pUserToolbar != NULL)
		{
			pUserToolbar->EnableCustomizeButton(TRUE, ID_VIEW_CUSTOMIZE, strCustomize);
		}
	}

	return TRUE;
}



void CMainFrame::launchClonePairsAsmWind(const CloneFile & p_pCurSelCloneFile, int p_id) const
{
		// TODO: Add your command handler code here
	CMDIFrameWnd* pMainWnd = (CMDIFrameWnd*)AfxGetMainWnd();
	CMDIChildWnd* pActiveChild = (CMDIChildWnd*)pMainWnd->MDIGetActive();
	
	CDocument* pDocument;
	if (pActiveChild == NULL || (pDocument = pActiveChild->GetActiveDocument()) == NULL) 
	{
		TRACE("Warning: No active document for WindowNew command\n");
		AfxMessageBox(AFX_IDP_COMMAND_FAILURE);
		return; // Command failed
	}

	// Otherwise, we have a new frame!
	CDocTemplate* pTemplate = ((CCloneDetectorGUIApp*) AfxGetApp())->m_pDocTemplateClonePairsAsm;
	ASSERT_VALID(pTemplate);
	CFrameWnd* pFrame = pTemplate->CreateNewFrame(NULL, pActiveChild);
	if (pFrame == NULL)
	{
		TRACE("Warning: failed to create new frame\n");
		AfxMessageBox(AFX_IDP_COMMAND_FAILURE);
		return; // Command failed
	}
	pTemplate->InitialUpdateFrame(pFrame, pDocument);
	
	ClonePairsAsmFrame* pAsmFram = (ClonePairsAsmFrame*) MDIGetActive();
	pAsmFram->init( p_pCurSelCloneFile,theApp.getCurrXMLFile(),p_id);

}

void CMainFrame::launchTokenRefWind(const CloneFiles & p_cloneFiles, const CString & p_searchTokenStr) const
{
	//CString searchStr(_T("9"));

	// launch a search box to get the search string


	// setup the TokeRef frame
	CMDIFrameWnd* pMainWnd = (CMDIFrameWnd*)AfxGetMainWnd();
	CMDIChildWnd* pActiveChild = (CMDIChildWnd*)pMainWnd->MDIGetActive();
	
	CDocument* pDocument;
	if (pActiveChild == NULL || (pDocument = pActiveChild->GetActiveDocument()) == NULL) 
	{
		TRACE("Warning: No active document for WindowNew command\n");
		AfxMessageBox(AFX_IDP_COMMAND_FAILURE);
		return; // Command failed
	}

	// Otherwise, we have a new frame!
	CDocTemplate* pTemplate = ((CCloneDetectorGUIApp*) AfxGetApp())->m_pDocTemplateTokenRef;
	ASSERT_VALID(pTemplate);
	CDocument* ptokenRefDoc = pTemplate->CreateNewDocument();
	CFrameWnd* pFrame = pTemplate->CreateNewFrame(NULL, pActiveChild);
	if (pFrame == NULL)
	{
		TRACE("Warning: failed to create new frame\n");
		AfxMessageBox(AFX_IDP_COMMAND_FAILURE);
		return; // Command failed
	}
	pTemplate->InitialUpdateFrame(pFrame, ptokenRefDoc); //pDocument);
	
	CTokenRefFrm* pTokenFrame = (CTokenRefFrm*) MDIGetActive();

	// now, for code efficency, pass the reference of the frame m_tokenRefsVecto for searching
	p_cloneFiles.searchTokens(p_searchTokenStr,pTokenFrame->m_tokenRefsVec);
	pTokenFrame->init(p_searchTokenStr, p_cloneFiles);

}


void CMainFrame::startProgress() 
{
	if (!m_bInProgress)
	{
	    GetStatusBar ().EnablePaneProgressBar (nStatusProgress, PROGRESS_MAX);
	    m_nProgressCurr = 0;
	    m_bInProgress = TRUE;
	    GetStatusBar ().SetPaneProgress (nStatusProgress, 5);
	    SetTimer (ID_PROGRESS_TIMER, 200, NULL);
	}
	
}

void CMainFrame::stopProgress() 
{
	if (m_bInProgress)
	{
		KillTimer (ID_PROGRESS_TIMER);
		GetStatusBar ().EnablePaneProgressBar (nStatusProgress, -1);
		m_bInProgress = FALSE;
	}
}

void CMainFrame::OnTimer(UINT_PTR nIDEvent) 
{
	if (nIDEvent == ID_PROGRESS_TIMER)
	{
		m_nProgressCurr += 5;
		if (m_nProgressCurr > PROGRESS_MAX)
		{
			m_nProgressCurr = 0;
		}
		GetStatusBar ().SetPaneProgress (nStatusProgress, m_nProgressCurr);
	}
}

void CMainFrame::OnFileProperties()
{
	// TODO: Add your command handler code here
	m_propertyDlg.DoModal();
}



// PropertyDialog dialog

IMPLEMENT_DYNAMIC(PropertyDialog, CDialogEx)

PropertyDialog::PropertyDialog(CWnd* pParent /*=NULL*/)
	: CDialogEx(PropertyDialog::IDD, pParent)
{

}

PropertyDialog::~PropertyDialog()
{
}

void PropertyDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PROP_WINSIZE, m_windSize);
	DDX_Control(pDX, IDC_PROP_STRIDE, m_stride);
	DDX_Control(pDX, IDC_PROP_MAXKOP, m_maxKOp);
	DDX_Control(pDX, IDC_PROP_MAXOVERLAP, m_maxOverLapF);
	DDX_Control(pDX, IDC_PROP_BFINDEXACT, m_bFindExact);
	DDX_Control(pDX, IDC_PROP_BFINDINEXACT, m_bFindInexact);
	DDX_Control(pDX, IDC_XMLFILE, m_xmlFile);
	DDX_Control(pDX, IDC_PROP_NUM_IMPT_FEAT, m_numImptFeature);
	DDX_Control(pDX, IDC_PROP_NORM_TOKEN, m_normToken);
	DDX_Control(pDX, IDC_PROP_FRAC_IMPT_FEAT, m_fracImptFeat);
	DDX_Control(pDX, IDC_PROP_KEY_VEC_SIZE, m_keyVecSize);
	DDX_Control(pDX, IDC_PROP_INEXACT_PAIR_THRES, m_inexactPairThres);
	DDX_Control(pDX, IDC_PROP_REG_NORM_LVL, m_regNormLvl);
    DDX_Control(pDX, IDC_PROP_INEXACT_MTD_LVL, m_inexactMTD); 
}

void PropertyDialog::init(const CString & p_str)
{
	m_windSize.SetWindowText(p_str);
}


BEGIN_MESSAGE_MAP(PropertyDialog, CDialogEx)
END_MESSAGE_MAP()


// PropertyDialog message handlers

BOOL PropertyDialog::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  Add extra initialization here
	//AfxGetApp();
	CMDIFrameWnd *pFrame = 
             (CMDIFrameWnd*)AfxGetApp()->m_pMainWnd;

	if( pFrame == NULL)
		return FALSE;

	// Get the active MDI child window.
	CMDIChildWnd *pChild = 
             (CMDIChildWnd *) pFrame->GetActiveFrame();

	if( pChild == NULL)
		return FALSE;

	// Get the active view attached to the active MDI child
	// window.
	CCloneDetectorGUIView *pView = (CCloneDetectorGUIView *) pChild->GetActiveView();
	
	if( pView == NULL)
		return FALSE;
	
	int windSize = pView->m_CloneFiles.getWindSize();
	int stride = pView->m_CloneFiles.getStride();
	int  maxKOp= pView->m_CloneFiles.getMaxKOperands();
	double maxOverlap = pView->m_CloneFiles.getMaxOverlapFraction();
	bool bFindExactClones = pView->m_CloneFiles.getFindExactClones();
	bool bFindInexactClones = pView->m_CloneFiles.getFindInexactClones();
	CString strNormalLevel = pView->m_CloneFiles.getNormalLevel();
    CString strInexactLevel = pView->m_CloneFiles.getInexactLevel(); 
	bool bNormToken = pView->m_CloneFiles.getNormalizetoken();
	int vectSize = pView->m_CloneFiles.getKeyVectorsSize();
	double occurThres = pView->m_CloneFiles.getOccurrenceThrs();

	CString str, res;
	
	res.LoadString(IDS_STRING_WIND_SIZE);
	str.Format(_T(" %d"),windSize);
	m_windSize.SetWindowTextW(res+str);

	res.LoadString(IDS_STRING_STEP_SIZE);
	str.Format(_T(" %d"),stride);
	m_stride.SetWindowTextW(res+str);

	res.LoadString(IDS_STRING_NUM_OF_FEAT);
	str.Format(_T(" %d"),maxKOp);
	m_maxKOp.SetWindowTextW(res+str);

	res.LoadString(IDS_STRING_MAX_OVERLAP);
	str.Format(_T(" %.8f"),maxOverlap);
	m_maxOverLapF.SetWindowTextW(res+str);

	str = (_T("Exact Detection = "));
	bFindExactClones?str+=_T("TRUE"):str+=_T("FALSE");
	m_bFindExact.SetWindowTextW(str);

	str = (_T("Inexact detection = "));
	bFindInexactClones?str+=_T("TRUE"):str+=_T("FALSE");
	m_bFindInexact.SetWindowTextW(str);

	res.LoadStringW(IDS_STRING_NORM_TOKEN);
	bNormToken?str=_T(": TRUE"):str=_T(": FALSE");
	m_normToken.SetWindowTextW(res+str);

	res.LoadString(IDS_STRING_KEY_VECT_SIZE);
	str.Format(_T(" %d"),vectSize);
	m_keyVecSize.SetWindowTextW(res+str);

	res.LoadString(IDS_STRING_INEXACT_PAIR_THRES);
	str.Format(_T(" %.8f"),occurThres);
	m_inexactPairThres.SetWindowTextW(res+str);

	res.LoadString(IDS_STRING_REG_NORM_LEVEL);
	m_regNormLvl.SetWindowTextW(res + _T(" ") + strNormalLevel);

    res.LoadString(IDS_STRING_INEXACT_MTD_LEVEL);
	m_inexactMTD.SetWindowTextW(res + _T(" ") + strInexactLevel);

	m_xmlFile.SetWindowTextW(_T("XML file = ") + pView->m_CloneFiles.getXMLFile());

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}


