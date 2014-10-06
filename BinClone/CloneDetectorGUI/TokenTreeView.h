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

#pragma once

#include "TokenRefDoc.h"
#include "CSIndexedToken.h"
#include "CloneFiles.h"


// TokenTreeView view

class TokenTreeView : public CTreeView
{
	DECLARE_DYNCREATE(TokenTreeView)

protected:
	TokenTreeView();           // protected constructor used by dynamic creation
	virtual ~TokenTreeView();

public:

	CTokenRefDoc* GetDocument() const;

	CloneFiles::TokenRefsVector_t m_tokenRefsVec;

#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

public:
    typedef std::map<HTREEITEM,Reference>  hItemRefMap_t;

	// v2.0
	void initTreeView(const CString &           p_searchToken,
	                  const CCSIndexedTokens &  p_idxStringTokens,
					  const CCSIndexedTokens &  p_idxConstantTokens,
					  const CCSIndexedTokens &  p_idxImportTokens);


	void initTreeView(const CString &                        p_searchToken,
		              const CloneFiles &                     p_cloneFiles, 
		              const CloneFiles::TokenRefsVector_t &  p_tokenRefsVec);
	 
protected:
	CloneFiles::AsmFilesMap_t m_asmFilesMap;
	hItemRefMap_t m_refMap;

	DECLARE_MESSAGE_MAP()

public:
	virtual void OnInitialUpdate();
	afx_msg void OnTvnItemChanged(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnTokenViewClose();
};


