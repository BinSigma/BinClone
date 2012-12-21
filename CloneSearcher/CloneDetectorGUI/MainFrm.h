
// MainFrm.h : interface of the CMainFrame class
//

#pragma once
#include "CloneFiles.h"
#include "afxwin.h"

// PropertyDialog dialog

class PropertyDialog : public CDialogEx
{
	DECLARE_DYNAMIC(PropertyDialog)

public:
	PropertyDialog(CWnd* pParent = NULL);   // standard constructor
	virtual ~PropertyDialog();
	void init(const CString &p_str);

// Dialog Data
	enum { IDD = IDD_PROPERTY };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	CStatic m_property;
	CStatic m_windSize;
	CStatic m_stride;
	CStatic m_maxKOp;
	CStatic m_maxOverLapF;
	CStatic m_bFindExact;
	CStatic m_bFindInexact;
	CStatic m_xmlFile;
	CStatic m_numImptFeature;
	CStatic m_normToken;
	CStatic m_fracImptFeat;
	CStatic m_keyVecSize;
	CStatic m_inexactPairThres;
	CStatic m_regNormLvl;
    CStatic m_inexactMTD;
};


const int nStatusProgress = 0;

class CMainFrame : public CMDIFrameWndEx
{
	DECLARE_DYNAMIC(CMainFrame)
public:
	CMainFrame();

// Attributes
public:

// Operations
public:

// Overrides
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL LoadFrame(UINT nIDResource, DWORD dwDefaultStyle = WS_OVERLAPPEDWINDOW | FWS_ADDTOTITLE, CWnd* pParentWnd = NULL, CCreateContext* pContext = NULL);
	virtual void SetAppName(LPCTSTR Title)
	{
		m_strTitle = Title;
	}

	CMFCStatusBar& GetStatusBar ()
	{
		return m_wndStatusBar;
	}


// Implementation
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // control bar embedded members
	CMFCMenuBar       m_wndMenuBar;
	CMFCToolBar       m_wndToolBar;
	CMFCStatusBar     m_wndStatusBar;
	CMFCToolBarImages m_UserImages;
	CloneFile         m_pCurSelCloneFile;

	int			m_nProgressCurr;
	BOOL		m_bInProgress;

// Generated message map functions
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnWindowManager();
	afx_msg void OnViewCustomize();
	afx_msg LRESULT OnToolbarCreateNew(WPARAM wp, LPARAM lp);
	afx_msg void OnApplicationLook(UINT id);
	afx_msg void OnUpdateApplicationLook(CCmdUI* pCmdUI);
	afx_msg void OnFileProperties();	
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	DECLARE_MESSAGE_MAP()

public:
	void launchClonePairsAsmWind(const CloneFile& p_pCurSelCloneFile, int p_id) const;
	void CMainFrame::launchTokenRefWind(const CString & p_searchTokenStr, 
		                                const CString & p_dbname, 
										const CString & p_dbUser, 
										const CString & p_dbPwd,
										bool            p_string,
										bool            p_constant,
										bool            p_import) const;
	void startProgress();
	void stopProgress();

	PropertyDialog m_propertyDlg;

	
	afx_msg void OnNewSearchtoken();
};

