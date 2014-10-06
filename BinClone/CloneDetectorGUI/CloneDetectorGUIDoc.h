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

// CloneDetectorGUIDoc.h : interface of the CCloneDetectorGUIDoc class
//


#pragma once

#include "MainFrm.h"


class CCloneDetectorGUIDoc : public CDocument
{
protected: // create from serialization only
	CCloneDetectorGUIDoc();
	DECLARE_DYNCREATE(CCloneDetectorGUIDoc)

// Attributes
public:

// Operations
public:

// Overrides
public:
	virtual BOOL OnNewDocument();
	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
	virtual void Serialize(CArchive& ar);
#ifdef SHARED_HANDLERS
	virtual void InitializeSearchContent();
	virtual void OnDrawThumbnail(CDC& dc, LPRECT lprcBounds);
#endif // SHARED_HANDLERS

// Implementation
public:
	virtual ~CCloneDetectorGUIDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

public:
	const CString & getXmlFilePathAndName();



protected:

	CMFCStatusBar& GetStatusBar () const
	{
		return ((CMainFrame*) AfxGetMainWnd ())->GetStatusBar ();
	}

	
	CString     m_xmlFilePathAndName;
	CString     m_tmpTargetFragAsmFile;
	bool        m_keepTmpFile;
	
// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()

#ifdef SHARED_HANDLERS
	// Helper function that sets search content for a Search Handler
	void SetSearchContent(const CString& value);
#endif // SHARED_HANDLERS
};
