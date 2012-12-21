#pragma once

#include "CSClone.h"
#include "CSController.h"

// CDControllerDialog dialog


static UINT NEAR WM_STOPCDCONTROL = RegisterWindowMessage(_T("Stop CDController"));

class CDControllerDialog : public CDialogEx
{
	DECLARE_DYNAMIC(CDControllerDialog)

public:
	CDControllerDialog(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDControllerDialog();

	void init(const CString& assemblyFolderPath,
              const CString& clonePairsFilePath,
              int windowSize,
              int stride,
              int kThreshold,
              double oThreshold,
              bool bFindExactClones,
              bool bFindInexactClones,
//              TCDRegNormalizeLevel regNormLevel,
              bool bNormalizeToken,
	          int keyVectorsSize,
	          double occurrenceThrs,
//              CDInexactMethod inexactMethod, 
              LPCTSTR targetFragment=NULL)
	{
		m_folderPath = assemblyFolderPath;
	    m_clonePairFileName = clonePairsFilePath;
	    m_windSize = windowSize;
        m_stride = stride;
        m_maxKOperands = kThreshold;
		m_maxOverlapFraction =	oThreshold;
        m_bFindExactClones = bFindExactClones;
        m_FindInexactClones = bFindInexactClones;
//        m_normalLevel = regNormLevel;
        m_bNormalizeToken = bNormalizeToken,
	    m_keyVectorsSize = keyVectorsSize;
	    m_occurrenceThrs = occurrenceThrs;
//        m_inexactLevel = inexactMethod; 
      	m_targetCodefrag = targetFragment;
		m_fragTmpFile = _T("");
	
		m_init = true;
	}

	void init( CCSController *  csController,
			   const CString &  targetAssemblyFilePath,
			   bool             bFindExactClones, 
               bool             bFindInexactClones, 
               double           minCoOccThreshold,
			   int              dbParamID,
			   const CCSClones* pClones,
			   LPCTSTR targetFragment=NULL)

	{
		//m_dbName = dbName;
		//m_dbUser = dbUser;
		//m_dbPwd = dbPwd;
		m_csController = csController;
        m_bFindExactClones = bFindExactClones;
        m_bFindInexactClones = bFindInexactClones;
        m_occurrenceThrs = minCoOccThreshold;
        m_clonePairFileName = targetAssemblyFilePath;
		m_dbParamID = dbParamID;
		m_pClones = pClones;
		m_targetCodefrag = targetFragment;

        m_init = true;
	}



	// Dialog Data
	enum { IDD = IDD_CDCONTROLLER_DIALOG };

protected:
	
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	afx_msg LRESULT OnStopCDControl(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()

public:
	bool                 m_cancel;
	bool                 m_init;
	bool                 m_result;
	CString              m_folderPath;
	CString              m_clonePairFileName;
    int                  m_windSize;
    int		             m_stride;
    int		             m_maxKOperands;
    double               m_maxOverlapFraction;
    bool                 m_bFindExactClones;
    bool                 m_FindInexactClones;
	bool                 m_bFindInexactClones;
//	TCDRegNormalizeLevel m_normalLevel;
    bool                 m_bNormalizeToken;
	int                  m_keyVectorsSize;
	double               m_occurrenceThrs;
//    CDInexactMethod      m_inexactLevel; 
    LPCTSTR              m_targetCodefrag;
	CString              m_fragTmpFile;

	CString              m_tmpAsmFilePath;

	afx_msg void OnBnClickedCancel();
	virtual BOOL OnInitDialog();

	CString	m_dbName;
	CString	m_dbUser;
	CString	m_dbPwd;
	int     m_dbParamID;
	CCSController * m_csController;

    const CCSClones* m_pClones;

private:

	CWinThread         *m_thread;

	
};
