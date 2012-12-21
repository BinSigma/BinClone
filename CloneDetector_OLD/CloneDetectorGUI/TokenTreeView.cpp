// TokenTreeView.cpp : implementation file
//

#include "stdafx.h"
#include "CloneDetectorGUI.h"
#include "TokenTreeView.h"
#include "TokenRefFrm.h"
#include "..\..\CloneDetector\CloneDetectorLib\CDController.h"

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
