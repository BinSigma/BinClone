
// CloneDetectorGUIDoc.cpp : implementation of the CCloneDetectorGUIDoc class
//
#include "stdafx.h"

// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
// and search filter handlers and allows sharing of document code with that project.
#ifndef SHARED_HANDLERS
#include "CloneDetectorGUI.h"
#endif

#include "CloneDetectorGUIDoc.h"
#include "NewDetectDialog.h"
#include "DControllerDialog.h"
#include "CDController.h"
#include "CloneFiles.h"
#include "BFFileHelper.h"
#include <propkey.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CCloneDetectorGUIDoc

IMPLEMENT_DYNCREATE(CCloneDetectorGUIDoc, CDocument)

BEGIN_MESSAGE_MAP(CCloneDetectorGUIDoc, CDocument)
END_MESSAGE_MAP()


// CCloneDetectorGUIDoc construction/destruction

CCloneDetectorGUIDoc::CCloneDetectorGUIDoc()
	:m_xmlFilePathAndName(_T("")),
	 m_tmpTargetFragAsmFile(_T("")),
	 m_keepTmpFile(false)
{
	// TODO: add one-time construction code here
}

CCloneDetectorGUIDoc::~CCloneDetectorGUIDoc()
{
	if( (m_tmpTargetFragAsmFile.GetLength() > 0) && !m_keepTmpFile)
	{
		CBFFileHelper::removeFile(m_tmpTargetFragAsmFile);
	}
}

BOOL CCloneDetectorGUIDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;
	
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
			LPCTSTR pSearchCodeFrag       = NULL;
			bool keepTmpFile              = false;
			
			if( bSearchTargetFrag )
			{
				pSearchCodeFrag = newDetectDlg.m_searchCodeFragString;
				keepTmpFile = newDetectDlg.m_keepTempFileChk ? true : false; 
				if( keepTmpFile) 
					m_keepTmpFile = true;
			}
			
			CMainFrame* pFrame = (CMainFrame*) AfxGetMainWnd();
			pFrame->startProgress();

			CDControllerDialog cdControlDlg;
		    cdControlDlg.init(newDetectDlg.m_asmFilePath,
				              newDetectDlg.m_xmlFileName,
							  windSize,
							  stride,
							  maxKOpLvl,
							  ovLap,
							  bfindExact,
							  bfindInExact,
							  (TCDRegNormalizeLevel) regNormLvl,
							  bNormalizeToken,
							  keyVectorsSize,
							  occurrenceThrs,
                              (CDInexactMethod) inexactMTD, 
							  pSearchCodeFrag);

			cdControlDlg.DoModal();
			pFrame->stopProgress();									   
			if( cdControlDlg.m_result == true)
			{
				m_xmlFilePathAndName = newDetectDlg.m_xmlFileName;
                theApp.setCurrXMLFile(m_xmlFilePathAndName);
				m_tmpTargetFragAsmFile = cdControlDlg.m_tmpAsmFilePath;
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

BOOL CCloneDetectorGUIDoc::OnOpenDocument(LPCTSTR lpszPathName)
{
	m_xmlFilePathAndName = lpszPathName;

	theApp.setCurrXMLFile(m_xmlFilePathAndName);

	return TRUE;
}

const CString & CCloneDetectorGUIDoc::getXmlFilePathAndName()
{
	return m_xmlFilePathAndName;
}

// CCloneDetectorGUIDoc serialization

void CCloneDetectorGUIDoc::Serialize(CArchive& ar)
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


#ifdef SHARED_HANDLERS

// Support for thumbnails
void CCloneDetectorGUIDoc::OnDrawThumbnail(CDC& dc, LPRECT lprcBounds)
{
	// Modify this code to draw the document's data
	dc.FillSolidRect(lprcBounds, RGB(255, 255, 255));

	CString strText = _T("TODO: implement thumbnail drawing here");
	LOGFONT lf;

	CFont* pDefaultGUIFont = CFont::FromHandle((HFONT) GetStockObject(DEFAULT_GUI_FONT));
	pDefaultGUIFont->GetLogFont(&lf);
	lf.lfHeight = 36;

	CFont fontDraw;
	fontDraw.CreateFontIndirect(&lf);

	CFont* pOldFont = dc.SelectObject(&fontDraw);
	dc.DrawText(strText, lprcBounds, DT_CENTER | DT_WORDBREAK);
	dc.SelectObject(pOldFont);
}

// Support for Search Handlers
void CCloneDetectorGUIDoc::InitializeSearchContent()
{
	CString strSearchContent;
	// Set search contents from document's data. 
	// The content parts should be separated by ";"

	// For example:  strSearchContent = _T("point;rectangle;circle;ole object;");
	SetSearchContent(strSearchContent);
}

void CCloneDetectorGUIDoc::SetSearchContent(const CString& value)
{
	if (value.IsEmpty())
	{
		RemoveChunk(PKEY_Search_Contents.fmtid, PKEY_Search_Contents.pid);
	}
	else
	{
		CMFCFilterChunkValueImpl *pChunk = NULL;
		ATLTRY(pChunk = new CMFCFilterChunkValueImpl);
		if (pChunk != NULL)
		{
			pChunk->SetTextValue(PKEY_Search_Contents, value, CHUNK_TEXT);
			SetChunkValue(pChunk);
		}
	}
}

#endif // SHARED_HANDLERS

// CCloneDetectorGUIDoc diagnostics

#ifdef _DEBUG
void CCloneDetectorGUIDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CCloneDetectorGUIDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CCloneDetectorGUIDoc commands
