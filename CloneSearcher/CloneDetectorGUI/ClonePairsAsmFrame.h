#pragma once

#include "CloneFiles.h"
#include "CSClone.h"
#include "CSController.h"


#ifdef _WIN32_WCE
#error "CMDIChildWnd is not supported for Windows CE."
#endif 

// ClonePairsAsmFrame frame with splitter

class ClonePairsAsmFrame : public CMDIChildWnd
{
	DECLARE_DYNCREATE(ClonePairsAsmFrame)


public:
	
	bool init(); 
	int getCurSelLine(){ return m_currentLine;}
	void setCurSelLine(unsigned int p_line) { m_currentLine=p_line;}
	void setNumOfClonePairs(unsigned int p_num) {m_numOfClonePairs = p_num;}
	unsigned int getNumOfClonePairs() { return m_numOfClonePairs;}
	bool fillSelectedClonePairsOnViews(int p_selLine);

	// version 2.0
	void displayAsmContents(const CString & tarFile, const CString & tarContent, const CString & srcFile, const CString & srcContent);
	void displayTarAsmContents(const CString & tarFile, const CString & tarContent);
	void displaySrcAsmContents(const CString & srcFile, const CString & srcContent);
	bool fillSelectedClonePairsOnViews2(int listLine, int tarStart, int tarEnd, int srcStart, int srcEnd);
	void SyncScroll(int fromView, UINT nScrollCode);
		                                
	bool selectedParticularLine(int p_line);
	CString & getXMLFile(){ return m_xmlFile;}
	int getCurId(){ return m_idFromXmlFile;}

	std::vector<CString> asmA;
	std::vector<CString> asmB;

	CCSController* m_csController;

	const CCSClones* m_pClones;

	CString m_targetFilePathAndName;

	bool m_syncScroll;

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

	unsigned int m_numOfClonePairs;

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
	//virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	afx_msg void OnViewSynchronizedscrolling();
	afx_msg void OnUpdateViewSynchronizedscrolling(CCmdUI *pCmdUI);
};


