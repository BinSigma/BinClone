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
#include "CloneFiles.h"
#include "CSClone.h"
#include "CSController.h"



// ClonePairsAsmDoc document

class ClonePairsAsmDoc : public CDocument
{
	DECLARE_DYNCREATE(ClonePairsAsmDoc)

public:
	ClonePairsAsmDoc();
	virtual ~ClonePairsAsmDoc();
#ifndef _WIN32_WCE
	virtual void Serialize(CArchive& ar);   // overridden for document i/o
#endif
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

	void init(const CloneFile * p_clonefile);

	CCSController* m_csController;

	const CCSClones* m_pClones;

	CString m_targetFilePathAndName;

	CString m_currentTarFilePathAndName;
	CString m_currentSrcFilePathAndName;

protected:
	virtual BOOL OnNewDocument();

	CloneFile    m_pCurSelCloneFile;

	DECLARE_MESSAGE_MAP()

};
