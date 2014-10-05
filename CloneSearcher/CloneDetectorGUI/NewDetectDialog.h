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
#include "afxshelltreectrl.h"
#include "afxeditbrowsectrl.h"
#include "afxcmn.h"


// NewDetectDialog dialog

class NewDetectDialog : public CDialogEx
{
	DECLARE_DYNAMIC(NewDetectDialog)

public:
	NewDetectDialog(bool p_searchCode, CString &initialStr, CWnd* pParent = NULL);   // standard constructor

	virtual ~NewDetectDialog();

    // Dialog Data
	enum { IDD = IDD_NEW_DETECT_DIALOG };

	afx_msg void OnBnClickedBexactTrue();
	afx_msg void OnBnClickedOk();
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
	
	BOOL    m_bFindExactClonesChk;
	BOOL    m_bFindInexactClonesChk;
	int     m_regNormalizedLevel;
    BOOL    m_bNormalizeTokenChk;
	CString m_occurrenceThrs;	
    int     m_inexactMethodLevel; 
	CString m_searchCodeFragString;
	int     m_db_param_id;
	CComboBox m_regNormLvl;
    CComboBox m_inexactMTD; 
	bool    m_ok;

    CButton m_normTokCtrl;
	//CEdit   m_1stsegperCtrl;
	CEdit   m_occthrsCtrl;
	CEdit   m_targetAsmFileCtrl;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	
	CToolTipCtrl  m_ToolTip;
	bool          m_withSearchCode;
	CRichEditCtrl m_editSearchCodeFrag;
	CString       m_initialFragStr;

	DECLARE_MESSAGE_MAP()

public:
	CButton m_keepTempFileChkCtrl;
	BOOL m_keepTempFileChk;
	afx_msg void OnBnClickedCancel();
	CString m_dbPwd;
	CString m_dbName;
	CString m_dbUser;

	int m_paramsId;
	afx_msg void OnBnClickedChkinexact();

	void NewDetectDialog::updateParametersStaticText(int windSize, int stride, int regNormsLvl);
	afx_msg void OnBnClickedButtonGetParams();
};


