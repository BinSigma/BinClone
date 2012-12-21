#pragma once
#include "afxwin.h"
#include <vector>


// SearchTokenDlg dialog


class SearchTokenDlg : public CDialogEx
{
	DECLARE_DYNAMIC(SearchTokenDlg)

public:
	SearchTokenDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~SearchTokenDlg();

// Dialog Data
	enum { IDD = IDD_SEARCH_TOKEN_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

public:
	CComboBox             m_comboBox;
	CString               m_searchStr;
	bool                  m_ok;
	std::vector<CString>  m_strVec;

	afx_msg void OnBnClickedOk();
	virtual BOOL OnInitDialog();
	CString m_dbName;
	CString m_dbUser;
	CString m_dbPwd;
	BOOL m_string;
	BOOL m_constant;
	BOOL m_import;
};
