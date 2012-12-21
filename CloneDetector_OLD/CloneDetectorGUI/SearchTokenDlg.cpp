// SearchTokenDlg.cpp : implementation file
//

#include "stdafx.h"
#include "CloneDetectorGUI.h"
#include "SearchTokenDlg.h"
#include "afxdialogex.h"


// SearchTokenDlg dialog

IMPLEMENT_DYNAMIC(SearchTokenDlg, CDialogEx)

SearchTokenDlg::SearchTokenDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(SearchTokenDlg::IDD, pParent),
	  m_searchStr(_T("")),
	  m_ok(false)
{

}

SearchTokenDlg::~SearchTokenDlg()
{
}

void SearchTokenDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SEARCH_TOKEN_COMBO, m_comboBox);
}


BEGIN_MESSAGE_MAP(SearchTokenDlg, CDialogEx)
	ON_BN_CLICKED(IDOK, &SearchTokenDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// SearchTokenDlg message handlers


void SearchTokenDlg::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
    CString strText;
	DWORD pos = m_comboBox.GetEditSel();
	if( pos != 0)
	{
		// user hit "ENTER"
		m_comboBox.GetWindowText(strText);
		if( strText.GetLength() > 0)
			m_strVec.push_back(strText);
	}
	else
	{
		// user clicked "OK"
		int idx = m_comboBox.GetCurSel();
		if( idx >= 0)
		{
			// select the existing string from the pull down list
			m_comboBox.GetLBText(idx,strText);
		}
		else
		{
			// user typed in the new string
			m_comboBox.GetWindowText(strText);
			if( strText.GetLength() > 0)
				m_strVec.push_back(strText);
		}
	}

	if( strText.GetLength() > 0)
	{
		m_searchStr = strText;
		m_ok = true;
	}

	CDialogEx::OnOK();
}


BOOL SearchTokenDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  Add extra initialization here
	m_searchStr = _T("");
	m_ok = false;

	//load the previous strings
	std::vector<CString>::iterator itr = m_strVec.begin();
	for( ; itr != m_strVec.end(); ++itr)
	{
		m_comboBox.AddString(*itr);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}
