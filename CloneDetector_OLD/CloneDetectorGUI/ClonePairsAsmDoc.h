#pragma once
#include "CloneFiles.h"

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


protected:
	virtual BOOL OnNewDocument();

	CloneFile    m_pCurSelCloneFile;

	DECLARE_MESSAGE_MAP()
};
