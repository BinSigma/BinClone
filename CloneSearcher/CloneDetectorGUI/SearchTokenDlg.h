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
