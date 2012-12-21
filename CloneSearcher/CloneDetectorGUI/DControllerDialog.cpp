// DControllerDialog.cpp : implementation file
//

#include "stdafx.h"
#include "CloneDetectorGUI.h"
#include "DControllerDialog.h"
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

   try 
   {    
	   
	   //CCSController controller(pcdControllerDlg->m_dbName, 
		//                        pcdControllerDlg->m_dbUser, 
		//						pcdControllerDlg->m_dbPwd);
    
         
	   CCSParams params; 
	   CCSController * pController = pcdControllerDlg->m_csController; 
	   if( !pController)
	   {
		    AfxMessageBox(_T("CCSController is not valid."), MB_ICONSTOP, 0);					
			pcdControllerDlg->m_result = false;
			return 1;
	   }

       if (!pController->fetchAllParameters(params)) 
	   {
            AfxMessageBox(_T("Failed to fetech parameters"), MB_ICONSTOP, 0);					
			pcdControllerDlg->m_result = false;
			return 1;
       }                      

       const CCSParam* pParam = params.getParamBydbParamID(pcdControllerDlg->m_dbParamID);
       if (!pParam) 
	   {
		   AfxMessageBox(_T("Failed to fetech parameter with dbParamID: "), MB_ICONSTOP, 0);
           return 1;
	   }

	   //const CCSClones* pClones = NULL;
	   if( pcdControllerDlg->m_targetCodefrag != NULL)
	   {
		   CString tmpFile;
		   pcdControllerDlg->m_result = pController->findFragmentClones(pcdControllerDlg->m_targetCodefrag,
			                                                            *pParam, 
																		pcdControllerDlg->m_bFindExactClones,
																		pcdControllerDlg->m_bFindInexactClones, 
																		pcdControllerDlg->m_occurrenceThrs,									
																		pcdControllerDlg->m_pClones,
																		pcdControllerDlg->m_fragTmpFile);
	   }
	   else
	   {
		   const CCSClones* pClones = NULL;
		   pcdControllerDlg->m_result = pController->findFileClones(pcdControllerDlg->m_clonePairFileName,
			                                                        *pParam, 
																	pcdControllerDlg->m_bFindExactClones,
																	pcdControllerDlg->m_bFindInexactClones, 
																	pcdControllerDlg->m_occurrenceThrs, 
																	pcdControllerDlg->m_pClones); 
		   
	   }

	   /* OLD
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
		*/
			
		if( !pcdControllerDlg->m_result)
		{
			AfxMessageBox(_T("Clone Dectector encountered failure, can't continue!"), MB_ICONSTOP, 0);	

			pcdControllerDlg->m_result = false;
		}
		//else
		//	pcdControllerDlg->m_pClones = pClones;
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
