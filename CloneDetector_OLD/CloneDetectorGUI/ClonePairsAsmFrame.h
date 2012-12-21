#pragma once

#include "CloneFiles.h"

#ifdef _WIN32_WCE
#error "CMDIChildWnd is not supported for Windows CE."
#endif 

// ClonePairsAsmFrame frame with splitter

class ClonePairsAsmFrame : public CMDIChildWnd
{
	DECLARE_DYNCREATE(ClonePairsAsmFrame)

public:
	void init(const CloneFile & p_clonefile, const CString & p_xmlFile, int p_id);
	int getCurSelLine(){ return m_currentLine;}
	bool fillSelectedClonePairsOnViews(int p_selLine);
	bool selectedParticularLine(int p_line);
	CString & getXMLFile(){ return m_xmlFile;}
	int getCurId(){ return m_idFromXmlFile;}

	std::vector<CString> asmA;
	std::vector<CString> asmB;

protected:
	ClonePairsAsmFrame();           // protected constructor used by dynamic creation
	virtual ~ClonePairsAsmFrame();

	CMFCToolBar  m_wndToolBar;
	CSplitterWnd m_wndSplitter;
	CSplitterWnd m_wndSplitter2;
	bool         m_bInitSplitter;
	
	CloneFile    m_pCurSelCloneFile;
	//int          m_numOfClonePairs;

	CString      m_xmlFile;
	int          m_idFromXmlFile;

protected:
	virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);
	
    int m_currentLine;
	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnViewNext();
	afx_msg void OnViewPrevious();
	afx_msg void OnButtonNext();
	afx_msg void OnButtonPrev();
};


