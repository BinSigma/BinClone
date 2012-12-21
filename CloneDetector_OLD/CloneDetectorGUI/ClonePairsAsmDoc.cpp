// ClonePairsAsmDoc.cpp : implementation file
//

#include "stdafx.h"
#include "CloneDetectorGUI.h"
#include "ClonePairsAsmDoc.h"
#include "CloneFiles.h"
#include "..\..\CloneDetector\CloneDetectorLib\CDController.h"


// ClonePairsAsmDoc

IMPLEMENT_DYNCREATE(ClonePairsAsmDoc, CDocument)

ClonePairsAsmDoc::ClonePairsAsmDoc()
{
}

BOOL ClonePairsAsmDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;
	return TRUE;
}

ClonePairsAsmDoc::~ClonePairsAsmDoc()
{
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
