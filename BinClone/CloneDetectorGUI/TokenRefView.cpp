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

// TokenRefView.cpp : implementation of the CTokenRefView class
//

#include "stdafx.h"
/*
// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
// and search filter handlers and allows sharing of document code with that project.
#ifndef SHARED_HANDLERS
#include "TokenRef.h"
#endif
*/

#include "TokenRefDoc.h"
#include "TokenRefView.h"
#include "CloneFiles.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CTokenRefView

IMPLEMENT_DYNCREATE(CTokenRefView, CRichEditView)

BEGIN_MESSAGE_MAP(CTokenRefView, CRichEditView)
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CTokenRefView::OnFilePrintPreview)
	ON_WM_CONTEXTMENU()
	ON_WM_RBUTTONUP()
END_MESSAGE_MAP()

// CTokenRefView construction/destruction

CTokenRefView::CTokenRefView()
{
	// TODO: add construction code here

}

CTokenRefView::~CTokenRefView()
{
}


// CTokenRefView printing

void CTokenRefView::OnFilePrintPreview()
{
#ifndef SHARED_HANDLERS
	AFXPrintPreview(this);
#endif
}


void CTokenRefView::OnRButtonUp(UINT /* nFlags */, CPoint point)
{
	ClientToScreen(&point);
	OnContextMenu(this, point);
}


// CTokenRefView diagnostics

#ifdef _DEBUG
void CTokenRefView::AssertValid() const
{
	CView::AssertValid();
}

void CTokenRefView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif //_DEBUG

CTokenRefDoc* CTokenRefView::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CTokenRefDoc)));
	return (CTokenRefDoc*)m_pDocument;
}


// CTokenRefView message handlers


void CTokenRefView::OnInitialUpdate()
{
	CRichEditView::OnInitialUpdate();

	// TODO: Add your specialized code here and/or call the base class
}

/*
void CTokenRefView::initTreeView(const CloneFiles::TokenRefsVector_t & p_tokenRefsVec) const
{
	 
}
*/

void CTokenRefView::loadFile(CString & p_filename, CString & p_content, int p_line)
{
	initView(p_filename,p_content);
/*
	CString str;
	str.Format(_T("File = %s and line = %d"),p_file,p_line);
	SetWindowText(str);
	*/
	
}
