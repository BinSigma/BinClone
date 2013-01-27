// ClonePairsAsmDoc.cpp : implementation file
//

#include "stdafx.h"



#include "CloneDetectorGUI.h"

#include "MainFrm.h"
#include "ClonePairsAsmDoc.h"
#include "NewDetectDialog.h"
#include "DControllerDialog.h"
#include "CSController.h"
#include "CloneFiles.h"
#include "BFFileHelper.h"
#include "CloneFiles.h"
#include "..\..\CloneSearcher\CloneSearcherLib\CSController.h"


// ClonePairsAsmDoc

IMPLEMENT_DYNCREATE(ClonePairsAsmDoc, CDocument)

ClonePairsAsmDoc::ClonePairsAsmDoc()
:m_csController(NULL),
 m_targetFilePathAndName(_T("")),
 m_currentTarFilePathAndName(_T("")),
 m_currentSrcFilePathAndName(_T(""))
{
}

BOOL ClonePairsAsmDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	//return TRUE;

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
			//int regNormLvl                = newDetectDlg.m_regNormalizedLevel;
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

			m_csController = new CCSController( dbName, dbUser, dbPwd);
			CDControllerDialog cdControlDlg;
			if( bSearchTargetFrag) 
			{
				m_targetFilePathAndName = _T("");
				cdControlDlg.init(m_csController, m_targetFilePathAndName,bfindExact,bfindInExact,occurrenceThrs,dbParamId,m_pClones,pSearchCodeFrag);
			}
			else
			{
			    m_targetFilePathAndName = newDetectDlg.m_targetAsmFile; // "Z:\\Security20121108b\\exp\\Data02\\btwhidcs.asm";
				cdControlDlg.init(m_csController, m_targetFilePathAndName,bfindExact,bfindInExact,occurrenceThrs,dbParamId,m_pClones);
			}
			
			cdControlDlg.DoModal();
			pFrame->stopProgress();									   
			if( cdControlDlg.m_result == true)
			{
				if( bSearchTargetFrag)
					m_targetFilePathAndName = cdControlDlg.m_fragTmpFile; 

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

	//return TRUE;
}

ClonePairsAsmDoc::~ClonePairsAsmDoc()
{
    if(m_csController)
		delete m_csController;
}


BEGIN_MESSAGE_MAP(ClonePairsAsmDoc, CDocument)
END_MESSAGE_MAP()


// ClonePairsAsmDoc diagnostics

#ifdef _DEBUG
void ClonePairsAsmDoc::AssertValid() const
{
	CDocument::AssertValid();
}

#ifndef _WIN32_WCE
void ClonePairsAsmDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif
#endif //_DEBUG

#ifndef _WIN32_WCE
// ClonePairsAsmDoc serialization

void ClonePairsAsmDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}
#endif

void ClonePairsAsmDoc::init(const CloneFile * p_clonefile)
{
	if( p_clonefile != NULL)
	{
	   m_pCurSelCloneFile = *p_clonefile;
	}
}


// ClonePairsAsmDoc commands
