#pragma once
#include "afxwin.h"
#include "afxshelltreectrl.h"
#include "afxeditbrowsectrl.h"
#include "afxcmn.h"


// NewDetectDialog dialog

class NewDetectDialog : public CDialogEx
{
	DECLARE_DYNAMIC(NewDetectDialog)

public:
	NewDetectDialog(bool p_searchCode = false, CWnd* pParent = NULL);   // standard constructor
	virtual ~NewDetectDialog();

    // Dialog Data
	enum { IDD = IDD_NEW_DETECT_DIALOG };

	afx_msg void OnBnClickedBexactTrue();
	afx_msg void OnBnClickedOk();
	//afx_msg void OnBnClickedButtonPathSel();
	afx_msg void OnBnClickedButtonXmlFile();
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnBnClickedOk2();
	afx_msg void OnBnClickedCancel2();
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);

	LRESULT OnCopy(WPARAM, LPARAM);
	LRESULT OnPaste(WPARAM, LPARAM);

	CString m_asmFilePath;
	CString m_targetAsmFile;
	CString m_windSize;
	CString m_stride;
	CString m_maxKOP;
	CString m_maxOVF;
	BOOL    m_bFindExactClonesChk;
	BOOL    m_bFindInexactClonesChk;
	int     m_regNormalizedLevel;
    BOOL    m_bNormalizeTokenChk;
	CString m_keyVectorsSize;
	CString m_occurrenceThrs;	
    int     m_inexactMethodLevel; 
	CString m_searchCodeFragString;
	CString m_db_param_id;
	CComboBox m_regNormLvl;
    CComboBox m_inexactMTD; 
	bool    m_ok;

	CEdit   m_winSizeCtrl;
	CEdit   m_strideCtrl;
	CEdit   m_maxkCtrl;
	CEdit   m_maxovfCtrl;
    CButton m_normTokCtrl;
	CEdit   m_1stsegperCtrl;
	CEdit   m_keyvectsizeCtrl;
	CEdit   m_occthrsCtrl;
	//CEdit   m_asmFilePathCtrl;
	CEdit   m_targetAsmFileCtrl;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	
	CToolTipCtrl  m_ToolTip;
	bool          m_withSearchCode;
	CRichEditCtrl m_editSearchCodeFrag;

	DECLARE_MESSAGE_MAP()

public:
	CButton m_keepTempFileChkCtrl;
	BOOL m_keepTempFileChk;
    afx_msg void OnCbnSelchangeRegnlvlCombo();
    afx_msg void OnStnClickedStaticKeyVectSize();
    afx_msg void OnEnChangeEditKeyvectsize();
	
	
	afx_msg void OnEnChangeDbParamId();
	
	afx_msg void OnBnClickedCancel();
	CString m_dbPwd;
	CString m_dbName;
	CString m_dbUser;
	afx_msg void OnBnClickedChkinexact();
};


