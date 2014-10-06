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
#include "..\CloneSearcherLib\CSController.h"


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

	bool bSearchTargetFrag = theApp.getNewCDSearchFlag();
	CString initialFragStr("");
	if( bSearchTargetFrag)
	{
		initialFragStr = theApp.getInitalFragStr();
	}
	NewDetectDialog newDetectDlg(bSearchTargetFrag,initialFragStr);
	//NewDetectDialog newDetectDlg(bSearchTargetFrag);
	newDetectDlg.DoModal();
	if( newDetectDlg.m_ok)
	{
		CBFStrHelper strHelper;
		try
		{
			bool bfindExact               = newDetectDlg.m_bFindExactClonesChk ? true : false;
			bool bfindInExact             = newDetectDlg.m_bFindInexactClonesChk ? true : false;			
			double occurrenceThrs         = strHelper.strToFloat(newDetectDlg.m_occurrenceThrs);
            bool bNormalizeToken          = newDetectDlg.m_bNormalizeTokenChk ? true : false;
            int inexactMTD                = newDetectDlg.m_inexactMethodLevel; 
			int dbParamId                 = newDetectDlg.m_db_param_id;
			LPCTSTR pSearchCodeFrag       = NULL;
			bool keepTmpFile              = false;
			CString dbName                = newDetectDlg.m_dbName;
			CString dbUser                = newDetectDlg.m_dbUser;
			CString dbPwd                 = newDetectDlg.m_dbPwd;
			
			if( bSearchTargetFrag )
			{
				pSearchCodeFrag = newDetectDlg.m_searchCodeFragString;
				keepTmpFile = newDetectDlg.m_keepTempFileChk ? true : false; 
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


