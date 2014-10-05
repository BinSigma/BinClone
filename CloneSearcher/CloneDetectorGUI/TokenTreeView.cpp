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

// TokenTreeView.cpp : implementation file
//

#include "stdafx.h"
#include "CloneDetectorGUI.h"
#include "TokenTreeView.h"
#include "TokenRefFrm.h"
#include "CSController.h"

// TokenTreeView

IMPLEMENT_DYNCREATE(TokenTreeView, CTreeView)

TokenTreeView::TokenTreeView()
{

}

TokenTreeView::~TokenTreeView()
{
}

BEGIN_MESSAGE_MAP(TokenTreeView, CTreeView)
	ON_NOTIFY_REFLECT(TVN_ITEMCHANGED, &TokenTreeView::OnTvnItemChanged)
	ON_COMMAND(ID_TOKEN_VIEW_CLOSE, &TokenTreeView::OnTokenViewClose)
END_MESSAGE_MAP()


// TokenTreeView diagnostics

#ifdef _DEBUG
void TokenTreeView::AssertValid() const
{
	CTreeView::AssertValid();
}

#ifndef _WIN32_WCE
void TokenTreeView::Dump(CDumpContext& dc) const
{
	CTreeView::Dump(dc);
}
#endif
#endif //_DEBUG

CTokenRefDoc* TokenTreeView::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CTokenRefDoc)));
	return (CTokenRefDoc*)m_pDocument;
}


void TokenTreeView::initTreeView(const CString &           p_searchToken,
	                             const CCSIndexedTokens &  p_idxStringTokens,
								 const CCSIndexedTokens &  p_idxConstantTokens,
								 const CCSIndexedTokens &  p_idxImportTokens)
{
	CTreeCtrl & mytreectrl  = GetTreeCtrl();
	
	CString root;
	root.Format(_T("Search Token string: \"%s\""),p_searchToken);
	HTREEITEM rootHitem = mytreectrl.InsertItem(root);

	int numStrings = p_idxStringTokens.GetSize();
	if( numStrings > 0)
	{
		CString type(_T("Strings")); 
		HTREEITEM typeHitem = mytreectrl.InsertItem(type,rootHitem);

		for (int c = 0; c < numStrings; ++c)
		{
			CString tmp;
			int srcId = p_idxStringTokens.GetAt(c)->getDBfileID();
			int line = p_idxStringTokens.GetAt(c)->getRawLineNum();
			CString asmFile;

			CTokenRefFrm* pFrame = (CTokenRefFrm*) GetParentFrame();
			pFrame->m_csController->fetchFilePath(srcId,asmFile);
			
			CString srcFile	= asmFile.Mid(asmFile.ReverseFind(_TCHAR('\\'))+1);

    		CString refString;
			refString.Format(_T("file=%s, line=%d"),srcFile,line); 

			// add to the tree and our own m_refMap
			HTREEITEM refHitem = mytreectrl.InsertItem(refString,typeHitem);
			Reference ref = {srcId,line};
			m_refMap.insert(std::make_pair(refHitem,ref)); //refObj))
		}

    	mytreectrl.Expand(typeHitem,TVE_EXPAND);
	}

	int numConstants = p_idxConstantTokens.GetSize();
	if( numConstants > 0)
	{
		CString type(_T("Constants")); 
		HTREEITEM typeHitem = mytreectrl.InsertItem(type,rootHitem);

		for (int c = 0; c < numConstants; ++c)
		{
			CString tmp;
			int srcId = p_idxConstantTokens.GetAt(c)->getDBfileID();
			int line = p_idxConstantTokens.GetAt(c)->getRawLineNum();
			CString asmFile;

			CTokenRefFrm* pFrame = (CTokenRefFrm*) GetParentFrame();
			pFrame->m_csController->fetchFilePath(srcId,asmFile);
			
			CString srcFile	= asmFile.Mid(asmFile.ReverseFind(_TCHAR('\\'))+1);

    		CString refString;
			refString.Format(_T("file=%s, line=%d"),srcFile,line); 

			// add to the tree and our own m_refMap
			HTREEITEM refHitem = mytreectrl.InsertItem(refString,typeHitem);
			Reference ref = {srcId,line};
			m_refMap.insert(std::make_pair(refHitem,ref)); //refObj))
		}

		mytreectrl.Expand(typeHitem,TVE_EXPAND);
	}

	int numImports = p_idxImportTokens.GetSize();
	if( numImports > 0)
	{
		CString type(_T("Imports")); 
		HTREEITEM typeHitem = mytreectrl.InsertItem(type,rootHitem);

		for (int c = 0; c < numImports; ++c)
		{
			CString tmp;
			int srcId = p_idxImportTokens.GetAt(c)->getDBfileID();
			int line = p_idxImportTokens.GetAt(c)->getRawLineNum();
			CString asmFile;

			CTokenRefFrm* pFrame = (CTokenRefFrm*) GetParentFrame();
			pFrame->m_csController->fetchFilePath(srcId,asmFile);
			
			CString srcFile	= asmFile.Mid(asmFile.ReverseFind(_TCHAR('\\'))+1);

    		CString refString;
			refString.Format(_T("file=%s, line=%d"),srcFile,line); 

			// add to the tree and our own m_refMap
			HTREEITEM refHitem = mytreectrl.InsertItem(refString,typeHitem);
			Reference ref = {srcId,line};
			m_refMap.insert(std::make_pair(refHitem,ref)); //refObj))
		}

		mytreectrl.Expand(typeHitem,TVE_EXPAND);
	}

	mytreectrl.Expand(rootHitem,TVE_EXPAND);

}


void TokenTreeView::initTreeView(const CString &                        p_searchToken,
	                             const CloneFiles &                     p_cloneFiles, 
		                         const CloneFiles::TokenRefsVector_t &  p_tokenRefsVec) 

{
	CTreeCtrl & mytreectrl  = GetTreeCtrl();
	// initialize the assembly filename index map
	p_cloneFiles.getAsmFilesMap(m_asmFilesMap);

	CString root;
	root.Format(_T("Search Token string: \"%s\""),p_searchToken);
	HTREEITEM rootHitem = mytreectrl.InsertItem(root);
	
	CloneFiles::TokenRefsVector_t::const_iterator tokenRefVectItr    = p_tokenRefsVec.begin();
	CloneFiles::TokenRefsVector_t::const_iterator tokenRefVectEndItr = p_tokenRefsVec.end();
	for( ; tokenRefVectItr != tokenRefVectEndItr; ++tokenRefVectItr)
	{
		CString type(tokenRefVectItr->m_type); 
		HTREEITEM refHitem = mytreectrl.InsertItem(type,rootHitem);
		
		// insert the token index nodes
		std::vector<IndexToken>::const_iterator tokenIdxItr    = tokenRefVectItr->m_indexTokenVector.begin();
		std::vector<IndexToken>::const_iterator tokenIdxEndItr = tokenRefVectItr->m_indexTokenVector.end();
		for(; tokenIdxItr != tokenIdxEndItr; ++tokenIdxItr)
		{
			CString token(tokenIdxItr->m_token); 
			HTREEITEM tokeIdxHitem = mytreectrl.InsertItem(token,refHitem);

			// insert the reference nodes
			std::vector<Reference>::const_iterator referenceItr    = tokenIdxItr->m_reference.begin();
		    std::vector<Reference>::const_iterator referenceEndItr = tokenIdxItr->m_reference.end();
			for(; referenceItr != referenceEndItr; ++referenceItr)
			{
				// get the fileId and line
				int fileId(referenceItr->m_fileId);
				int line(referenceItr->m_line);
				CString asmFile(m_asmFilesMap[fileId]);
				asmFile = asmFile.Mid(asmFile.ReverseFind(_T('\\'))+1);
				
				// format the item to be shown on a tree leap
				CString ref;
				ref.Format(_T("file=%s, line=%d"),asmFile,line); 

				// add to the tree and our own m_refMap
				HTREEITEM refHitem = mytreectrl.InsertItem(ref,tokeIdxHitem);
				m_refMap.insert(std::make_pair(refHitem,*(referenceItr))); //refObj));
			}
		}
		mytreectrl.Expand(refHitem,TVE_EXPAND);
	}
}


// TokenTreeView message handlers

void TokenTreeView::OnInitialUpdate()
{
	CTreeView::OnInitialUpdate();
	CTreeCtrl & mytreectrl  = GetTreeCtrl();
	mytreectrl.ModifyStyle ( 0, TVS_HASLINES | TVS_HASBUTTONS | TVS_LINESATROOT);

	CTokenRefFrm* pFrame = (CTokenRefFrm*) GetParentFrame();
	pFrame->ShowWindow(SW_SHOWMAXIMIZED);
	// TODO: Add your specialized code here and/or call the base class
}


void TokenTreeView::OnTvnItemChanged(NMHDR *pNMHDR, LRESULT *pResult)
{
	NMTVITEMCHANGE *pNMTVItemChange = reinterpret_cast<NMTVITEMCHANGE*>(pNMHDR);

	// TODO: Add your control notification handler code here
	CTreeCtrl & mytreectrl  = GetTreeCtrl();
	HTREEITEM hItem = mytreectrl.GetSelectedItem();
	if ((hItem != NULL) && !mytreectrl.ItemHasChildren(hItem))
	{
		hItemRefMap_t::iterator itr = m_refMap.find(hItem);
		if( itr != m_refMap.end())
		{
			int fileId(itr->second.m_fileId);
			int line(itr->second.m_line);
			CTokenRefFrm* pFrame = (CTokenRefFrm*) GetParentFrame();
			pFrame->selectAsmFileAndLine(fileId,line);
		}
	}

	*pResult = 0;
}


void TokenTreeView::OnTokenViewClose()
{
	// TODO: Add your command handler code here
	GetParentFrame()->PostMessage(WM_CLOSE);
}
