#pragma once
#include "afxcmn.h"
#include "CSController.h"


// ParamsListDialog dialog

class ParamsListDialog : public CDialogEx
{
	DECLARE_DYNAMIC(ParamsListDialog)

public:
	ParamsListDialog(CWnd* pParent = NULL);   // standard constructor
	virtual ~ParamsListDialog();

// Dialog Data
	enum { IDD = IDD_DIALOG_PARAMS };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CListCtrl m_params_list_ctrl;
	CCSParams m_params; 
	CCSController * m_csController;
	int m_paramId;
	CString m_dbName;
    CString m_dbUser;
    CString m_dbPwd;

	int m_selWndSize;
	int m_selStride;
	int m_selRegNormLvl;

	bool m_result;

	virtual BOOL OnInitDialog();
	afx_msg void OnLvnItemchangedList1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnClickList1(NMHDR *pNMHDR, LRESULT *pResult);
	void init(const CString & dbname, const CString &dbUser, const CString &dbPwd)
	{
		m_dbName = dbname;
		m_dbUser = dbUser;
		m_dbPwd = dbPwd;
	}

	void updateParameters();

	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnNMDblclkList1(NMHDR *pNMHDR, LRESULT *pResult);
};
