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


// CloneDetectorGUI.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "afxwinappex.h"
#include "afxdialogex.h"
#include "CloneDetectorGUI.h"
#include "MainFrm.h"

#include "ChildFrm.h"
#include "CloneDetectorGUIDoc.h"
#include "CloneDetectorGUIView.h"

#include "ClonePairsAsmFrame.h"
#include "ClonePairsAsmDoc.h"
#include "ClonePairsAsmView.h"

#include "TokenRefDoc.h"
#include "TokenRefFrm.h"

#include "CSController.h"
#include "CloneFiles.h"

#include "NewDetectDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CCloneDetectorGUIApp

BEGIN_MESSAGE_MAP(CCloneDetectorGUIApp, CWinAppEx)
	ON_COMMAND(ID_APP_ABOUT, &CCloneDetectorGUIApp::OnAppAbout)
	ON_COMMAND(ID_FILE_OPEN, &CWinAppEx::OnFileOpen)
	ON_COMMAND(ID_FILE_PRINT_SETUP, &CWinAppEx::OnFilePrintSetup)
	//ON_COMMAND(ID_FILE_NEWCLONEDETECTSEARCH, &CWinAppEx::OnFileNew)
	ON_COMMAND(ID_FILE_NEW_CLONES, &CCloneDetectorGUIApp::OnFileNewClonePairsDetect)
	ON_COMMAND(ID_FILE_NEW_SEARCH_CODE_FRAG, &CCloneDetectorGUIApp::OnFileNewSearchCodeFrag)
	ON_COMMAND(ID_SEARCH_FRAG_BUTTON, &CCloneDetectorGUIApp::OnFileNewSearchCodeFrag)
END_MESSAGE_MAP()


// CCloneDetectorGUIApp construction

CCloneDetectorGUIApp::CCloneDetectorGUIApp()
	: m_pDocTemplateClonePairsAsm(NULL),
	  m_pDocTemplateTokenRef(NULL),
	  m_initFragStr(""),
      m_newCDSearchFlag(false)
{
	m_bHiColorIcons = TRUE;

	// support Restart Manager
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_ALL_ASPECTS;
#ifdef _MANAGED
	// If the application is built using Common Language Runtime support (/clr):
	//     1) This additional setting is needed for Restart Manager support to work properly.
	//     2) In your project, you must add a reference to System.Windows.Forms in order to build.
	System::Windows::Forms::Application::SetUnhandledExceptionMode(System::Windows::Forms::UnhandledExceptionMode::ThrowException);
#endif

	// TODO: replace application ID string below with unique ID string; recommended
	// format for string is CompanyName.ProductName.SubProduct.VersionInformation
	SetAppID(_T("CloneDetectorGUI.AppID.NoVersion"));

	// TODO: add construction code here,
	// Place all significant initialization in InitInstance

}

// The one and only CCloneDetectorGUIApp object

CCloneDetectorGUIApp theApp;


// CCloneDetectorGUIApp initialization
void CCloneDetectorGUIApp::readFragFromFile(const CString &infile, CString &fragStr)
{
	fragStr = "";
	/*
	FILE *fStream;
	errno_t err;
	char *f = ( char *)(LPCTSTR)infile;
	strcpy(f,(const char *)(LPCTSTR)infile);
	
	fStream = fopen("C:\\Users\\benfung\\AppData\\Local\\Temp\\temFrag.ext", "r");

	if(fStream != NULL)
	{
		fseek(fStream,0,SEEK_END);
		int size = ftell(fStream); 
		fseek(fStream,0, SEEK_SET);
		//char * buffer = (char*) malloc(size);
		fread(fragStr.GetBuffer(size),1,size,fStream);
		fragStr.ReleaseBuffer(size);
		fclose(fStream);
	}
	else
	{
		CString err;
		err.Format(_T("Error open framgment file %s"),f);
		AfxMessageBox(err);
	}
	*/

   CFile file;
   CFileException ex;

   if( !file.Open((LPCTSTR)infile,CFile::modeRead | CFile::shareDenyWrite, &ex))
   {
	   TCHAR szError[1024];
	   ex.GetErrorMessage(szError, 1024);
	   TRACE1("Couldn't open source file: %s\n", szError);
	   return;
   }
   else
   {
	   file.SeekToBegin();
	   bool done = false;
       while(!done)
	   {
	       char buf[1025] = {0};
		   UINT nRead = file.Read(buf,1024);
		   if( nRead < 1024)
		   {
			   done = true;
		   };
		   buf[nRead] = '\0';
	       fragStr += CString(buf);
	   }
	   /*
	   UINT nBytes = (UINT)file.GetLength();
	   int nChars = nBytes / sizeof(TCHAR);
	   nBytes = file.Read(fragStr.GetBuffer(nChars), nBytes);
	   fragStr.ReleaseBuffer(nChars);
	   */
   }
   file.Close();
   
}

BOOL CCloneDetectorGUIApp::InitInstance()
{
	// InitCommonControlsEx() is required on Windows XP if an application
	// manifest specifies use of ComCtl32.dll version 6 or later to enable
	// visual styles.  Otherwise, any window creation will fail.
	AfxInitRichEdit();

	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// Set this to include all the common control classes you want to use
	// in your application.
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinAppEx::InitInstance();


	// Initialize OLE libraries
	if (!AfxOleInit())
	{
		AfxMessageBox(IDP_OLE_INIT_FAILED);
		return FALSE;
	}

	AfxEnableControlContainer();

	EnableTaskbarInteraction(FALSE);

	// AfxInitRichEdit2() is required to use RichEdit control	
	// AfxInitRichEdit2();

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	// of your final executable, you should remove from the following
	// the specific initialization routines you do not need
	// Change the registry key under which our settings are stored
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization
	SetRegistryKey(_T("Local AppWizard-Generated Applications"));
	LoadStdProfileSettings(4);  // Load standard INI file options (including MRU)


	InitContextMenuManager();

	InitKeyboardManager();

	InitTooltipManager();
	CMFCToolTipInfo ttParams;
	ttParams.m_bVislManagerTheme = TRUE;
	theApp.GetTooltipManager()->SetTooltipParams(AFX_TOOLTIP_TYPE_ALL,
		RUNTIME_CLASS(CMFCToolTipCtrl), &ttParams);

	// Register the application's document templates.  Document templates
	//  serve as the connection between documents, frame windows and views
	CMultiDocTemplate* pDocTemplate;
	
    pDocTemplate = new CMultiDocTemplate(IDR_MAINFRAME, //IDR_CloneDetectorGUTYPE,
		RUNTIME_CLASS(ClonePairsAsmDoc),
		RUNTIME_CLASS(ClonePairsAsmFrame), // custom MDI child frame
		RUNTIME_CLASS(ClonePairsAsmView));
	if (!pDocTemplate) 
		return FALSE;
	AddDocTemplate(pDocTemplate);
	
	m_pDocTemplateTokenRef = new CMultiDocTemplate(IDR_MAINFRAME, //IDR_TOKEN_REF_MENU,
		RUNTIME_CLASS(CTokenRefDoc),
		RUNTIME_CLASS(CTokenRefFrm), // custom MDI child frame
		RUNTIME_CLASS(ClonePairsAsmView));

	// create main MDI Frame window
	CMainFrame* pMainFrame = new CMainFrame;
	if (!pMainFrame || !pMainFrame->LoadFrame(IDR_MAINFRAME))
	{
		delete pMainFrame;
		return FALSE;
	}
	m_pMainWnd = pMainFrame;
	
	// call DragAcceptFiles only if there's a suffix
	//  In an MDI app, this should occur immediately after setting m_pMainWnd
	
	pMainFrame->ShowWindow(m_nCmdShow);
	
	pMainFrame->UpdateWindow();

	//m_initFragStr = "This is line one\r\nThis is line two\r\n";

	CCmdLineInfo cmdInfo;
    ParseCommandLine(cmdInfo);
	if( cmdInfo.m_fragmentTmpFile && !cmdInfo.m_strFileName.IsEmpty())
	//if(cmdInfo.m_fragmentTmpFile == true)
	{
		//TODO IDA PRO PLUGIN HERE
		readFragFromFile(cmdInfo.m_strFileName,m_initFragStr);
		//m_initFragStr = cmdInfo.m_strFileName; //"This is line one\r\nThis is line two\r\n";
		OnFileNewSearchCodeFrag();
	}
	return TRUE;
}

int CCloneDetectorGUIApp::ExitInstance()
{
	//TODO: handle additional resources you may have added
	AfxOleTerm(FALSE);

	if( m_pDocTemplateClonePairsAsm)
	{
		delete m_pDocTemplateClonePairsAsm;
	}

	if( m_pDocTemplateTokenRef)
	{
		delete m_pDocTemplateTokenRef;
	}


	return CWinAppEx::ExitInstance();
}

void CCloneDetectorGUIApp::setCurrXMLFile(CString & p_xmlFile)
{
	m_currXMLFile = p_xmlFile;
}

const CString & CCloneDetectorGUIApp::getCurrXMLFile()
{
	return m_currXMLFile;
}


// CCloneDetectorGUIApp message handlers


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()

// App command to run the dialog
void CCloneDetectorGUIApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

// CCloneDetectorGUIApp customization load/save methods

void CCloneDetectorGUIApp::PreLoadState()
{
	BOOL bNameValid;
	CString strName;
	bNameValid = strName.LoadString(IDS_EDIT_MENU);
	ASSERT(bNameValid);
	GetContextMenuManager()->AddMenu(strName, IDR_POPUP_EDIT);
}

void CCloneDetectorGUIApp::LoadCustomState()
{
}

void CCloneDetectorGUIApp::SaveCustomState()
{
}

// CCloneDetectorGUIApp message handlers
void CCloneDetectorGUIApp::OnFileNewClonePairsDetect()
{
	// TODO: Add your command handler code here
	m_newCDSearchFlag = false;
    OnFileNew();
}


void CCloneDetectorGUIApp::OnFileNewSearchCodeFrag()
{
	m_newCDSearchFlag = true;
	// TODO: Add your command handler code here
	OnFileNew();
}

CCmdLineInfo::CCmdLineInfo()
	:m_fragmentTmpFile(false)
	,m_bBoolParam(false)
	,m_bHelpParam(false)
	//,m_strFileParam("")
{
}
 
void CCmdLineInfo::ParseParam(const TCHAR* pszParam, BOOL bFlag, BOOL bLast)
{
    bool bHandled = false;
    const TCHAR *lpszParse = pszParam;
    if (bFlag)
    {
        switch (*lpszParse)
        {
        case _T('f') :
			m_fragmentTmpFile=true;
			/*
            ++lpszParse;
            if (*lpszParse == _T('='))
                ++lpszParse;
            m_strFileParam = lpszParse;
            bHandled = true;
			*/
            break;
        }
    }
	else if( m_fragmentTmpFile)
	{
		//m_strFileParam = lpszParse;
		m_strFileName = lpszParse;
	}
	
    // If the last parameter has no flag, it is treated as the file name to be
    //  opened and the string is stored in the m_strFileName member.
	/*
	if (!bHandled)
        CCommandLineInfo::ParseParam(pszParam, bFlag, bLast);
		*/
}