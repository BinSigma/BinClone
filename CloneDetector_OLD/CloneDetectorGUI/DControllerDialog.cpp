// DControllerDialog.cpp : implementation file
//

#include "stdafx.h"
#include "CloneDetectorGUI.h"
#include "DControllerDialog.h"
#include "CDController.h"
#include "afxdialogex.h"


// CDControllerDialog dialog

IMPLEMENT_DYNAMIC(CDControllerDialog, CDialogEx)

UINT RunCDControllerThread( LPVOID pParam )
{   
   CDControllerDialog *pcdControllerDlg = (CDControllerDialog*) pParam;
   if( !pcdControllerDlg->m_init) {
       pcdControllerDlg->PostMessage(WM_STOPCDCONTROL);
	   return 1;
   }

   try {    
	    CCDController cloneDetectorObj(pcdControllerDlg->m_folderPath,
			                           pcdControllerDlg->m_clonePairFileName,
								       pcdControllerDlg->m_windSize,
								       pcdControllerDlg->m_stride,
								       pcdControllerDlg->m_maxKOperands,
								       pcdControllerDlg->m_maxOverlapFraction,
								       pcdControllerDlg->m_bFindExactClones,
								       pcdControllerDlg->m_FindInexactClones,
								       pcdControllerDlg->m_normalLevel,
                                       pcdControllerDlg->m_bNormalizeToken,
								       pcdControllerDlg->m_keyVectorsSize,
								       pcdControllerDlg->m_occurrenceThrs,
                                       pcdControllerDlg->m_inexactLevel); 

		if( pcdControllerDlg->m_targetCodefrag != NULL)
		{
			pcdControllerDlg->m_result = cloneDetectorObj.searchFragments(pcdControllerDlg->m_targetCodefrag,
				                                                          pcdControllerDlg->m_tmpAsmFilePath);
		}
		else
		{
			pcdControllerDlg->m_result = cloneDetectorObj.detectClones();
		}
			
		if( !pcdControllerDlg->m_result)
		{
			AfxMessageBox(_T("Clone Dectector encountered failure, can't continue!"), MB_ICONSTOP, 0);					
			pcdControllerDlg->m_result = false;
		}
   }
   catch (CException *e) {
      TCHAR msg[255];
      e->GetErrorMessage(msg, 255);
	  CString err;
	  err.Format(_T("Caught exception while running Clone Dectector, error = %s"),msg);
	  AfxMessageBox(err,MB_ICONSTOP,0);
      e->Delete();
   }
   catch (...) {
	   AfxMessageBox(_T("Caught unknown exception while running Clone Dectector"),MB_ICONSTOP,0);
   }
   pcdControllerDlg->PostMessage(WM_STOPCDCONTROL);   
   return 1;
}

CDControllerDialog::CDControllerDialog(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDControllerDialog::IDD, pParent),
	  m_cancel(false),
	  m_init(false),
	  m_result(false),
	  m_tmpAsmFilePath(_T(""))
{

}

CDControllerDialog::~CDControllerDialog()
{
}

void CDControllerDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDControllerDialog, CDialogEx)
	ON_BN_CLICKED(IDCANCEL, &CDControllerDialog::OnBnClickedCancel)
	ON_REGISTERED_MESSAGE(WM_STOPCDCONTROL, &CDControllerDialog::OnStopCDControl)
END_MESSAGE_MAP()


// CDControllerDialog message handlers


void CDControllerDialog::OnBnClickedCancel()
{
	// TODO: Add your control notification handler code here
	int res = AfxMessageBox( _T("Abort the operation? \nThe detectClone() process may still be running in the background."),
		                     MB_YESNO|MB_ICONHAND,0);			
	if( res == IDYES)
	{
	    DWORD dwExitCode=0;
	    TerminateThread(m_thread, dwExitCode);
	    m_cancel = true;
	    CDialogEx::OnCancel();
	}
}


BOOL CDControllerDialog::OnInitDialog()
{
	if( !m_init)
		return FALSE;
     
	CDialogEx::OnInitDialog();
	

	// start the thread to run CDController
	m_thread = AfxBeginThread(RunCDControllerThread, this);

	// TODO:  Add extra initialization here

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}


afx_msg LRESULT CDControllerDialog::OnStopCDControl(WPARAM wParam, LPARAM lParam)
{
	DWORD dwExitCode=0;
	TerminateThread(m_thread, dwExitCode);
	CDialogEx::OnCancel();
	return 0;
}
